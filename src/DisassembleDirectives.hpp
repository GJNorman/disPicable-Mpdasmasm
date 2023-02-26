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


void deal_with_non_program_data(std::vector<uint8_t> Data_bytes,                // input data
                                const char *Tag,                                // name for datafield 'EEPROM', 'DEVICE' etc.
                                uint32_t address,                               // program address
                                uint16_t Address_Upper_16_bits,
                                Converted_Assembly_Code &OutputAssemblyCode,    // output file
                                bool include_address_line ) ;                   // for first entry
#endif /* DisassembleDirectives_hpp */
