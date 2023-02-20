//
//  AssembleSingleWord.cpp
//  
//
//  Created by Greg Norman on 28/1/2023.
//

#include "AssembleSingleWord.hpp"


void Assemble_PIC18f_SingleWord(std::string &Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set,uint64_t pos,uint32_t address,uint16_t &checksum)
{
    switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos])
    {
        case PIC18F_FDA_CASE:
            assemble_single_word_case(Assembly_Instruction.c_str(), Instruction_Set,  pos,true, true,address&0xffff,checksum,0xff);
            break;
        case PIC18F_FA_CASE:
            assemble_single_word_case(Assembly_Instruction.c_str(), Instruction_Set,  pos,false, true,address&0xffff,checksum,0xff);
            break;
        case PIC18F_CTRL_CASE:
            assemble_single_word_case(Assembly_Instruction.c_str(), Instruction_Set,  pos,false, false,address&0xffff,checksum,0xff);
            break;
        case PIC18F_CTRL_CASE2:
            assemble_single_word_case(Assembly_Instruction.c_str(), Instruction_Set,  pos,false, false,address&0xffff,checksum,0xfff);
            break;
        case PIC18F_BIT_CASE:
            assemble_single_word_case(Assembly_Instruction.c_str(), Instruction_Set,  pos,true, true,address&0xffff,checksum,0xff);
            break;
        case PIC18F_LIT_CASE:
            assemble_single_word_case(Assembly_Instruction.c_str(), Instruction_Set,  pos,false, false,address&0xffff,checksum,0xff);
            break;
        case PIC18F_RET_CASE:
            assemble_single_word_case(Assembly_Instruction.c_str(), Instruction_Set,  pos,false, false,address&0xffff,checksum,0xff);
            break;
    }
}

void assemble_single_word_case(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, bool find_d_or_b_parameter, bool find_a_or_s_parameter, uint32_t address, uint16_t &checksum, uint16_t f_mask)
{

    //     MOVWF ANSEL, A
    bool optionalArgumentPresent = false; // assume it will not be there
    /*-------------------
     Find F (or N)
     -------------------*/
    uint16_t f=0;
    uint16_t d_or_b=0, a=0;
    f = find_f_value_for_assembly(Assembly_Instruction,Instruction_Set, pos,address&0xffff);

    /*-------------------
     (optional) Find D or B
     -------------------*/
    size_t a_offset=0;
    if(find_d_or_b_parameter == true)
    {
        d_or_b=find_d_value_for_assembly(Assembly_Instruction,Instruction_Set, pos,optionalArgumentPresent);
        
        if(optionalArgumentPresent == true)
        {
            a_offset = strlen(",");
        }
    }
    /*-------------------
     (optional) Find a
     -------------------*/
    if(find_a_or_s_parameter == true)
    {
        a = find_a_value_for_assembly(Assembly_Instruction,Instruction_Set, pos, a_offset) & 1;
    }

    
    uint16_t OPCODE = Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos] + (d_or_b<<9) + (a<<8) + (f & f_mask);

    output_Machine_Code("%.2X%.2X",(OPCODE&0xff),((0xff00&OPCODE)>>8));  //little endian

    add_to_checksum(OPCODE,checksum);
    
    if(OPCODE > Instruction_Set.PIC18F_OPCODES_UPPER_VAL[pos])
    {
        printf("ERROR: OPCODE OUT OF LIMITS: INSTR = {%s},OPCODE = {%x}, case = {%d}\n",Assembly_Instruction,OPCODE,Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos]);
    }
}
