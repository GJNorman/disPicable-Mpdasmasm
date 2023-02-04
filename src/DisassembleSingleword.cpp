//
//  DisassembleSingleword.cpp
//  annoyed
//
//  Created by Greg Norman on 2/2/2023.
//

#include "DisassembleSingleword.hpp"

void disassemble_with_PIC18f_SingleWord(uint16_t MachineCode_byte, PIC18F_FULL_IS &Instruction_Set,uint8_t IS_Pos,char*&command_for_prompt, uint32_t program_counter)
{
    uint8_t CASE_TYPE = Instruction_Set.PIC18F_OPCODES_DECODE_CASE[IS_Pos];
    std::string opcode_mnem = Instruction_Set.PIC18F_MNEMONICS[IS_Pos];

    //final two bits of byte1 are d and a, byte2 is f
    uint8_t d=0,a=0;
    uint16_t f=0;
    char Access_or_bank_RAM='A';

    find_opcode_parameters(MachineCode_byte,d, a, f, 0b10,0b1,0xff, 1,Access_or_bank_RAM);

    char f_stand_in[20]="";
    char Comment[100]="";
    
    snprintf(f_stand_in,sizeof(f_stand_in),"0x%x",f);
    
    if(a==0)    //check for Special Function Registers
    {
        if((CASE_TYPE==PIC18F_FDA_CASE)||(CASE_TYPE==PIC18F_FA_CASE)||(CASE_TYPE==PIC18F_BIT_CASE))
        {
            if(f>0x5f)
            {
                f+=0xf00;
                f = f & 0xffff;
                std::string RegisterName = processEQUforDisassembler(f, 0xff);

                if(RegisterName.empty() == false)
                {
                    snprintf(f_stand_in,sizeof(f_stand_in),"%s",RegisterName.c_str());
                }
                
            }
        }
    }
    int32_t n =0;
    uint8_t b=0, s=0;
    char temp[100]="";
    switch(CASE_TYPE)
    {
        case PIC18F_FDA_CASE:
            snprintf(temp,sizeof(temp),"%s %s, %u, %u", opcode_mnem.c_str(),f_stand_in,d,a);//for whatever reason we have to use 0 or 1 for "a" on FDA commands, but "A" or "B" for FA commands
            break;
        case PIC18F_FA_CASE:
            snprintf(temp,sizeof(temp),"%s %s, %c", opcode_mnem.c_str(),f_stand_in,Access_or_bank_RAM);
            break;
        case PIC18F_CTRL_CASE:
            n = check_n((MachineCode_byte)&0xff,0xff,0x80, program_counter);
            snprintf(temp,sizeof(temp),"%s 0x%.4X", opcode_mnem.c_str(),n);
            break;
        case PIC18F_CTRL_CASE2:
            n = check_n((MachineCode_byte)&0x7ff,0x7ff,0x400, program_counter);
            snprintf(temp,sizeof(temp),"%s 0x%.4X", opcode_mnem.c_str(),n);
            break;
        case PIC18F_BIT_CASE:
            find_opcode_parameters(MachineCode_byte,b, a, f, 0b1110,0b1,0xff, 1,Access_or_bank_RAM);
            snprintf(temp,sizeof(temp),"%s %s, %u, %c", opcode_mnem.c_str(),f_stand_in,b,Access_or_bank_RAM);
            break;
        case PIC18F_LIT_CASE:
            snprintf(temp,sizeof(temp),"%s %s", opcode_mnem.c_str(),f_stand_in);
            break;
        case PIC18F_RET_CASE:
            s = MachineCode_byte & 0x1;
            snprintf(temp,sizeof(temp),"%s %u", opcode_mnem.c_str(),s);
            break;
        default :
            snprintf(temp,sizeof(temp),"UNKNOWN[%d]",MachineCode_byte);
    }

    strcat(command_for_prompt,temp);
    strcat(command_for_prompt,Comment);
}
