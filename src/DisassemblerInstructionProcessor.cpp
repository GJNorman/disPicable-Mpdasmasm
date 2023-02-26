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

void record_UserData(Converted_Assembly_Code &OutputAssemblyCode,       // output for Assembly code
                     PIC18F_FULL_IS &Instruction_Set,                   // Pic instruction set)
                     uint16_t n,
                     char *command_for_prompt,
                     size_t command_for_prompt_len_max
                     )
{
    Instruction_Set.currentCommand = ~0;
    
    uint8_t upperByte = 0xff&(n>>8);
    uint8_t lowerByte = n & 0xff;
    
    char UPPER[10];
    char LOWER[10];
    
    removeEscapeCharacter(upperByte,UPPER,sizeof(UPPER));
    removeEscapeCharacter(lowerByte,LOWER,sizeof(LOWER));
    
    snprintf(&command_for_prompt[0],command_for_prompt_len_max,"    DB \"%s\"\n    DB \"%s\"",LOWER,UPPER);
    OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(command_for_prompt);

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
    
    size_t command_for_prompt_len_max = 1024;
    
    char *command_for_prompt=NULL;
    
    command_for_prompt=(char*)calloc(sizeof(char),command_for_prompt_len_max);
    
    // sometimes double word instructions wrap around to the next line of the hex file
    // if this is the case, we will adjust our start position to the start of the next word
    // i.e. 4 bytes
    int start_point =0 ;
    if(final_byte_is_double_word==true)
    {
        start_point=4;                      // skip first word (we already did it)
        final_byte_is_double_word=false;
        address+=2;
    }

    for(uint8_t counter = start_point;    counter<MachineCode.size(); counter += 4)
    {
        Instruction_Set.currentCommand = 0;

        char MachineCodeAscii[5];
        snprintf(MachineCodeAscii,sizeof(MachineCodeAscii),"%c%c%c%c",MachineCode[counter+2],MachineCode[counter+3],MachineCode[counter+0],MachineCode[counter+1]);
        uint16_t n = strtol(MachineCodeAscii, NULL, 16);
        
        recordOpcode(OutputAssemblyCode,n);
        bool Instruction_found = false;

        snprintf(command_for_prompt,command_for_prompt_len_max,"%.4x%.4x    %s    ",Address_Upper_16_bits,0xffff & address,MachineCodeAscii);

        address+=2;
        
        bool skipFinalise = false;
        for(auto it =  Instruction_Set.PIC18F_OPCODES_UPPER_VAL.begin(); it != Instruction_Set.PIC18F_OPCODES_UPPER_VAL.end(); it++)
        {
            if((n>=Instruction_Set.PIC18F_OPCODES_LOWER_VAL[Instruction_Set.currentCommand]) &&
               (n<=Instruction_Set.PIC18F_OPCODES_UPPER_VAL[Instruction_Set.currentCommand]))
            {
                
                OutputAssemblyCode.Address.push_back(command_for_prompt);

                // for holding second word of double word instructions
                char MachineCodeAscii_SecondWord[5];
                uint16_t WORD_2 = 0;
                switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[Instruction_Set.currentCommand])
                {
                    //double word instructions
                    case PIC18F_DW_CASE:
                    case PIC18F_DW_CASE2:
                    case PIC18F_CALL_CASE:
                    case PIC18F_LIT_CASE2:
                            
                        if(counter+(unsigned)7<MachineCode.size())//sometimes double word instructions are spread accross two file records
                        {
                            snprintf(MachineCodeAscii_SecondWord,
                                     sizeof(MachineCodeAscii_SecondWord),
                                     "%c%c%c%c",
                                     MachineCode[counter+6],        // little endian
                                     MachineCode[counter+7],
                                     MachineCode[counter+4],
                                     MachineCode[counter+5]);
                        }
                        else //data is inside the next record
                        {
                            //find the next entry
                            while(HEX_FILE_CONTENTS[file_position]!=':')
                            {
                                file_position++;
                            }
                            //and copy over the bytes
                            
                            snprintf(MachineCodeAscii_SecondWord,
                                     sizeof(MachineCodeAscii_SecondWord),
                                     "%c%c%c%c",
                                     HEX_FILE_CONTENTS[file_position+11],
                                     HEX_FILE_CONTENTS[file_position+12],
                                     HEX_FILE_CONTENTS[file_position+9],
                                     HEX_FILE_CONTENTS[file_position+10]);
                            
                            final_byte_is_double_word=true;
                        }
                        // copy second word to opcode buffer
                        WORD_2 = strtol(MachineCodeAscii_SecondWord, NULL, 16);
                        recordOpcode(OutputAssemblyCode,WORD_2);
                        
                        if(disassemble_PIC18F_DoubleWord(n,WORD_2,Instruction_Set,command_for_prompt,command_for_prompt_len_max,address&0xffff)==true)
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
                        if(strcmp(MachineCodeAscii,"FFFF")==0)
                        {
                            snprintf(&command_for_prompt[strlen(command_for_prompt)], command_for_prompt_len_max-strlen(command_for_prompt),"NOP");
                        }
                        // Everything else becomes "unknown" so that no information is lost
                        // most likely this is user data
                        else
                        {
                            skipFinalise = true;
                            record_UserData(OutputAssemblyCode,Instruction_Set,n,command_for_prompt,command_for_prompt_len_max);
                        }
                        break;
                    case PIC18F_NO_ARGS:
                        snprintf(&command_for_prompt[strlen(command_for_prompt)],
                                 command_for_prompt_len_max-strlen(command_for_prompt),"%s",
                                 Instruction_Set.PIC18F_MNEMONICS[Instruction_Set.currentCommand].c_str());
                        break;

                    //single word statements
                    default:
                        disassemble_PIC18f_SingleWord(n,Instruction_Set,command_for_prompt,command_for_prompt_len_max,address&0xffff);
                        break;
                }
                if(skipFinalise == false)
                {
                    finalise_command(command_for_prompt,
                                     OutputAssemblyCode,
                                     Instruction_Set,
                                     Address_Upper_16_bits,
                                     Instruction_Set.FLASH_size,
                                     n);
                }
                Instruction_found = true;
                break;
            }

            Instruction_Set.currentCommand++;
            
        }
        // if we can't find the instruction, then we will assume this is user data
        // this will be marked with as
        // "    DB   LOWER_BYTE"
        // "    DB   UPPER_BYTE"
        if(Instruction_found==false)
        {
            OutputAssemblyCode.Address.push_back(command_for_prompt);
            record_UserData(OutputAssemblyCode,Instruction_Set,n,command_for_prompt,command_for_prompt_len_max);
        }
    }
    
    free(command_for_prompt);
    return;
}


// add final formatting before pushing to output
void finalise_command(char *&command_for_prompt,                    // buffer for current instruction
                      Converted_Assembly_Code &OutputAssemblyCode,  // holds entire output program
                      PIC18F_FULL_IS &Instruction_Set,              // Pic instruction set
                      uint16_t Address_Upper_16_bits,               // upper 16 bits of address
                      uint32_t device_mem_size,                     // total available flash memory
                      uint16_t n )                                  // opcode
{
    bool include_comments=false;
    size_t end_of_meta_data=16;
    RAMTracker::trackRAM(command_for_prompt,Instruction_Set);
    trackTableReads(command_for_prompt,OutputAssemblyCode,n,Instruction_Set);
    watchFunctionStacks(command_for_prompt,OutputAssemblyCode,Instruction_Set);
    
    /*
     optionally add comments explaining what each command does
     */

    if(include_comments==true)
    {
        OutputAssemblyCode.CommentAddress.push_back((uint32_t)OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size());
        OutputAssemblyCode.Comments.push_back(Instruction_Set.PIC18F_DESCRIPTIONS[Instruction_Set.currentCommand]);
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

