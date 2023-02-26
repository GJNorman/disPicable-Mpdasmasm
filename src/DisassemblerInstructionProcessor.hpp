//
//  DisassemblerInstructionProcessor.hpp
//
//
//  Created by Greg Norman on 10/2/2023.
//

#ifndef DisassemblerInstructionProcessor_hpp
#define DisassemblerInstructionProcessor_hpp

#include <iostream>

#include <vector>
#include <string>
#include "AssemblerFilePreprocessing.hpp"
#include "DisassembleSingleword.hpp"
#include "DisassembleDoubleWords.hpp"
#include "DisassemblerTableReads.hpp"
#include "DisassemblerFunctionStacks.hpp"
#include "DisassemblerLabels.hpp"
#include "PIC18_IS.h"

// generate assembly instructiond from OPCODEs
void generate_asm(std::vector<uint8_t> &MachineCode,                // data from Hex file
                  uint32_t &address,                                // current program address
                  std::vector<unsigned char> &HEX_FILE_CONTENTS,    // entire contents of hex file, in case we need to check the next word
                  uint64_t file_position,                           // current hex file offset
                  Converted_Assembly_Code &OutputAssemblyCode,      // output for Assembly code
                  PIC18F_FULL_IS &Instruction_Set,                  // Pic instruction set
                  bool bdisplayASM,                                 // debugging - display assembly output
                  bool bDisplayBinContents,                         // debugging - display opcode input
                  uint16_t Address_Upper_16_bits);                  // upper 16 bits of current address

void finalise_command(char *&command_for_prompt,                    // buffer for current instruction
                      Converted_Assembly_Code &OutputAssemblyCode,  // holds entire output program
                      PIC18F_FULL_IS &Instruction_Set,              // Pic instruction set
                      uint16_t Address_Upper_16_bits,               // upper 16 bits of address
                      uint32_t device_mem_size,                     // total available flash memory
                      uint16_t n );                                 // opcode
#endif /* DisassemblerInstructionProcessor_hpp */
