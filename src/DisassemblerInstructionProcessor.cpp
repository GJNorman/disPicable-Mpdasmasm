//
//  DisassemblerInstructionProcessor.cpp
//  
//
//  Created by Greg Norman on 10/2/2023.
//

#include "DisassemblerInstructionProcessor.hpp"



void recordOpcode(Converted_Assembly_Code &OutputAssemblyCode,uint16_t OPCODE)
{
    OutputAssemblyCode.OPCODES.push_back(OPCODE&0xff);      // keep track of opcodes
    OutputAssemblyCode.OPCODES.push_back((OPCODE>>8)&0xff); // keep track of opcodes
}



void generate_asm(std::vector<uint8_t> &MachineCode,                // data from Hex file
                  uint32_t &address,                                // current program address
                  std::vector<unsigned char> &HEX_FILE_CONTENTS,    // entire contents of hex file, in case we need to check the next word
                  uint64_t file_position,                           // current hex file offset
                  Converted_Assembly_Code &OutputAssemblyCode,      // output for Assembly code
                  PIC18F_FULL_IS &Instruction_Set,                  // Pic instruction set
                  bool bdisplayASM,                                 // debugging - display assembly output
                  bool bDisplayBinContents,                         // debugging - display opcode input
                  uint16_t Address_Upper_16_bits)                   // upper 16 bits of current address
{
    static bool final_byte_is_double_word = false;
    char temp[5]="";
    bool Instruction_found = false;
    size_t command_for_prompt_len_max = 1024;
    char *command_for_prompt=NULL;
    command_for_prompt=(char*)calloc(sizeof(char),command_for_prompt_len_max);

    
    // sometimes double word instructions wrap around to the next line of the hex file
    // if this is the case, we will adjust our start position to the start of the next word
    // i.e. 4 bytes
    int start_point =0 ;
    if(final_byte_is_double_word==true)
    {
        start_point=4;
        final_byte_is_double_word=false;
        address+=2;
    }
    for(uint8_t counter=start_point;counter<MachineCode.size();counter+=4)
    {
        snprintf(temp,sizeof(temp),"%c%c%c%c",MachineCode[counter+2],MachineCode[counter+3],MachineCode[counter+0],MachineCode[counter+1]);
        uint16_t n = (int)strtol(temp, NULL, 16);
        
        recordOpcode(OutputAssemblyCode,n);
        Instruction_found = false;

        snprintf(command_for_prompt,command_for_prompt_len_max,"%.4x%.4x    %s    ",Address_Upper_16_bits,0xffff & address,temp);

        address+=2;
        uint16_t array_pos=0;
        uint16_t WORD_2 = 0;
        for(auto it =  Instruction_Set.PIC18F_OPCODES_UPPER_VAL.begin(); it != Instruction_Set.PIC18F_OPCODES_UPPER_VAL.end(); it++)
        {
            if((n>=Instruction_Set.PIC18F_OPCODES_LOWER_VAL[array_pos]) && (n<=Instruction_Set.PIC18F_OPCODES_UPPER_VAL[array_pos]))
            {
                
                OutputAssemblyCode.Address.push_back(command_for_prompt);

                char temp2[5]="";
                switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[array_pos])
                {
                    //double word instructions
                    case PIC18F_DW_CASE:
                    case PIC18F_DW_CASE2:
                    case PIC18F_CALL_CASE:
                    case PIC18F_LIT_CASE2:
                            
                        if(counter+(unsigned)7<MachineCode.size())//sometimes double word instructions are spread accross two file records
                        {
                            snprintf(temp2,sizeof(temp2),"%c%c%c%c",MachineCode[counter+6],MachineCode[counter+7],MachineCode[counter+4],MachineCode[counter+5]);
                        }
                        else //data is inside the next record
                        {
                            //find the next entry
                            while(HEX_FILE_CONTENTS[file_position]!=':')
                            {
                                file_position++;
                            }
                            //and copy over the bytes
                            snprintf(temp2,sizeof(temp2),"%c%c%c%c",HEX_FILE_CONTENTS[file_position+11],HEX_FILE_CONTENTS[file_position+12],HEX_FILE_CONTENTS[file_position+9],HEX_FILE_CONTENTS[file_position+10]);
                            final_byte_is_double_word=true;
                        }
                        // copy second word to opcode buffer
                        WORD_2 = strtol(temp2, NULL, 16);
                        recordOpcode(OutputAssemblyCode,WORD_2);
                        
                        if(disassemble_PIC18F_DoubleWord(n,WORD_2, Instruction_Set.PIC18F_MNEMONICS[array_pos], Instruction_Set.PIC18F_OPCODES_DECODE_CASE[array_pos], command_for_prompt,address&0xffff)==true)
                        {
                            counter+=4;//double word
                            address+=2;
                        }
                        else    //sometimes ascii data aliases double word instructions, if this occurs the above statement will return false
                        {
                            final_byte_is_double_word=false;
                        }

                        break;

                    //no operation instructions
                    case PIC18F_NOP_CASE:

                        //ascii data will alias as NOP instructions, so only "FFFF" is treated as NOP
                        if(strcmp(temp,"FFFF")==0)
                        {
                            snprintf(&command_for_prompt[strlen(command_for_prompt)], command_for_prompt_len_max-strlen(command_for_prompt),"NOP");
                        }
                        //Everything else becomes "unknown" so that no information is lost
                        else
                        {
                            snprintf(&command_for_prompt[strlen(command_for_prompt)], command_for_prompt_len_max-strlen(command_for_prompt), "UNKNOWN[%s]",temp);
                        }
                        break;
                    case PIC18F_NO_ARGS:
                        strcat(command_for_prompt,Instruction_Set.PIC18F_MNEMONICS[array_pos].c_str());
                        break;

                    //single word statements
                    default:
                        disassemble_with_PIC18f_SingleWord(n,Instruction_Set,array_pos, command_for_prompt,address&0xffff);
                        break;
                }

                finalise_command(command_for_prompt,OutputAssemblyCode,Instruction_Set.PIC18F_DESCRIPTIONS[array_pos].c_str(),Address_Upper_16_bits,Instruction_Set.FLASH_size,n);
                Instruction_found = true;
                break;
            }

            array_pos++;
        }
        if(Instruction_found==false)
        {
            OutputAssemblyCode.Address.push_back(command_for_prompt);
            uint8_t upperByte = 0xff&(n>>8);
            uint8_t lowerByte = n & 0xff;
            char UPPER[10];
            char LOWER[10];
            removeEscapeCharacter(upperByte,UPPER,sizeof(UPPER));
            removeEscapeCharacter(lowerByte,LOWER,sizeof(LOWER));
            snprintf(command_for_prompt,command_for_prompt_len_max,"    DB \"%s\"\n    DB \"%s\"",LOWER,UPPER);
            OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(command_for_prompt);

        }
    }
    free(command_for_prompt);
    return;
}


