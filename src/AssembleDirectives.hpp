//
//  AssembleDirectives.hpp
//
//
//  Created by Greg Norman on 28/1/2023.
//

#ifndef AssembleDirectives_hpp
#define AssembleDirectives_hpp

#include <iostream>
#include "AssemblerMachineCodeWriter.hpp"
#include "BasicFileEditing.hpp"
#include "AssemblerAddressAndChecksum.hpp"
#include "PIC18_IS.h"

void processDirective(std::string Assembly_Instruction,
                      Address_And_Checksum_t &Address,
                      PIC18F_FULL_IS &Instruction_Set);
#endif /* AssembleDirectives_hpp */
