//
//  DisassembleSingleword.cpp
//  
//
//  Created by Greg Norman on 2/2/2023.
//

#include "DisassembleSingleword.hpp"

void disassemble_PIC18f_SingleWord(uint16_t MachineCode_byte,
                                        PIC18F_FULL_IS &Instruction_Set,
                                        char*&command_for_prompt,
                                        size_t command_for_prompt_max_len,
                                        uint32_t program_counter)
{
    uint8_t CASE_TYPE = Instruction_Set.PIC18F_OPCODES_DECODE_CASE[Instruction_Set.currentCommand];
    std::string opcode_mnem = Instruction_Set.PIC18F_MNEMONICS[Instruction_Set.currentCommand];

    //final two bits of byte1 are d and a, byte2 is f
    uint8_t d=0,a=0;                // d is the desination bit, a specifies either access for general purpose ram banks
    uint16_t f=0;                   // f is a file address
    char Access_or_bank_RAM='A';    // will indicate the value of 'a'

    find_opcode_parameters(MachineCode_byte,d, a, f, 0b10,0b1,0xff, 1,Access_or_bank_RAM);

    char f_stand_in[50];
    
    snprintf(f_stand_in,sizeof(f_stand_in),"0x%x",f);
    
    if(a==0)    //check for Special Function Registers
    {
        if((CASE_TYPE==PIC18F_FDA_CASE)||(CASE_TYPE==PIC18F_FA_CASE)||(CASE_TYPE==PIC18F_BIT_CASE))
        {
            
            if(f>0x5f)  // address indicates special function registers
            {
                f+= getSFRAccessBankOffset();   // add the address offset required for SFR register mapped to the access bank
                f = f & 0xffff;
                std::string RegisterName = processEQUforDisassembler(f, 0xff);

                if(RegisterName.empty() == false)
                {
                    snprintf(f_stand_in,sizeof(f_stand_in),"%s",RegisterName.c_str());
                }
            }
            else
            {
                // this is for tracking which register's are in use - see "EQUs.cpp" -> variable "MostRecentEQU"
                setEQU(f);
            }
        }

    }
    Instruction_Set.currentBank = a;
    Instruction_Set.destinationBit = d;
    Instruction_Set.currentFile = f;

    // parameters for various pic instructions
    int32_t n =0;           // parameter in branching instructions
    uint8_t b=0, s=0;       // b is used in 'bit-based' instructions

    std::string BitName;    // the bit 'b' may have a name specified in the header file
    
    
    // add a formatted command to the end of the string
    switch(CASE_TYPE)
    {
        case PIC18F_FDA_CASE:
            snprintf(&command_for_prompt[strlen(command_for_prompt)],
                     command_for_prompt_max_len-strlen(command_for_prompt),
                     "%s %s, %u, %c", opcode_mnem.c_str(),f_stand_in,d,Access_or_bank_RAM);
            break;
            
        case PIC18F_FA_CASE:
            snprintf(&command_for_prompt[strlen(command_for_prompt)],
                     command_for_prompt_max_len-strlen(command_for_prompt)
                     ,"%s %s, %c", opcode_mnem.c_str(),f_stand_in,Access_or_bank_RAM);
            break;
            
        case PIC18F_CTRL_CASE:
            n = check_n((MachineCode_byte)&0xff,0xff,0x80, program_counter);
            snprintf(&command_for_prompt[strlen(command_for_prompt)],
                     command_for_prompt_max_len-strlen(command_for_prompt),
                     "%s 0x%.4X", opcode_mnem.c_str(),n);
            break;
            
        case PIC18F_CTRL_CASE2:
            n = check_n((MachineCode_byte)&0x7ff,0x7ff,0x400, program_counter);
            snprintf(&command_for_prompt[strlen(command_for_prompt)],
                     command_for_prompt_max_len-strlen(command_for_prompt),
                     "%s 0x%.4X", opcode_mnem.c_str(),n);
            break;
            
        case PIC18F_BIT_CASE:
            find_opcode_parameters(MachineCode_byte,b, a, f, 0b1110,0b1,0xff, 1,Access_or_bank_RAM);
            Instruction_Set.currentBit = b;
            
            // convert b to a string
            BitName = findEQUBitForDisassembler(b);
            snprintf(&command_for_prompt[strlen(command_for_prompt)],
                     command_for_prompt_max_len-strlen(command_for_prompt),
                     "%s %s, %s, %c", opcode_mnem.c_str(),f_stand_in,BitName.c_str(),Access_or_bank_RAM);
            break;
            
        case PIC18F_LIT_CASE:
            snprintf(&command_for_prompt[strlen(command_for_prompt)],
                     command_for_prompt_max_len-strlen(command_for_prompt),
                     "%s %s", opcode_mnem.c_str(),f_stand_in);
            
            // track current ram bank
            if(Instruction_Set.currentCommand == PIC18F_IS::PIC_COMMANDS::MOVLB)
                Instruction_Set.currentBSR = f & 0xff;
            
            break;
            
        case PIC18F_RET_CASE:
            s = MachineCode_byte & 0x1;
            snprintf(&command_for_prompt[strlen(command_for_prompt)],
                     command_for_prompt_max_len-strlen(command_for_prompt),
                     "%s %u", opcode_mnem.c_str(),s);
            break;
            
        default :
            snprintf(&command_for_prompt[strlen(command_for_prompt)],
                     command_for_prompt_max_len-strlen(command_for_prompt),
                     "UNKNOWN[%d]",MachineCode_byte);
    }
    
    // record any file addresses from GPR banks
    if(a==1)
    {
        if((CASE_TYPE==PIC18F_FDA_CASE)||(CASE_TYPE==PIC18F_FA_CASE)||(CASE_TYPE==PIC18F_BIT_CASE))
        {
            setEQU(Instruction_Set.currentFile + (Instruction_Set.currentBSR<<8));
        }
    }
}