// add final formatting before pushing to output
void finalise_command(char *&command_for_prompt,                    // buffer for current instruction
                      Converted_Assembly_Code &OutputAssemblyCode,  // holds entire output program
                      const char *Description,                      // Comment that describes what the instruction does
                      uint16_t Address_Upper_16_bits,               // upper 16 bits of address
                      uint32_t device_mem_size,                     // total available flash memory
                      uint16_t n )                                  // opcode
{
    bool include_comments=false;
    size_t end_of_meta_data=16;
    
    trackTableReads(command_for_prompt,OutputAssemblyCode,n);
    watchFunctionStacks(command_for_prompt,OutputAssemblyCode);
    
    /*
     optionally add comments explaining what each command does
     */

    if(include_comments==true)
    {
        OutputAssemblyCode.CommentAddress.push_back((uint32_t)OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size());
        OutputAssemblyCode.Comments.push_back(Description);
    }
    uint8_t index =0 ;
    const std::vector<std::string> instructionList = {"GOTO","BC","BN","BNN","BNC","BNOV","BNZ","BOV","BRA","BZ","CALL","RCALL"};
    
    size_t endOfCommand = check_against_list_of_instructions(instructionList, &command_for_prompt[20],  index);
    
    if(endOfCommand!=0) // zero indicates command not present
    {
        const char *Branching_Command = instructionList[index].c_str();
        
        addLabeltoBranchingInstruction(OutputAssemblyCode,command_for_prompt, Branching_Command,  device_mem_size,end_of_meta_data);
    }
    else
    {
        char *temp =copy_out_substring(end_of_meta_data,strlen(command_for_prompt),command_for_prompt);

        OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);

        free(temp);
    }
    clearMostRecentEQU();
}

