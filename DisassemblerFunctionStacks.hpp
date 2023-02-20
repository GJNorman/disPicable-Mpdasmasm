//
//  DisassemblerFunctionStacks.hpp
//  
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
#include "EQUs.hpp"

enum FuncionArgumentTypes{
    literal = 0,
    RAM = 1
};

typedef struct{
    uint8_t type;               // literal, RAM register etc.
    uint16_t value;
    std::string ArgumentName;
}FunctionStack_t;

typedef struct{
    std::string Reg;
    uint32_t address;    
}RegistersTracker_t;

typedef struct{
    uint8_t value;          // literal value
    uint8_t bank;           // ram bank of address
    uint8_t type;           // literal or register
    RegistersTracker_t REG; // for registers
}WREG_Follower_t;


typedef struct{
    uint32_t address_of_call;
    uint32_t address_of_function;
}ListOfFunctionCalls_t;


void watchFunctionStacks(char* Instruction,Converted_Assembly_Code &OutputAssemblyCode);
void HighlightFSRs(Converted_Assembly_Code &OutputAssemblyCode);

std::string grabFSRList(int funcNum);
#endif /* DisassemblerFunctionStacks_hpp */
