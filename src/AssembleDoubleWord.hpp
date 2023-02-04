//
//  AssembleDoubleWord.hpp
//  annoyed
//
//  Created by Greg Norman on 28/1/2023.
//

#ifndef AssembleDoubleWord_hpp
#define AssembleDoubleWord_hpp

#include <iostream>
#include <vector>
#include <string>
#include "PIC18_IS.h"
#include "AssemblerAddressAndChecksum.hpp"
#include "AssemblerMachineCodeWriter.hpp"
#include "AssemblerArguments.hpp"
char * assemble_double_word_case(std::string &Assembly_Instruction, PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address, uint16_t &checksum);


void find_FILE_values_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address, uint16_t &fs, uint16_t &fd);
#endif /* AssembleDoubleWord_hpp */
