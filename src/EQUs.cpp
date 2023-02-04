//
//  EQUs.cpp
//  annoyed
//
//  Created by Greg Norman on 2/2/2023.
//

#include "EQUs.hpp"

// holds all of the EQU variables defined by the user files
static std::vector<EQU_list_t> EQU_List;

void clearEQU()
{
    EQU_List.clear();
}
// for debugging
void printEQUs()
{
    std::cout << "************************************************************\n";
    std::cout << "\t\t\tEQU list\n";
    std::cout << "************************************************************\n";
    for (auto it: EQU_List)
    {
        std::cout << "'" << it.Tag << "' @ '" << it.Address << "'\n";
    }
}
// bit fields as defined like so
// ANSEL_ANS0_POSN                          equ 0000h
// modifies input string to remove "ANSEL_" and "_POSN"
// returns the index of the parent in the EQU_List, or otherwise ~0
size_t findBitFieldParent(std::string &BitFieldTag)
{
    size_t p1 = 0;
    
    size_t parentRegPos = BitFieldTag.find("_");
    
    std::string parent = BitFieldTag.substr(0,parentRegPos);
    
    for(auto it: EQU_List)
    {
        if(it.Tag.size() == parent.size())
        {
            if(strncmp(it.Tag.c_str(), parent.c_str(), parent.size()) == 0)
            {
                size_t posn = BitFieldTag.find("_POSN");
                
                if(posn != std::string::npos)
                {
                    parentRegPos++; //move past _
                    BitFieldTag = BitFieldTag.substr(parentRegPos,posn-parentRegPos);
                }
                return p1 ;
            }
        }
        p1++;
    }
    p1 = ~0;
    return p1;
}
void addNewEQU(std::string Assembly_Instruction)
{
    // example
    // TagName      equ     0007h
    // AnotherName  equ     0x500
    // leading spaces are already removed
    size_t CheckForEqu = Assembly_Instruction.find("equ");
    
    if(CheckForEqu != std::string::npos)
    {
        size_t TagName = FindNextSpaceCharacter(Assembly_Instruction.c_str(), 0);
        
        EQU_list_t NewEntry ;
        
        NewEntry.Tag = Assembly_Instruction.substr(0,TagName);
        // find the address
        
        CheckForEqu += strlen("equ");
        
        CheckForEqu = CheckForLeadingSpaces(Assembly_Instruction.c_str(), CheckForEqu);
        
        size_t endOfAddress = FindNextSpaceCharacter(Assembly_Instruction, CheckForEqu);
        
        NewEntry.Address = Assembly_Instruction.substr(CheckForEqu,endOfAddress-CheckForEqu);
        
        /*
            check if this is a bitfield or a register
         */
        if(NewEntry.Tag.find("_POSN") == std::string::npos)  // _POSN indicates bitfield position
        {
            EQU_List.push_back(NewEntry);
        }
        else
        {
            size_t parentPos = findBitFieldParent(NewEntry.Tag);
            
            EQU_List[parentPos].bitfields.push_back(NewEntry.Tag);
            EQU_List[parentPos].bitfieldAddresses.push_back(NewEntry.Address);
            
        }
    }
}

// take a register address from a "FILE" command and return the associated "EQU"
std::string processEQUforDisassembler(uint32_t regAddress, uint32_t mask)
{
    std::string RegName;
    
    char temp[10] = "";
    
    snprintf(temp,sizeof(temp),"0x%x",regAddress&mask);
    
    RegName.resize(strlen(temp)+1);
    RegName = temp;
    
    for(auto Equis: EQU_List)
    {
        uint32_t address = strtol(Equis.Address.c_str(),NULL,16) &0xffffff;
        
        if(address == regAddress)
        {
            // header file bit fields indicated by underscores
            if(Equis.Tag.find("_")==std::string::npos)
            {
                RegName = Equis.Tag;
                break;
            }
        }
    }
    
    return RegName;
}
// take a variable name and return the address
uint32_t processEQUforAssembler(std::string &RegisterName)
{
    for(auto Equis: EQU_List)
    {
        if(Equis.Tag.size() == RegisterName.size())
        {
            if(strncasecmp(Equis.Tag.c_str(),RegisterName.c_str(),Equis.Tag.size())==0)
            {
                //replace with the register ID
                uint32_t address = strtol(Equis.Address.c_str(),NULL,16) &0xffffff;
                
                return address;
            }
        }
    }
    
    return ~0;
}
