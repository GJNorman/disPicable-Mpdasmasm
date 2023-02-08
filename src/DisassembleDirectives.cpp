//
//  DisassembleDirectives.cpp
//  
//
//  Created by Greg Norman on 2/2/2023.
//

#include "DisassembleDirectives.hpp"

void deal_with_non_program_data(std::vector<uint8_t> Data_bytes,const char *Tag, uint32_t address, uint16_t Address_Upper_16_bits,Converted_Assembly_Code &OutputAssemblyCode, bool include_address_line )
{
    size_t temp_length =Data_bytes.size()+1+strlen(Tag)+strlen(":10123400");
    char temp[temp_length+1];   //":101234.. == space for address line


    snprintf(temp,temp_length,"%s",Tag);
    if(include_address_line == true)
    {
        snprintf(temp,temp_length,"%s10%.4X00",Tag,address);
        
    }
    uint8_t counter_offset = strlen(temp);
    for(int counter=0;counter<Data_bytes.size();counter++)
    {
        char super_temp[3];
        snprintf(super_temp,sizeof(super_temp),"%c",Data_bytes[counter]);
        strcat(temp,super_temp);
        temp[counter+1+counter_offset]=0;
    }
    OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);

    char print_addr[10]="";
    snprintf(print_addr,sizeof(print_addr),"%.4X%.4X",Address_Upper_16_bits,(address&0xffff));
    OutputAssemblyCode.Address.push_back(print_addr);
}
void add_ORG_to_asm_file(uint32_t address,Converted_Assembly_Code &OutputAssemblyCode)
{
    
    char temp[strlen("ORG ")+1+6]="";
    snprintf(temp,sizeof(temp),"ORG 0x%.4X",address);
    OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);

    snprintf(temp,sizeof(temp),"0x0000%.4X",address);
    OutputAssemblyCode.Address.push_back(temp);
}
