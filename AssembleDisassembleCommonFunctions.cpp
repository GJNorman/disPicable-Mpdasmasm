//
//  AssembleDisassembleCommonFunctions.cpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#include "AssembleDisassembleCommonFunctions.hpp"

size_t check_against_list_of_instructions(std::vector<std::string> Instruction_list, const char *Input_string, uint8_t &outputindex)
{
    // find the end of the command
    size_t endOfCommand = FindNextSpaceCharacter(Input_string, 0) ;

    // for disassembler, format is '00007c52    EF33    GOTO 0x7C66 '
    for(uint8_t counter=0;counter<Instruction_list.size();counter++)
    {
        if(endOfCommand == Instruction_list[counter].size())
        {
            if(strncasecmp(Input_string,Instruction_list[counter].c_str(),endOfCommand)==0)
            {
                outputindex = counter;
                
                return endOfCommand;
            }
        }
    }
    return 0;
}

// prints data in binary, for debugging
void print_as_bits(uint8_t OutputBytes)
{
    printf(" " BYTE_TO_BINARY_PATTERN,BYTE_TO_BINARY(  OutputBytes));
    printf("\n");
}

