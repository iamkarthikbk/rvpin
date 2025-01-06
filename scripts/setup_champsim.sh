#!/bin/bash

# Exit on error
set -e

echo "Setting up ChampSim dependencies..."

# Create third_party directory if it doesn't exist
mkdir -p third_party

# Clone or update ChampSim
if [ ! -d "third_party/champsim" ]; then
    echo "Cloning ChampSim..."
    git clone https://github.com/ChampSim/ChampSim.git third_party/champsim
else
    echo "Updating ChampSim..."
    cd third_party/champsim
    git fetch origin
    git reset --hard origin/master
    cd ../..
fi

cd third_party/champsim

# Initialize and update vcpkg
echo "Setting up vcpkg..."
if [ ! -d "vcpkg" ]; then
    git clone https://github.com/Microsoft/vcpkg.git
fi

# Create vcpkg manifest file
cat > vcpkg.json << EOL
{
  "name": "champsim",
  "version": "1.0.0",
  "dependencies": [
    "catch2",
    "fmt",
    "nlohmann-json"
  ]
}
EOL

# Install vcpkg and dependencies
./vcpkg/bootstrap-vcpkg.sh
./vcpkg/vcpkg install

# Get vcpkg include path
VCPKG_ROOT=$PWD/vcpkg
VCPKG_INSTALLED_DIR=$VCPKG_ROOT/installed/arm64-osx
VCPKG_INCLUDE_PATH=$VCPKG_INSTALLED_DIR/include
VCPKG_LIB_PATH=$VCPKG_INSTALLED_DIR/lib

# Create options files with proper include paths
echo "Configuring ChampSim..."

# Create module.options file
cat > module.options << EOL
-I${VCPKG_INCLUDE_PATH}
-I.
-Iinc
-Isrc
-L${VCPKG_LIB_PATH}
-lfmt
EOL

# Create global.options file
cat > global.options << EOL
-std=c++17
-I${VCPKG_INCLUDE_PATH}
-I.
-Iinc
-Isrc
-L${VCPKG_LIB_PATH}
EOL

# Create absolute.options file
cat > absolute.options << EOL
-I${VCPKG_INCLUDE_PATH}
-I.
-Iinc
-Isrc
-L${VCPKG_LIB_PATH}
EOL

# Configure and build ChampSim
echo "Building ChampSim..."
./config.sh champsim_config.json
CPLUS_INCLUDE_PATH="${VCPKG_INCLUDE_PATH}:$PWD/inc:$PWD/src" LIBRARY_PATH="${VCPKG_LIB_PATH}" make

# Create symbolic links
cd ../..
echo "Creating symbolic links..."
mkdir -p include/champsim
ln -sf $(pwd)/third_party/champsim/inc/* include/champsim/
ln -sf $(pwd)/third_party/champsim/src/*.h include/champsim/

# Copy ChampSim libraries
echo "Setting up libraries..."
mkdir -p lib
cp third_party/champsim/bin/champsim lib/
cp third_party/champsim/lib/*.a lib/ 2>/dev/null || true

echo "ChampSim setup complete!"
