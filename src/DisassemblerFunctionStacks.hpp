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
#include "DisassemblerRamTracker.hpp"


typedef struct{
    uint8_t type;               // literal, RAM register etc.
    uint16_t value;
    std::string ArgumentName;
}FunctionStack_t;



typedef struct{
    uint32_t address_of_call;
    uint32_t address_of_function;
}ListOfFunctionCalls_t;


// monitor the use of software stacks (POSTINC) etc, to create a list of registers used in each function
void watchFunctionStacks(char* Instruction,Converted_Assembly_Code &OutputAssemblyCode,PIC18F_FULL_IS &Instruction_Set);

// add a list of special function registers into the code
void HighlightSFRs(Converted_Assembly_Code &OutputAssemblyCode);

// get a list of Special function registers used inside a certain function
std::string grabSFRList(int funcNum);

// get a list of literal registers used inside a function
std::string grabLiteralRegisterList(int funcNum);
#endif /* DisassemblerFunctionStacks_hpp */
