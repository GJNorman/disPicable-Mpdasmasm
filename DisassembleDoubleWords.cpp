//
//  DisassembleDoubleWords.cpp
//  
//
//  Created by Greg Norman on 3/2/2023.
//

#include "DisassembleDoubleWords.hpp"
bool disassemble_PIC18F_DoubleWord(uint16_t MachineCode_byte ,uint16_t MachineCode_byte2, std::string opcode_mnem, uint8_t CASE_TYPE, char*& command_for_prompt, uint64_t program_counter)
{
    //the second word of every double word command will begin with an "0xF000"
    //sometimes ascii data will alias a double word instruction
    if((MachineCode_byte2&0xf000) != 0xf000)
    {
        char temp[5+strlen("UNKNOWN[]")]="";
        snprintf(temp,sizeof(temp),"UNKNOWN[%.4X]",MachineCode_byte);
        strcat(command_for_prompt,temp);
        return false;
    }
    //source and destination
    uint16_t fs=0,fd=0;

    uint16_t fs_bitmask=0x0fff;
    uint16_t fd_bitmask=0x0fff;

    if((CASE_TYPE==PIC18F_DW_CASE2)||(CASE_TYPE==PIC18F_CALL_CASE))
    {
        fs_bitmask=0x00ff;//lower 8 bits
    }
    if(CASE_TYPE==PIC18F_LIT_CASE2)
    {
        fs_bitmask=0x000f;//lower 4 bits
        fd_bitmask=0x00ff;//lower 8 bits
    }
    fs = MachineCode_byte & fs_bitmask;///lower 12 bits

    fd = MachineCode_byte2 & fd_bitmask;
    char temp[100]="";

    if(CASE_TYPE==PIC18F_DW_CASE)
    {
        std::string fs_stand_in = processEQUforDisassembler(fs,fs_bitmask);
        std::string fd_stand_in = processEQUforDisassembler(fd,fd_bitmask);

        snprintf(temp,sizeof(temp),"%s %s, %s",opcode_mnem.c_str(),fs_stand_in.c_str(),fd_stand_in.c_str());
    }
    if(CASE_TYPE==PIC18F_DW_CASE2)//applies to goto command
    {
        uint32_t n = (fd<<8) + fs;
        snprintf(temp,sizeof(temp),"%s 0x%.4X",opcode_mnem.c_str(),n<<1);
    }
    if(CASE_TYPE==PIC18F_CALL_CASE)
    {
        uint32_t n = (fd<<8) + fs;
        uint8_t s = (MachineCode_byte & 0b0000000100000000)>>8;
        snprintf(temp,sizeof(temp),"%s 0x%.4X, %d",opcode_mnem.c_str(),n<<1,s);
    }
    if(CASE_TYPE==PIC18F_LIT_CASE2)
    {
        uint32_t n = fd + (fs<<8);
        uint8_t s = (MachineCode_byte & 0b110000)>>4;
        snprintf(temp,sizeof(temp),"%s %u, 0x%x", opcode_mnem.c_str(),s,n);
    }
    strcat(command_for_prompt,temp);

    return true;
}
