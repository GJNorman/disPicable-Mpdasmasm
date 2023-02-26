//
//  DisassembleDoubleWords.cpp
//  
//
//  Created by Greg Norman on 3/2/2023.
//

#include "DisassembleDoubleWords.hpp"
bool disassemble_PIC18F_DoubleWord(uint16_t MachineCode_byte ,
                                   uint16_t MachineCode_byte2,
                                   PIC18F_FULL_IS &Instruction_Set,
                                   char*& command_for_prompt,
                                   size_t commandForPromptMaxSize,
                                   uint64_t program_counter)
{
    std::string opcode_mnem = Instruction_Set.PIC18F_MNEMONICS[Instruction_Set.currentCommand];
    uint8_t CASE_TYPE = Instruction_Set.PIC18F_OPCODES_DECODE_CASE[Instruction_Set.currentCommand];
    //the second word of every double word command will begin with an "0xF000"
    //sometimes user data will alias a double word instruction
    if((MachineCode_byte2&0xf000) != 0xf000)
    {
        snprintf(&command_for_prompt[strlen(command_for_prompt)],
                 commandForPromptMaxSize-strlen(command_for_prompt),"UNKNOWN[%.4X]",MachineCode_byte);
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

    if(CASE_TYPE==PIC18F_DW_CASE)
    {
        Instruction_Set.MOVFFs = fs;
        Instruction_Set.MOVFFd = fd;
        std::string fs_stand_in = processEQUforDisassembler(fs,fs_bitmask);
        std::string fd_stand_in = processEQUforDisassembler(fd,fd_bitmask);

        snprintf(&command_for_prompt[strlen(command_for_prompt)],
                 commandForPromptMaxSize-strlen(command_for_prompt),"%s %s, %s",opcode_mnem.c_str(),fs_stand_in.c_str(),fd_stand_in.c_str());
    }
    if(CASE_TYPE==PIC18F_DW_CASE2)//applies to goto command
    {
        uint32_t n = (fd<<8) + fs;
        
        snprintf(&command_for_prompt[strlen(command_for_prompt)],
                 commandForPromptMaxSize-strlen(command_for_prompt),"%s 0x%.4X",opcode_mnem.c_str(),n<<1);
    }
    if(CASE_TYPE==PIC18F_CALL_CASE)
    {
        uint32_t n = (fd<<8) + fs;
        uint8_t s = (MachineCode_byte & 0b0000000100000000)>>8;
        
        snprintf(&command_for_prompt[strlen(command_for_prompt)],
                 commandForPromptMaxSize-strlen(command_for_prompt),"%s 0x%.4X, %d",opcode_mnem.c_str(),n<<1,s);
    }
    if(CASE_TYPE==PIC18F_LIT_CASE2)
    {
        uint32_t n = fd + (fs<<8);
        uint8_t s = (MachineCode_byte & 0b110000)>>4;
        
        snprintf(&command_for_prompt[strlen(command_for_prompt)],
                 commandForPromptMaxSize-strlen(command_for_prompt),"%s %u, 0x%x", opcode_mnem.c_str(),s,n);
    }

    return true;
}
