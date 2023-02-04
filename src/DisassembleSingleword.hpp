//
//  DisassembleSingleword.hpp
//  annoyed
//
//  Created by Greg Norman on 2/2/2023.
//

#ifndef DisassembleSingleword_hpp
#define DisassembleSingleword_hpp

#include <iostream>
#include <stdint.h>
#include "PIC18_IS.h"
#include "DisassemblerParameters.hpp"
#include "EQUs.hpp"
void disassemble_with_PIC18f_SingleWord(uint16_t MachineCode_byte,              // current byte being disassembled
                                        PIC18F_FULL_IS &Instruction_Set,        // pic IS
                                        uint8_t IS_Pos,                         // offset for detected command
                                        char*&command_for_prompt,               // disassembled data is stored here
                                        uint32_t program_counter);              // current program address
#endif /* DisassembleSingleword_hpp */
