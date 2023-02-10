//
//  AssemblerInstructionProcessor.cpp
//  
//
//  Created by Greg Norman on 10/2/2023.
//

#include "AssemblerInstructionProcessor.hpp"

bool processInstruction(std::string &Assembly_Instruction,      // current line being processed
                        PIC18F_FULL_IS &Instruction_Set,        // PIC18 instruction set
                        uint32_t &address,                      // program counter
                        bool &check_sum_required,               // check sum needed at end of each line of hex file
                        uint16_t &check_sum,                    // current check sum
                        uint16_t &address_upper_16bits,         // for address header
                        uint32_t START_ADDRESS)
{
    static char *ReturnWord=NULL;                  // when the first byte of data comes from the previous line

    bool instruction_found = false;
    // isolate the instruction from any attached parameters
    size_t point1 = 0;
    size_t point2 = FindNextSpaceCharacter(Assembly_Instruction,point1);
    
    std::string command = Assembly_Instruction.substr(point1,point2-point1);
    
    for(size_t pos = 0; pos<Instruction_Set.PIC18F_MNEMONICS.size(); pos++)
    {
        if(strcasecmp(Instruction_Set.PIC18F_MNEMONICS[pos].c_str(),command.c_str())==0)
        {
    
            instruction_found=true;
            char header[45]="";
            
            formatHexHeader(header,check_sum_required,check_sum,address);

            if(ReturnWord!=NULL)
            {
                output_Machine_Code("%s%s",header,ReturnWord);
                check_sum=0x10;
                add_to_checksum(address&0xffff, check_sum);
                
                increment_assembler_address(address_upper_16bits,address,check_sum,check_sum_required,Instruction_Set.FLASH_size);
                
                uint16_t OPCODE = strtol(ReturnWord,NULL,16);
                
                add_to_checksum(OPCODE, check_sum);
                
                free(ReturnWord);
                ReturnWord=NULL;
            }
            else
            {
                if((address %16 == 0) || (address==0))
                {
                    output_Machine_Code("%s",header);
                    check_sum=0x10;
                    add_to_checksum(address, check_sum);
                }
            }
            
            //convert labels for branching instructions
            switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos])
            {
                case PIC18F_DW_CASE2:
                case PIC18F_CALL_CASE:
                case PIC18F_CTRL_CASE:
                case PIC18F_CTRL_CASE2:
                    ConvertLabelToAddress(Assembly_Instruction,START_ADDRESS);
                default:
                    break;
            }
            switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos])
            {
                    //double word instructions
                case PIC18F_DW_CASE:
                case PIC18F_DW_CASE2:
                case PIC18F_CALL_CASE:
                case PIC18F_LIT_CASE2:
                    ReturnWord = assemble_double_word_case(Assembly_Instruction, Instruction_Set,  pos, address&0xffff,check_sum);
                    
                    if(ReturnWord==NULL)
                    {
                        increment_assembler_address(address_upper_16bits,address,check_sum,check_sum_required,Instruction_Set.FLASH_size);//address+=2;
                    }
                    break;
                    
                    //no argument instructions
                case PIC18F_NOP_CASE:
                    output_Machine_Code("%s","FFFF");
                    add_to_checksum(0xffff, check_sum);
                    break;
                case PIC18F_NO_ARGS:
                    output_Machine_Code("%.2X%.2X",Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos]&0xff,(Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos]>>8)&0xff);
                    add_to_checksum(Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos], check_sum);
                    break;
                    
                    //single word statements
                default:
                    Assemble_PIC18f_SingleWord(Assembly_Instruction,Instruction_Set,pos,address&0xffff,check_sum);
                    break;
            }
            increment_assembler_address(address_upper_16bits,address,check_sum,check_sum_required,Instruction_Set.FLASH_size);//address+=2;
            
            break;
        }
    }
    return instruction_found;
}

