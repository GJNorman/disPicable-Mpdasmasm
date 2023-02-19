//
//  DisassemblerLabels.cpp
//  
//
//  Created by Greg Norman on 2/2/2023.
//

#include "DisassemblerLabels.hpp"

// add all of the accumulated labels into the finished assembly program
void addLabelsToAssemblyCode(Converted_Assembly_Code &OutputAssemblyCode)
{
    modifyBasedOnAddress(OutputAssemblyCode,OutputAssemblyCode.LABEL_POSITIONS,OutputAssemblyCode.LABEL_STRINGS,true, true);
}


// add symbolic label to branching instruction
void addLabeltoBranchingInstruction(Converted_Assembly_Code &OutputAssemblyCode,        // full program being created
                                    const char *command_for_prompt,                     // currently constructed line
                                    const char *Branching_Command,                      // the instruction detected in the line
                                    uint32_t device_mem_size,                           // MCU's flash memory size
                                    size_t end_of_meta_data)                            // end of meta-data in "command_for_prompt"
                                                                                        // the meta-data is the memory address and opcode and has a fixed position
{
    //find the position to move to
    size_t p1 = (strstr(command_for_prompt,Branching_Command)-command_for_prompt)+strlen(Branching_Command);
    size_t p2 = strlen(command_for_prompt);
    size_t x = (strstr(command_for_prompt,"x")-command_for_prompt)+1;//mem address

    const char *nextComma = strstr(&command_for_prompt[p1],",");
    
    if(nextComma!=NULL)
    {
        p2 = nextComma-command_for_prompt;
    }

    char *pre_label = copy_out_substring(end_of_meta_data,p1,command_for_prompt);

    char *label_address = copy_out_substring(x-strlen("0x"),p2,command_for_prompt);

    char *comment = copy_out_substring(p2,strlen(command_for_prompt),command_for_prompt);

    char *Addr = copy_out_substring(x,p2,command_for_prompt);

    OutputAssemblyCode.LABEL_POSITIONS.push_back(strtol(Addr, NULL, 16)/2);

    size_t label_len = strlen(label_address)+strlen(" LABEL_:")+strlen(command_for_prompt)+100;
    char label[label_len+1];
    
    snprintf(label,label_len,"%s LABEL_%s %s",pre_label,label_address,comment);

    //the label may be refering to external memory
    if((unsigned)strtol(label_address,NULL,16)>device_mem_size)
    {
        snprintf(label,label_len,"%sLABEL_%s %s//External Memory",pre_label,label_address,comment);
        std::cout <<"Address (" << label_address  << ") Exceeds Internal Storage (" << device_mem_size << ")\n";
    }

    
    OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(label);

    strtok(label_address," ");
    snprintf(label,label_len,"LABEL_0x%.4X:",(unsigned)strtol(label_address,NULL,16));
    OutputAssemblyCode.LABEL_STRINGS.push_back(label);

    //free(Branching_Command);
    free(label_address);
    free(comment);
    free(Addr);
    free(pre_label);
}
