//
//  Assembler.hpp
//  annoyed
//
//  Created by Greg Norman on 27/1/2023.
//

#ifndef Assembler_hpp
#define Assembler_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdint.h>
#include "PIC18_IS.h"
#include "BasicFileEditing.hpp"
#include "MacroEngine.hpp"
#include "AssembleDisassembleCommonFunctions.hpp"
#include "AssemblerLabels.hpp"
#include "AssemblerFilePreprocessing.hpp"
#include "AssembleSingleWord.hpp"
#include "AssembleDoubleWord.hpp"
#include "AssembleDirectives.hpp"
#include "AssemblerMachineCodeWriter.hpp"

#include "Timer.h"

void Assemble(const char *inputfiledir, bool bDisplayBinContents,bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set);

bool processInstruction(std::string &Assembly_Instruction,      // current line being processed
                        PIC18F_FULL_IS &Instruction_Set,        // PIC18 instruction set
                        uint32_t &address,                      // program counter
                        bool &check_sum_required,               // check sum needed at end of each line of hex file
                        uint16_t &check_sum,                    // current check sum
                        uint16_t &address_upper_16bits,         // for address header
                        uint32_t START_ADDRESS);


void processDirective(std::string Assembly_Instruction,
                      uint32_t &address,
                      bool &check_sum_required,
                      uint16_t &check_sum,
                      PIC18F_FULL_IS &Instruction_Set,
                      uint16_t &address_upper_16bits,
                      uint32_t &START_ADDRESS);

#endif /* Assembler_hpp */
