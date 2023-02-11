//
//  AssembleDoubleWord.hpp
//  
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

// returns a value only if the second word falls onto the next line of the hexfile
char * assemble_double_word_case(std::string &Assembly_Instruction, PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address, uint16_t &checksum);

#endif /* AssembleDoubleWord_hpp */
