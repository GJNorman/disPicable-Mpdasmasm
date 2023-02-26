//
//  DisassembleDirectives.cpp
//  
//
//  Created by Greg Norman on 2/2/2023.
//

#include "DisassembleDirectives.hpp"

// non program data meaning eeprom, config etc etc
void deal_with_non_program_data(std::vector<uint8_t> Data_bytes,                // input data
                                const char *Tag,                                // name for datafield 'EEPROM', 'DEVICE' etc.
                                uint32_t address,                               // program address
                                uint16_t Address_Upper_16_bits,
                                Converted_Assembly_Code &OutputAssemblyCode,    // output file
                                bool include_address_line )                     // for first entry
{
    size_t temp_length = Data_bytes.size()+1+strlen(Tag)+strlen(":10123400"); //":101234.. == space for address line

    // add 'tag' and address
    char temp[temp_length+1];

    snprintf(temp,temp_length,"%s",Tag);
    
    if(include_address_line == true)
    {
        snprintf(temp,temp_length,"%s10%.4X00",Tag,address);
    }
    
    //concatenate data bytes
    for(int counter=0;counter<Data_bytes.size();counter++)
    {
        snprintf(&temp[strlen(temp)],temp_length, "%c",Data_bytes[counter]);
    }
    OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);

    char print_addr[10];
    snprintf(print_addr,sizeof(print_addr),"%.4X%.4X",Address_Upper_16_bits,(address&0xffff));
    OutputAssemblyCode.Address.push_back(print_addr);
}

// the first line of the file is an ORG directive
void add_ORG_to_asm_file(uint32_t address,Converted_Assembly_Code &OutputAssemblyCode)
{
    char temp[strlen("ORG ")+1+6]="";
    snprintf(temp,sizeof(temp),"ORG 0x%.4X",address);
    OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);

    snprintf(temp,sizeof(temp),"0x0000%.4X",address);
    OutputAssemblyCode.Address.push_back(temp);
}
