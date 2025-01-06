#include "engine.hpp"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <iomanip>

namespace rvpin {

// ELF constants
constexpr uint32_t EI_MAG0 = 0x7f;
constexpr uint32_t EI_MAG1 = 'E';
constexpr uint32_t EI_MAG2 = 'L';
constexpr uint32_t EI_MAG3 = 'F';
constexpr uint32_t EI_CLASS = 4;
constexpr uint32_t ELFCLASS32 = 1;
constexpr uint32_t ELFCLASS64 = 2;
constexpr uint32_t EI_DATA = 5;
constexpr uint32_t ELFDATA2LSB = 1;
constexpr uint32_t ELFDATA2MSB = 2;
constexpr uint32_t EM_RISCV = 243;

// ELF header for both 32-bit and 64-bit
struct Elf64_Ehdr {
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

// Section header for both 32-bit and 64-bit
struct Elf64_Shdr {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
};

Engine& Engine::getInstance() {
    static Engine instance;
    return instance;
}

bool Engine::initialize(int argc, char* argv[]) {
    if (argc < 1) {
        return false;
    }
    
    program_path_ = argv[0];
    decoder_ = std::make_unique<Decoder>();
    return true;
}

void Engine::registerBeforeInstruction(InstrumentationCallback callback) {
    before_callbacks_.push_back(std::move(callback));
}

void Engine::registerAfterInstruction(InstrumentationCallback callback) {
    after_callbacks_.push_back(std::move(callback));
}

const Instruction* Engine::getCurrentInstruction() const {
    return current_instruction_.get();
}

bool Engine::isElfFile(std::ifstream& file) {
    unsigned char magic[16];  // Read entire e_ident
    if (!file.read(reinterpret_cast<char*>(magic), sizeof(magic))) {
        std::cerr << "Failed to read ELF magic\n";
        return false;
    }
    file.seekg(0);  // Reset file position
    
    // Check magic number
    if (magic[0] != EI_MAG0 || magic[1] != EI_MAG1 ||
        magic[2] != EI_MAG2 || magic[3] != EI_MAG3) {
        std::cerr << "Invalid ELF magic number\n";
        return false;
    }
    
    // Check if it's a 64-bit ELF file
    if (magic[EI_CLASS] != ELFCLASS64) {
        std::cerr << "Not a 64-bit ELF file\n";
        return false;
    }
    
    // Check endianness (RISC-V is little endian)
    if (magic[EI_DATA] != ELFDATA2LSB) {
        std::cerr << "Not a little-endian ELF file\n";
        return false;
    }
    
    return true;
}

std::vector<uint32_t> Engine::loadTextSection(std::ifstream& program) {
    std::vector<uint32_t> instructions;
    
    try {
        // Read ELF header
        Elf64_Ehdr header;
        program.seekg(0);
        if (!program.read(reinterpret_cast<char*>(&header), sizeof(header))) {
            throw std::runtime_error("Failed to read ELF header");
        }
        
        // Check if it's a RISC-V ELF file
        if (header.e_machine != EM_RISCV) {
            throw std::runtime_error("Not a RISC-V ELF file");
        }
        
        if (header.e_shnum == 0 || header.e_shstrndx >= header.e_shnum) {
            throw std::runtime_error("Invalid ELF header");
        }
        
        // Read section headers
        std::vector<Elf64_Shdr> section_headers(header.e_shnum);
        program.seekg(header.e_shoff);
        if (!program.read(reinterpret_cast<char*>(section_headers.data()),
                         header.e_shnum * sizeof(Elf64_Shdr))) {
            throw std::runtime_error("Failed to read section headers");
        }
        
        // Read section names
        std::vector<char> shstrtab;
        const auto& shstrtab_hdr = section_headers[header.e_shstrndx];
        if (shstrtab_hdr.sh_size == 0) {
            throw std::runtime_error("Empty string table");
        }
        shstrtab.resize(shstrtab_hdr.sh_size);
        program.seekg(shstrtab_hdr.sh_offset);
        if (!program.read(shstrtab.data(), shstrtab_hdr.sh_size)) {
            throw std::runtime_error("Failed to read string table");
        }
        
        // Find and load .text section
        for (const auto& shdr : section_headers) {
            if (shdr.sh_name >= shstrtab_hdr.sh_size) {
                continue;
            }
            
            const char* name = &shstrtab[shdr.sh_name];
            if (strcmp(name, ".text") == 0) {
                if (shdr.sh_size == 0) {
                    throw std::runtime_error("Empty text section");
                }
                
                // Read text section
                std::vector<uint8_t> text(shdr.sh_size);
                program.seekg(shdr.sh_offset);
                if (!program.read(reinterpret_cast<char*>(text.data()), shdr.sh_size)) {
                    throw std::runtime_error("Failed to read text section");
                }
                
                // Convert to instructions (handle both 32-bit and 16-bit instructions)
                for (size_t i = 0; i < text.size();) {
                    uint32_t inst = 0;
                    
                    // Check if it's a compressed instruction (16-bit)
                    uint16_t first_half = (text[i+1] << 8) | text[i];
                    bool is_compressed = (first_half & 0x3) != 0x3;
                    
                    if (is_compressed && i + 2 <= text.size()) {
                        // 16-bit instruction
                        inst = first_half;
                        i += 2;
                    } else if (i + 4 <= text.size()) {
                        // 32-bit instruction
                        inst = (text[i+3] << 24) | (text[i+2] << 16) | 
                               (text[i+1] << 8) | text[i];
                        i += 4;
                    } else {
                        break;  // Not enough bytes left
                    }
                    
                    instructions.push_back(inst);
                }
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading text section: " << e.what() << "\n";
    }
    
    return instructions;
}

int Engine::run() {
    try {
        // Load the program
        std::ifstream program(program_path_, std::ios::binary);
        if (!program) {
            std::cerr << "Failed to open program: " << program_path_ << "\n";
            return 1;
        }
        
        // Check if it's an ELF file
        if (!isElfFile(program)) {
            std::cerr << "Not an ELF file: " << program_path_ << "\n";
            return 1;
        }
        
        // Load instructions from .text section
        auto instructions = loadTextSection(program);
        if (instructions.empty()) {
            std::cerr << "No instructions loaded\n";
            return 1;
        }
        
        // Execute each instruction
        for (uint32_t inst : instructions) {
            current_instruction_ = decoder_->decode(inst);
            if (!current_instruction_) {
                // Skip invalid instructions
                continue;
            }
            
            // Call before callbacks
            for (const auto& callback : before_callbacks_) {
                callback(*current_instruction_);
            }
            
            // TODO: Actually execute the instruction
            
            // Call after callbacks
            for (const auto& callback : after_callbacks_) {
                callback(*current_instruction_);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during execution: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

} // namespace rvpin
