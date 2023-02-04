//
//  AssemblerLabels.cpp
//  annoyed
//
//  Created by Greg Norman on 27/1/2023.
//

#include "AssemblerLabels.hpp"


extern Converted_Assembly_Code AssemblyCode;


uint32_t translate_external_label_call_into_address(const char *Label_instruction)
{
    uint32_t address=0;

    char *ADDR = return_substring(Label_instruction, "0x", NULL, 0, strlen("0x"), 0);

    address = strtol (ADDR,NULL,16) & 0xffffffff;

    free(ADDR);

    return address;
}


// takes the label in a branching/calling instruction and converts to a memory address
char *translate_label_to_mem_address(const char *Label_instruction, bool CALL_USES_EXTERNAL_MEM, uint32_t Start_Address)
{
    // find the end of the instruction
    size_t endOfPicInstruction = FindNextSpaceCharacter(Label_instruction, 0);
    
    // and the start of the label
    size_t findStartOfLabel = CheckForLeadingSpaces(Label_instruction, endOfPicInstruction);
    
    size_t findEndOfLabel = FindEndOfCurrentWord(Label_instruction, findStartOfLabel+1);
    
    char *LABEL = copy_out_substring( findStartOfLabel,findEndOfLabel,Label_instruction);
    if(LABEL==NULL)
    {
        std::cout << "Could Not Parse Label on Instruction: " << Label_instruction << std::endl;
        exit(0);
    }

    //uint32_t address=Start_Address;
    /*if(CALL_USES_EXTERNAL_MEM == true)
    {
        free(LABEL);
        address=translate_external_label_call_into_address(Label_instruction);
        LABEL = convert_label(Label_instruction,address);
        return LABEL;
    }*/

    //find the address of the actual label
    for(auto it = 0; it<AssemblyCode.LABEL_POSITIONS.size();it++)
    {
        size_t compareSize =  AssemblyCode.LABEL_STRINGS[it].size();
        
        //the label may have a colon at the end, this will not be in the label used by the bracnhing instruction
        
        //so we need to remove it from the comparison if present
        
        if(AssemblyCode.LABEL_STRINGS[it][compareSize-1] == ':')
        {
            compareSize--;
        }
        
        if(strlen(LABEL) == compareSize)
        {
            if(strncasecmp(AssemblyCode.LABEL_STRINGS[it].c_str(),LABEL,strlen(LABEL))==0)
            {
                free(LABEL);
                
                //we've found the address
                LABEL = convert_label(Label_instruction,(uint32_t)AssemblyCode.LABEL_POSITIONS[it],findStartOfLabel,findEndOfLabel);

                return LABEL;
            }
        }
    }

    free(LABEL);
    
    printf("Couldn't find label position: %s\n",Label_instruction);
    exit(0);
    return NULL;
}
// remove the symbolic label and replace it with an address
// change GOTO LOOP1
//   into GOTO 0x100
char *convert_label(const char *Label_instruction, uint32_t address, size_t startOfLabel, size_t endOfLabel)
{
    char *post_label =copy_out_substring(endOfLabel,strlen(Label_instruction),Label_instruction);

    char *pre_label=copy_out_substring(0,startOfLabel,Label_instruction);

    constexpr uint8_t max_address_size = 6;
    
    size_t label_len =strlen(pre_label)+strlen(post_label)+max_address_size+1;
    
    char *LABEL = (char*)malloc(sizeof(char)*(label_len));
    
    snprintf(LABEL,label_len, "%s0x%x%s",pre_label,address,post_label);

    free(post_label);
    free(pre_label);
    return LABEL;
}


void ConvertLabelToAddress(std::string &Assembly_Instruction,uint32_t address)
{
    bool CALL_USES_EXTERNAL_MEM = false;    // memory in external chip
    if(strstr(Assembly_Instruction.c_str(),"//External Memory"))
    {
        CALL_USES_EXTERNAL_MEM = true;
    }
    
    //this is the label being called from the branch command
    
    //if((strstr(Assembly_Instruction.c_str()," LABEL")))
    {
        char *temp = translate_label_to_mem_address(Assembly_Instruction.c_str(),CALL_USES_EXTERNAL_MEM,address);

        Assembly_Instruction.resize(strlen(temp)+1);
        Assembly_Instruction = temp;
        free(temp);
    }
}

