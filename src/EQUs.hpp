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

// grab the second most recently found EQU, needed for MOVFF instructions
std::string getSecondMostRecentEQU();

// and their addresses
uint16_t getAddressOfsecondMostRecentEQU();

uint16_t getAddressOfMostRecentEQU();

void clearMostRecentEQU();

// reset vector
void clearEQU();

// mamual set the most recent EQU -> used for setting to a literal address
void setEQU(uint32_t regAddress);

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

// SFR bank is calculated using equ's from the header file
// this is an offset for SFR registers mapped to the access bank only
uint16_t getSFRAccessBankOffset();

// SFRs can use more than one bank
// this function will return the highest, this is so that memory can be allocated for RAM tracking
uint16_t getHighestSFRBank();



#endif /* EQUs_hpp */
