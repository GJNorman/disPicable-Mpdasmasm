//
//  AssembleDisassembleCommonFunctions.hpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#ifndef AssembleDisassembleCommonFunctions_hpp
#define AssembleDisassembleCommonFunctions_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <stdint.h>
#include "BasicFileEditing.hpp"

size_t check_against_list_of_instructions(std::vector<std::string> Instruction_list, const char *Input_string, uint8_t &outputindex);


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
(byte & 0x80 ? '1' : '0'), \
(byte & 0x40 ? '1' : '0'), \
(byte & 0x20 ? '1' : '0'), \
(byte & 0x10 ? '1' : '0'), \
(byte & 0x08 ? '1' : '0'), \
(byte & 0x04 ? '1' : '0'), \
(byte & 0x02 ? '1' : '0'), \
(byte & 0x01 ? '1' : '0')



void print_as_bits(uint8_t OutputBytes);
#endif /* AssembleDisassembleCommonFunctions_hpp */
