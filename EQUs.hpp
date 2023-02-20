//
//  EQUs.hpp
//  
//
//  Created by Greg Norman on 2/2/2023.
//

#ifndef EQUs_hpp
#define EQUs_hpp

#include <iostream>
#include <stdint.h>
#include <vector>
#include <string>
#include "BasicFileEditing.hpp"
// for holding user 'equ's
// i.e.     myVar   equ     57h
typedef struct{
    std::string Tag;                                // name of variable
    std::string Address;                            // address of variable
    std::vector<std::string> bitfields;             // bit field declarations for reg
    std::vector<std::string> bitfieldAddresses;
}EQU_list_t;

enum EQU_Types{
    EQU_bf=0,
    EQU_Reg=1,
};
// grab the previously found equ
std::string getMostRecentEQU();

void clearMostRecentEQU();

// reset vector
void clearEQU();

// copy equ statement into EQU_list_t vector
void addNewEQU(std::string Assembly_Instruction);

// compares the register named in a command to the list of equ's
// returns the address if successful, otherwise ~0
uint32_t processEQUforAssembler(std::string RegisterName);

uint32_t processEQUBitForAssembler(std::string BitName);

//convert a register address into a variable name
std::string processEQUforDisassembler(uint32_t regAddress, uint32_t mask);

std::string findEQUBitForDisassembler(int bitNum);
// std::cout EQU_list_t vector
void printEQUs();

#endif /* EQUs_hpp */
