//
//  AssemblerInstructionProcessor.hpp
//  
//
//  Created by Greg Norman on 10/2/2023.
//

#ifndef AssemblerInstructionProcessor_hpp
#define AssemblerInstructionProcessor_hpp

#include <iostream>
#include "PIC18_IS.h"
#include <string>
#include <vector>
#include "BasicFileEditing.hpp"
#include "AssemblerMachineCodeWriter.hpp"
#include "AssemblerAddressAndChecksum.hpp"
#include "AssembleSingleWord.hpp"
#include "AssembleDoubleWord.hpp"
#include "AssemblerLabels.hpp"
bool processInstruction(std::string &Assembly_Instruction,      // current line being processed
                        PIC18F_FULL_IS &Instruction_Set,        // PIC18 instruction set
                        Address_And_Checksum_t &Address);

#endif /* AssemblerInstructionProcessor_hpp */
