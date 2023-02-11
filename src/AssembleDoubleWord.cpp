//
//  AssembleDoubleWord.cpp
//  
//
//  Created by Greg Norman on 28/1/2023.
//

#include "AssembleDoubleWord.hpp"

void find_FILE_values_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address, uint16_t &fs, uint16_t &fd);

// this is for optimising branching instructions
// currently it performs a check, but does not do anything
static void check_if_single_word_branch_will_work(uint32_t k, uint32_t address,const char *Assembly_Instruction,uint32_t &OPCODE_UPPER, uint32_t &OPCODE_LOWER)
{
    int32_t check_if_rcall_can_be_used = (int32_t ) (k - address);
    if(abs(check_if_rcall_can_be_used ) < 512)
    {
        //printf("Shorter Branch can be used: %s; Range = %d\n",Assembly_Instruction, check_if_rcall_can_be_used);
    }
}

char * assemble_double_word_case(std::string &Assembly_Instruction, PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address, uint16_t &checksum)
{
    //MOVFF Fs, Fd  PIC18F_DW_CASE
    //      1100 ffff ffff ffff
    //      1111 ffff ffff ffff

    //CALL n,s  PIC18F_CALL_CASE
    //      1110 000s kkkk kkkk
    //      1111 kkkk kkkk kkkk

    //GOTO n PIC18F_DW_CASE2
    //      1110 1111 kkkk kkkk
    //      1111 kkkk kkkk kkkk

    //LFSR f,k PIC18F_LIT_CASE2
    //      1110 1110 00ff kkkk
    //      1111 0000 kkkk kkkk
    char *ReturnWord=NULL;
    uint32_t k=0;
    uint32_t OPCODE_UPPER = Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos];
    uint32_t OPCODE_LOWER = 0b1111000000000000;                                 //same value used for all double word instructions
    if(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_DW_CASE)
    {
        uint16_t fs=0,fd=0;
        find_FILE_values_for_assembly(Assembly_Instruction.c_str(),Instruction_Set, pos,address&0xffff,fs,fd);
        OPCODE_UPPER+=fs;
        OPCODE_LOWER+=fd;
    }
    if(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_CALL_CASE)
    {
        uint8_t s=0;
        k = find_f_value_for_assembly(Assembly_Instruction.c_str(),Instruction_Set, pos,address&0xffff);
        s = find_a_value_for_assembly(Assembly_Instruction.c_str(),Instruction_Set, pos, strlen(","));
        if(s==0)
            check_if_single_word_branch_will_work(k,address,Assembly_Instruction.c_str(),OPCODE_UPPER,OPCODE_LOWER);
        k>>=1;
        OPCODE_UPPER+= (k&0xff) + (s<<8);
        OPCODE_LOWER+= (k>>8)&0xfff;
        
    }
    if(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_DW_CASE2 )
    {
        k = find_f_value_for_assembly(Assembly_Instruction.c_str(),Instruction_Set, pos,address&0xffff);
        check_if_single_word_branch_will_work(k,address,Assembly_Instruction.c_str(),OPCODE_UPPER,OPCODE_LOWER);
        k>>=1;
        OPCODE_UPPER+= (k&0xff);
        OPCODE_LOWER+= (k>>8)&0xfff;
    }
    if(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_LIT_CASE2)
    {
        uint8_t f=0;
        f = find_LFSR_f_value(Assembly_Instruction.c_str());
        k = find_f_value_for_assembly(Assembly_Instruction.c_str(),Instruction_Set, pos,address&0xffff);
 
        OPCODE_UPPER+= ((k>>8)&0x0f) + (f<<4);
        OPCODE_LOWER+= (k)&0xff;
    }
    uint32_t temp = address&0xffff;

    while(temp>=16)
    {
        temp-=16;
    }
    add_to_checksum(OPCODE_UPPER,checksum);
    if(temp+4<=16)
    {
        output_Machine_Code("%.2X%.2X%.2X%.2X",(OPCODE_UPPER&0xff),((0xff00&OPCODE_UPPER)>>8),((OPCODE_LOWER&0xff)),((0xff00&OPCODE_LOWER)>>8));

        add_to_checksum(OPCODE_LOWER,checksum);
    }
    else    //Second word out of bounds, return it to main and add to next entry
    {
        output_Machine_Code("%.2X%.2X",((OPCODE_UPPER&0xff)),((0xff00&OPCODE_UPPER)>>8));
        ReturnWord = (char*)malloc(sizeof(char)*5);
        snprintf(ReturnWord,5,"%.2X%.2X",(OPCODE_LOWER&0xff),(0xff00&OPCODE_LOWER)>>8);
    }

    if(OPCODE_UPPER > Instruction_Set.PIC18F_OPCODES_UPPER_VAL[pos])
    {
        printf("ERROR: UPPER OPCODE OUT OF LIMITS: %s, %x%x\n",Assembly_Instruction.c_str(),OPCODE_UPPER,OPCODE_LOWER);
    }

    return ReturnWord;
}



// used for MOVFF fs,fd
// FILE means Fs and Fd
// both arguments are mandatory
void find_FILE_values_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address, uint16_t &fs, uint16_t &fd)
{

    char* FD = return_substring(Assembly_Instruction, ",",NULL, 0, strlen(","), 0);
    
    size_t spaces = CheckForLeadingSpaces(FD, 0);

    size_t temp_len =strlen(Assembly_Instruction);
    
    char temp[temp_len+1];
    snprintf(temp,temp_len,"%s",Assembly_Instruction);
    strtok(temp,",");                                   //remove FD

    // MOVFF is guaranteed to be in the string
    size_t p1 = (strstr(temp,"MOVFF")-temp)+strlen("MOVFF");

    p1 = CheckForLeadingSpaces(temp, p1);
    char FS[strlen(temp)-p1+1] ;
    snprintf(FS,(strlen(temp)-p1+1),"%s",&temp[p1]);
    
    fs = check_if_FSR_or_RAW_REG(FS,Instruction_Set);
    fd = check_if_FSR_or_RAW_REG(&FD[spaces],Instruction_Set);
    
    free(FD);

    return ;
}
