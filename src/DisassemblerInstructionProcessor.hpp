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
void generate_asm(std::vector<uint8_t> &MachineCode,
                  uint32_t &address,
                  std::vector<unsigned char> &HEX_FILE_CONTENTS,
                  uint64_t file_position,
                  bool &final_byte_is_double_word,
                  Converted_Assembly_Code &OutputAssemblyCode,
                  PIC18F_FULL_IS &Instruction_Set,
                  bool bdisplayASM,
                  bool bDisplayBinContents,
                  uint16_t Address_Upper_16_bits);

void finalise_command(char *&command_for_prompt,
                      Converted_Assembly_Code &OutputAssemblyCode,
                      const char *Description,
                      uint16_t Address_Upper_16_bits,
                      uint32_t device_mem_size,
                      uint16_t n );
#endif /* DisassemblerInstructionProcessor_hpp */
