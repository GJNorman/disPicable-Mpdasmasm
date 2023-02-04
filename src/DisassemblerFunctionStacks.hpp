//
//  DisassemblerFunctionStacks.hpp
//  annoyed
//
//  Created by Greg Norman on 3/2/2023.
//

#ifndef DisassemblerFunctionStacks_hpp
#define DisassemblerFunctionStacks_hpp

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include "AssemblerFilePreprocessing.hpp"
typedef struct{
    uint8_t type;               // int, register etc.
    uint16_t value;
    std::string ArgumentName;
}FunctionStack_t;

enum FuncionArgumentTypes{
    INTEGER = 0,
    REGISTER = 1,
};
void watchFunctionStacks(char* Instruction,Converted_Assembly_Code &OutputAssemblyCode);

#endif /* DisassemblerFunctionStacks_hpp */
