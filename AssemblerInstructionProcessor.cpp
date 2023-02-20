//
//  AssemblerInstructionProcessor.cpp
//  
//
//  Created by Greg Norman on 10/2/2023.
//

#include "AssemblerInstructionProcessor.hpp"

void outputHeader(Address_And_Checksum_t &Address,
                  PIC18F_FULL_IS &Instruction_Set,
                  char *&ReturnWord)
{
    char header[45]="";
    
    formatHexHeader(header,Address);

    if(ReturnWord!=NULL)
    {
        output_Machine_Code("%s%s",header,ReturnWord);
        Address.check_sum=0x10;
        add_to_checksum(Address.lower_32_bits&0xffff, Address.check_sum);
        
        increment_assembler_address(Address,Instruction_Set.FLASH_size);
        
        uint16_t OPCODE = strtol(ReturnWord,NULL,16);
        
        add_to_checksum(OPCODE, Address.check_sum);
        
        free(ReturnWord);
        ReturnWord=NULL;
    }
    else
    {
        if((Address.lower_32_bits %16 == 0) || (Address.lower_32_bits==0))
        {
            output_Machine_Code("%s",header);
            Address.check_sum=0x10;
            add_to_checksum(Address.lower_32_bits, Address.check_sum);
        }
    }
}
bool processInstruction(std::string &Assembly_Instruction,      // current line being processed
                        PIC18F_FULL_IS &Instruction_Set,        // PIC18 instruction set
                        Address_And_Checksum_t &Address)
{
    static char *ReturnWord=NULL;                  // when the first byte of data comes from the previous line

    bool instruction_found = false;
    // isolate the instruction from any attached parameters
    size_t point1 = 0;
    size_t point2 = FindNextSpaceCharacter(Assembly_Instruction,point1);
    
    std::string command = Assembly_Instruction.substr(point1,point2-point1);
    
    for(size_t pos = 0; pos<Instruction_Set.PIC18F_MNEMONICS.size(); pos++)
    {
        if((Instruction_Set.PIC18F_MNEMONICS[pos].size() == command.size())&&
           (strcasecmp(Instruction_Set.PIC18F_MNEMONICS[pos].c_str(),command.c_str())==0))
        {
    
            instruction_found=true;
            outputHeader(Address,Instruction_Set,ReturnWord);
            
            //convert labels for branching instructions
            switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos])
            {
                case PIC18F_DW_CASE2:
                case PIC18F_CALL_CASE:
                case PIC18F_CTRL_CASE:
                case PIC18F_CTRL_CASE2:
                    ConvertLabelToAddress(Assembly_Instruction,Address.START_ADDRESS);
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
                    ReturnWord = assemble_double_word_case(Assembly_Instruction, Instruction_Set,  pos, Address.lower_32_bits&0xffff,Address.check_sum);
                    
                    if(ReturnWord==NULL)
                    {
                        increment_assembler_address(Address,Instruction_Set.FLASH_size);
                    }
                    break;
                    
                    //no argument instructions
                case PIC18F_NOP_CASE:
                    output_Machine_Code("%s","FFFF");
                    add_to_checksum(0xffff, Address.check_sum);
                    break;
                case PIC18F_NO_ARGS:
                    output_Machine_Code("%.2X%.2X",Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos]&0xff,(Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos]>>8)&0xff);
                    add_to_checksum(Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos], Address.check_sum);
                    break;
                    
                    //single word statements
                default:
                    Assemble_PIC18f_SingleWord(Assembly_Instruction,Instruction_Set,pos,Address.lower_32_bits&0xffff,Address.check_sum);
                    break;
            }
            increment_assembler_address(Address,Instruction_Set.FLASH_size);
            
            break;
        }
    }
    return instruction_found;
}

