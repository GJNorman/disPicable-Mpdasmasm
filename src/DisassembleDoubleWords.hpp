//
//  DisassembleDoubleWords.hpp
//  annoyed
//
//  Created by Greg Norman on 3/2/2023.
//

#ifndef DisassembleDoubleWords_hpp
#define DisassembleDoubleWords_hpp

#include <iostream>
#include <string>
#include <stdint.h>
#include "PIC18_IS.h"
#include "EQUs.hpp"
bool disassemble_PIC18F_DoubleWord(uint16_t MachineCode_byte ,uint16_t MachineCode_byte2, std::string opcode_mnem, uint8_t CASE_TYPE, char*& command_for_prompt, uint64_t program_counter);
#endif /* DisassembleDoubleWords_hpp */
