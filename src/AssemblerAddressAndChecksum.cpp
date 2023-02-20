//
//  AssemblerAddressAndChecksum.cpp
//  
//
//  Created by Greg Norman on 28/1/2023.
//

#include "AssemblerAddressAndChecksum.hpp"

void add_to_checksum(uint16_t OPCODE, uint16_t &checksum)
{
    checksum+=(OPCODE&0xff)&0xff;
    checksum+=((0xff00&OPCODE)>>8)&0xff;
}

bool increment_assembler_address(Address_And_Checksum_t &Address, uint32_t FLASH_size)
{
    if(Address.lower_32_bits <= FLASH_size)
    {
        Address.lower_32_bits+=2;
    }
    else    //eeprom addresses
    {
        Address.upper_16_bits+=0xF0;
        Address.lower_32_bits=0;

        uint16_t EEPROM_checksum = 0x2 +0x4;
        add_to_checksum(Address.upper_16_bits,EEPROM_checksum);
        output_Machine_Code("%.2X\r\n:02000004%.4X%.2X",(1+(~Address.check_sum)) & 0xff,Address.upper_16_bits,(1+(~EEPROM_checksum)) & 0xff);
        Address.check_sum_required=false;
        Address.check_sum=0;
        return true;
    }
    return false;
}


// header will specifier address and number of databytes
// we will always write a complete line, so num_bytes = 0x10
void formatHexHeader(char *header,Address_And_Checksum_t &Address)
{
    constexpr uint16_t headerMaxSize = 20;
    if(Address.lower_32_bits!=0)
    {
        if(Address.check_sum_required==true)    // checksum is from the previous line
            snprintf(header,headerMaxSize,"%.2X\r\n:10%.4X00",(1+(~Address.check_sum))&0xff,Address.lower_32_bits&0xffff);
        else
            snprintf(header,headerMaxSize,"\r\n:10%.4X00",Address.lower_32_bits&0xffff);
        
        Address.check_sum_required=true;
    }
    
    else    //no checksum
    {
        snprintf(header,headerMaxSize,"\r\n:10%.4X00",Address.lower_32_bits&0xffff);
    }
}

// finish the current line of the hex file with NOPs
void padFile(Address_And_Checksum_t &Address)
{
    bool lineAdded=false;
    if(Address.check_sum_required == true)
    {
        output_Machine_Code("%.2X",(1+(~Address.check_sum))&0xff);
        Address.check_sum=0;
        Address.check_sum_required = false;
    }
    while(Address.lower_32_bits%16 != 0)
    {
        output_Machine_Code("%s","FFFF");
        add_to_checksum(0xffff, Address.check_sum);
        
        Address.lower_32_bits+=2;
        lineAdded=true;
    }
    if(lineAdded==true)
    {
        output_Machine_Code("%.2X",(1+(~Address.check_sum))&0xff);
        Address.check_sum=0;
    }
}
