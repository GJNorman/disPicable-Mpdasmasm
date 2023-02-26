//
//  EQUs.cpp
//  
//
//  Created by Greg Norman on 2/2/2023.
//

#include "EQUs.hpp"

// holds all of the EQU variables defined by the user files
static std::vector<EQU_list_t> EQU_List;
std::string MostRecentEQU;                  // the last EQU found in the list from either the disassembler or assembler
std::string secondMostRecentEQU;            // we need this because of MOVFF instructions, otherwise we will only record the second file

static size_t LastAccessEquOffset = 0;      // records vector position of MostRecentEQU


std::string getMostRecentEQU()
{
    return MostRecentEQU;
}
std::string getSecondMostRecentEQU()
{
    return secondMostRecentEQU;
}
void clearMostRecentEQU()
{
    MostRecentEQU.clear();
    secondMostRecentEQU.clear();
}
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
void setEQU(uint32_t regAddress)
{
    char temp[10];
    
    snprintf(temp,sizeof(temp),"0x%x",regAddress);
    
    MostRecentEQU = temp;
}
// take a register address from a "FILE" command and return the associated "EQU"
std::string processEQUforDisassembler(uint32_t regAddress, uint32_t mask)
{
    char temp[10];
    
    snprintf(temp,sizeof(temp),"0x%x",regAddress&mask);
    secondMostRecentEQU = MostRecentEQU;
    MostRecentEQU = temp;
    LastAccessEquOffset = 0;
    for(auto Equis: EQU_List)
    {
        uint32_t address = strtol(Equis.Address.c_str(),NULL,16) &0xffffff;
        
        if(address == regAddress)
        {
            // header file bit fields indicated by underscores
            if(Equis.Tag.find("_")==std::string::npos)
            {
                MostRecentEQU = Equis.Tag;
                break;
            }
        }
        LastAccessEquOffset++;
    }
    return MostRecentEQU;
}

std::string findEQUBitForDisassembler(int bitNum)
{
    std::string returnValue;
    
    char temp[10];
    
    snprintf(temp, sizeof(temp), "%u", bitNum);
    
    returnValue = temp;
    for(size_t index=0; index<EQU_List[LastAccessEquOffset].bitfields.size(); index++)
    {
        if(bitNum == strtol(EQU_List[LastAccessEquOffset].bitfieldAddresses[index].c_str(), NULL,16))
        {
            returnValue=EQU_List[LastAccessEquOffset].bitfields[index];
            
            break;
        }
    }

    return returnValue;
}

// take a variable name and return the address
uint32_t processEQUforAssembler(std::string RegisterName)
{
    LastAccessEquOffset = 0;
    for(auto Equis: EQU_List)
    {
        if(Equis.Tag.size() == RegisterName.size())
        {
            if(strncasecmp(Equis.Tag.c_str(),RegisterName.c_str(),Equis.Tag.size())==0)
            {
                secondMostRecentEQU = MostRecentEQU;
                MostRecentEQU = Equis.Tag;
                //replace with the register ID
                uint32_t address = strtol(Equis.Address.c_str(),NULL,16) &0xffffff;
                return address;
            }
        }
        LastAccessEquOffset++;
    }
    
    // assume this is jsut a literal value
    return strtol(RegisterName.c_str(),NULL,16) &0xffffff;
}
uint32_t processEQUBitForAssembler(std::string BitName)
{
    uint32_t bitNum = strtol(BitName.c_str(), NULL, 16) &0xffffff;

    for(size_t index=0; index<EQU_List[LastAccessEquOffset].bitfields.size(); index++)
    {
        if(EQU_List[LastAccessEquOffset].bitfields[index].size() == BitName.size())
        {
            if(strncmp(EQU_List[LastAccessEquOffset].bitfields[index].c_str(),BitName.c_str(),EQU_List[LastAccessEquOffset].bitfields[index].size())==0)
            {
                bitNum =  strtol(EQU_List[LastAccessEquOffset].bitfieldAddresses[index].c_str(), NULL,16) &0xffffff;
                
                break;
            }
        }
    }

    return bitNum;
}
