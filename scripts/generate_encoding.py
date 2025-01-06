#!/usr/bin/env python3

import os
import sys
import argparse
import csv
from pathlib import Path

# Add riscv-opcodes to Python path
script_dir = Path(__file__).parent
project_root = script_dir.parent
sys.path.append(str(project_root / 'third_party' / 'riscv-opcodes'))

from shared_utils import create_inst_dict

def load_arg_lut():
    """Load the argument lookup table from arg_lut.csv"""
    arg_lut = {}
    arg_lut_path = project_root / 'third_party' / 'riscv-opcodes' / 'arg_lut.csv'
    with open(arg_lut_path, 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            if not row:  # Skip empty rows
                continue
            name = row[0].strip('"')
            msb = int(row[1])
            lsb = int(row[2])
            arg_lut[name] = (msb, lsb)
    return arg_lut

def generate_header(instr_dict, arg_lut):
    template = """// Auto-generated RISC-V instruction encoding file
// DO NOT EDIT MANUALLY

#pragma once
#include <cstdint>
#include <cstddef>

namespace rvpin {
namespace encoding {

struct InstructionEncoding {
    const char* name;
    uint32_t mask;
    uint32_t match;
    const char* extension;
};

constexpr InstructionEncoding INSTRUCTION_ENCODINGS[] = {
%s
};

constexpr size_t NUM_ENCODINGS = sizeof(INSTRUCTION_ENCODINGS) / sizeof(InstructionEncoding);

} // namespace encoding
} // namespace rvpin
"""
    
    entries = []
    for name, inst in instr_dict.items():
        # Convert match and mask from hex strings to integers
        match = int(inst['match'], 16)
        mask = int(inst['mask'], 16)
        
        # The mask from create_inst_dict includes variable fields
        # We need to modify it to only include fixed fields
        # Variable fields should be 0 in the mask
        if 'variable_fields' in inst:
            # Each variable field is a string like 'rd', 'rs1', etc.
            # We need to look up their bit positions in arg_lut
            for field in inst['variable_fields']:
                if field in arg_lut:
                    msb, lsb = arg_lut[field]
                    # Clear the bits in the mask for this field
                    for bit in range(lsb, msb + 1):
                        mask &= ~(1 << bit)
        
        # Get the extension name (e.g., "rv32i" -> "RV32I")
        extension = inst['extension'][0].upper() if inst['extension'] else "UNKNOWN"
        
        entry = f'    {{"{name}", 0x{mask:08x}, 0x{match:08x}, "{extension}"}}'
        entries.append(entry)
    
    return template % ',\n'.join(entries)

def main():
    parser = argparse.ArgumentParser(description="Generate RISC-V instruction encodings")
    parser.add_argument('--isa', type=str, default='rv32i', help='RISC-V ISA string (e.g., rv32i, rv64i)')
    parser.add_argument('--include-pseudo', action='store_true', help='Include pseudo-instructions')
    args = parser.parse_args()
    
    # Convert ISA string to list of extensions
    # For rv32i we need both rv_i (common) and rv32_i (specific)
    # For rv64i we need both rv_i (common) and rv64_i (specific)
    extensions = []
    
    # Add common instructions
    for ext in args.isa[4:]:  # Skip "rv32" or "rv64"
        extensions.append(f"rv_{ext}")
    
    # Add ISA-specific instructions
    if args.isa.startswith('rv32'):
        prefix = 'rv32'
    elif args.isa.startswith('rv64'):
        prefix = 'rv64'
    else:
        prefix = 'rv'
        
    for ext in args.isa[4:]:  # Skip "rv32" or "rv64"
        extensions.append(f"{prefix}_{ext}")
    
    print(f"Using extensions: {extensions}")
    
    # Load argument lookup table
    arg_lut = load_arg_lut()
    print(f"Loaded {len(arg_lut)} argument positions")
    
    # Create instruction dictionary
    instr_dict = create_inst_dict(extensions, include_pseudo=args.include_pseudo)
    
    # Generate header file
    header_content = generate_header(instr_dict, arg_lut)
    header_path = project_root / 'src' / 'core' / 'encoding.hpp'
    
    # Create directories if they don't exist
    header_path.parent.mkdir(parents=True, exist_ok=True)
    
    # Write header file
    with open(header_path, 'w') as f:
        f.write(header_content)
    
    print(f"Generated {header_path} with {len(instr_dict)} instruction encodings for ISA {args.isa}")

if __name__ == '__main__':
    main()
