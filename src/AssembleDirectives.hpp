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
                      uint32_t &address,
                      bool &check_sum_required,
                      uint16_t &check_sum,
                      PIC18F_FULL_IS &Instruction_Set,
                      uint16_t &address_upper_16bits,
                      uint32_t &START_ADDRESS);
#endif /* AssembleDirectives_hpp */
