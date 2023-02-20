//
//  DisassemblerLabels.hpp
//  
//
//  Created by Greg Norman on 2/2/2023.
//

#ifndef DisassemblerLabels_hpp
#define DisassemblerLabels_hpp

#include <iostream>
#include "AssemblerFilePreprocessing.hpp"   //Converted Assembly Code class
#include "DisassemblerModifyByAddress.hpp"  //for adding labels to completed program

// insert labels into completed code
void addLabelsToAssemblyCode(Converted_Assembly_Code &OutputAssemblyCode);

// add symbolic label to branching instruction
void addLabeltoBranchingInstruction(Converted_Assembly_Code &OutputAssemblyCode,        // full program being created
                                    const char *command_for_prompt,                     // currently constructed line
                                    const char *Branching_Command,                      // the instruction detected in the line
                                    uint32_t device_mem_size,                           // MCU's flash memory size
                                    size_t end_of_meta_data);                           // end of meta-data in "command_for_prompt"
                                                                                        // the meta-data is the memory address and opcode and has a fixed position
#endif /* DisassemblerLabels_hpp */
