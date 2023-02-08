//
//  DisassembleDirectives.hpp
//  
//
//  Created by Greg Norman on 2/2/2023.
//

#ifndef DisassembleDirectives_hpp
#define DisassembleDirectives_hpp

#include <stdio.h>
#include "AssemblerFilePreprocessing.hpp"
#include <stdint.h>
#include <string>
#include <vector>
#include "PIC18_IS.h"
void add_ORG_to_asm_file(uint32_t address,Converted_Assembly_Code &OutputAssemblyCode);


void deal_with_non_program_data(std::vector<uint8_t> Data_bytes,
                                const char *Tag,
                                uint32_t address,
                                uint16_t Address_Upper_16_bits,
                                Converted_Assembly_Code &OutputAssemblyCode,
                                bool include_address_line );
#endif /* DisassembleDirectives_hpp */
