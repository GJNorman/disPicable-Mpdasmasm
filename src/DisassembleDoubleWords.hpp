//
//  DisassembleDoubleWords.hpp
//  
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
bool disassemble_PIC18F_DoubleWord(uint16_t MachineCode_byte ,
                                   uint16_t MachineCode_byte2,
                                   PIC18F_FULL_IS &Instruction_Set,
                                   char*& command_for_prompt,
                                   size_t commandForPromptMaxSize,
                                   uint64_t program_counter);
#endif /* DisassembleDoubleWords_hpp */
