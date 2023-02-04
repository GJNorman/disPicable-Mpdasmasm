//
//  AssemblerAddressAndChecksum.cpp
//  annoyed
//
//  Created by Greg Norman on 28/1/2023.
//

#include "AssemblerAddressAndChecksum.hpp"

void add_to_checksum(uint16_t OPCODE, uint16_t &checksum)
{
    checksum+=(OPCODE&0xff)&0xff;
    checksum+=((0xff00&OPCODE)>>8)&0xff;
}

bool increment_assembler_address(uint16_t &upper_16_bits, uint32_t &lower_32_bits,uint16_t &check_sum, bool &check_sum_required, uint32_t FLASH_size)
{
    if(lower_32_bits <= FLASH_size)
    {
        lower_32_bits+=2;
    }
    else    //eeprom addresses
    {
        upper_16_bits+=0xF0;
        lower_32_bits=0;

        uint16_t EEPROM_checksum = 0x2 +0x4;
        add_to_checksum(upper_16_bits,EEPROM_checksum);
        output_Machine_Code("%.2X\r\n:02000004%.4X%.2X",(1+(~check_sum)) & 0xff,upper_16_bits,(1+(~EEPROM_checksum)) & 0xff);
        check_sum_required=false;
        check_sum=0;
        return true;
    }
    return false;
}


// header will specifier address and number of databytes
// we will always write a complete line, so num_bytes = 0x10
void formatHexHeader(char *header, bool &check_sum_required,uint16_t &check_sum, uint32_t &address)
{
    constexpr uint16_t headerMaxSize = 20;
    if(address!=0)
    {
        if(check_sum_required==true)    // checksum is from the previous line
            snprintf(header,headerMaxSize,"%.2X\r\n:10%.4X00",(1+(~check_sum))&0xff,address&0xffff);
        else
            snprintf(header,headerMaxSize,"\r\n:10%.4X00",address&0xffff);
        
        check_sum_required=true;
    }
    
    else    //no checksum
    {
        snprintf(header,headerMaxSize,"\r\n:10%.4X00",address&0xffff);
    }
}
