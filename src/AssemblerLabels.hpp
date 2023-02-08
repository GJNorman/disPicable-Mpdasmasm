//
//  AssemblerLabels.hpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#ifndef AssemblerLabels_hpp
#define AssemblerLabels_hpp

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "PIC18_IS.h"
#include "BasicFileEditing.hpp"
#include "AssemblerFilePreprocessing.hpp"
uint32_t translate_external_label_call_into_address(const char *Label_instruction);
char *translate_label_to_mem_address(const char *Label_instruction, bool CALL_USES_EXTERNAL_MEM, uint32_t Start_Address);


char *convert_label(const char *Label_instruction, uint32_t address, size_t startOfLabel, size_t endOfLabel);

void ConvertLabelToAddress(std::string &Assembly_Instruction, uint32_t adress);
#endif /* AssemblerLabels_hpp */
