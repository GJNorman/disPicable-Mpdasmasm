//
//  DisassemblerParameters.cpp
//  annoyed
//
//  Created by Greg Norman on 2/2/2023.
//

#include "DisassemblerParameters.hpp"


uint16_t check_n(int16_t n, uint16_t bitmask,uint16_t negative_sign_mask, uint32_t program_counter)
{
    if(n&negative_sign_mask) //negative sign
    {
        n=((~n)&bitmask)+1;
        n=program_counter-n*2;
    }
    else
    {
        n=program_counter+n*2;
    }
    return (n);//&0xffff);
}

void find_opcode_parameters(uint16_t MachineCode_byte,uint8_t &prm1, uint8_t &prm2, uint16_t &prm3, uint8_t prm1_operand,uint8_t prm2_operand,uint8_t prm3_operand, uint8_t prm_bitshift, char &Access_or_bank_RAM)
{
    uint8_t upper_byte = (MachineCode_byte&0xff00)>>8;

    prm2 = upper_byte &prm2_operand;                //access or bank

    if(prm2==0)
    {
        Access_or_bank_RAM = 'A';
    }
    else
    {
        Access_or_bank_RAM = 'B';
    }
    prm1 =(upper_byte &prm1_operand)>>prm_bitshift;

    prm3 = MachineCode_byte&prm3_operand;
}
