//
//  DisassemblerParameters.cpp
//  
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

void find_opcode_parameters(uint16_t MachineCode_byte,
                            uint8_t &b_or_d,                    
                            uint8_t &Access,
                            uint16_t &file,
                            uint8_t b_or_d_mask,
                            uint8_t access_mask,
                            uint8_t file_mask,
                            uint8_t b_or_d_bitshift,
                            char &Access_or_bank_RAM)           // outputs as either 'A' or 'B'
{
    uint8_t upper_byte = (MachineCode_byte&0xff00)>>8;

    Access = upper_byte &access_mask;                //access or bank

    if(Access==0)
    {
        Access_or_bank_RAM = 'A';
    }
    else
    {
        Access_or_bank_RAM = 'B';
    }
    b_or_d =(upper_byte &b_or_d_mask)>>b_or_d_bitshift;

    file = MachineCode_byte&file_mask;
}
