//
//  AssembleSingleWord.hpp
//  annoyed
//
//  Created by Greg Norman on 28/1/2023.
//

#ifndef AssembleSingleWord_hpp
#define AssembleSingleWord_hpp

#include <iostream>
#include <vector>
#include <string>
#include <stdint.h>
#include "PIC18_IS.h"
#include "AssemblerArguments.hpp"
#include "AssemblerAddressAndChecksum.hpp"

void assemble_single_word_case(const char *Assembly_Instruction,
                               PIC18F_FULL_IS & Instruction_Set,
                               uint64_t pos,
                               bool find_d_or_b_parameter,
                               bool find_a_or_s_parameter,
                               uint32_t address,
                               uint16_t &checksum,
                               uint16_t f_mask);                    // some commands use 8 bit, some use 12 bit


void Assemble_PIC18f_SingleWord(std::string &Assembly_Instruction,
                                PIC18F_FULL_IS & Instruction_Set,
                                uint64_t pos,
                                uint32_t address,
                                uint16_t &checksum);

#endif /* AssembleSingleWord_hpp */
