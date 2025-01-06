#!/bin/bash

# Exit on error
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Create third_party directory if it doesn't exist
mkdir -p "$PROJECT_ROOT/third_party"

# Clone or update riscv-opcodes
if [ ! -d "$PROJECT_ROOT/third_party/riscv-opcodes" ]; then
    echo "Cloning riscv-opcodes repository..."
    git clone https://github.com/riscv/riscv-opcodes.git "$PROJECT_ROOT/third_party/riscv-opcodes"
else
    echo "Updating riscv-opcodes repository..."
    cd "$PROJECT_ROOT/third_party/riscv-opcodes"
    git pull origin master
fi

# Generate instruction encoding header for RV32I base ISA
echo "Generating instruction encoding header..."
python3 "$PROJECT_ROOT/scripts/generate_encoding.py" --isa rv32i --include-pseudo

echo "Setup completed successfully!"
