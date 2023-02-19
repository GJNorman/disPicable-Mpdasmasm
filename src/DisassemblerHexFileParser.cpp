//
//  DisassemblerHexFileParser.cpp
//  
//
//  Created by Greg Norman on 19/2/2023.
//

#include "DisassemblerHexFileParser.hpp"

static uint32_t Convert_num_to_hex( std::vector<unsigned char> input,uint64_t &vec_start, uint8_t return_len)
{
    uint32_t return_value=0;
    for(int counter=0;counter<return_len;counter++)
    {
        char tempstring[10];
        snprintf(tempstring,sizeof(tempstring),"%c",input[vec_start+counter]);

        return_value=return_value*0x10;
        return_value+=strtol(tempstring,NULL,16);
    }
    vec_start+=return_len;

    return return_value;

}

bool dissassembleDataEntry(uint64_t &file_pos,
                           Converted_Assembly_Code &OutputAssemblyCode,
                           std::vector<unsigned char> &OutputFileContents,
                           PIC18F_FULL_IS &Instruction_Set,
                           bool bDisplayAssembly,
                           bool bDisplayBinContents)
{
    
    static uint16_t Address_Upper_16_bits = 0x00;

    // intel hex file header format
    // :DBADDRTDATADATADATADATACS
    uint8_t number_of_bytes=       0xff &   Convert_num_to_hex(OutputFileContents,file_pos,2);
    uint32_t address =                      Convert_num_to_hex(OutputFileContents,file_pos,4);
    uint8_t type =                 0xff &   Convert_num_to_hex(OutputFileContents,file_pos,2);

    if(type==0x00)
    {
        static bool first_byte = true;
        if(first_byte==true)
        {
            first_byte=false;
            add_ORG_to_asm_file(address,OutputAssemblyCode);
        }

        number_of_bytes*=2;

        std::vector<uint8_t> Data_bytes(number_of_bytes);
        for(int counter=0;counter<number_of_bytes;counter++)
        {
            Data_bytes[counter] = OutputFileContents[file_pos];
           
            file_pos++;
        }

        if(Address_Upper_16_bits==Instruction_Set.Device_ID_Address)
        {
            deal_with_non_program_data(Data_bytes,"DEVICE_ID:",address,Address_Upper_16_bits,OutputAssemblyCode,false);
        }
        else if(Address_Upper_16_bits==Instruction_Set.Config_Address)
        {
            deal_with_non_program_data(Data_bytes,"CONFIG_BITS:",address,Address_Upper_16_bits,OutputAssemblyCode,false);
        }
        else if(Address_Upper_16_bits==Instruction_Set.EEPROM_START_ADDR)
        {
            deal_with_non_program_data(Data_bytes,"EEPROM_DIRECTIVE:",address,Address_Upper_16_bits,OutputAssemblyCode,true);
        }
        else
        {
            generate_asm(Data_bytes,address,OutputFileContents,file_pos, OutputAssemblyCode,Instruction_Set,bDisplayAssembly,bDisplayBinContents,Address_Upper_16_bits);
        }
    }
    if(type==0x01)  //End of File
    {
        return true;
    }
    if(type==0x04)  //Extended Linear Address
    {
        Address_Upper_16_bits = Convert_num_to_hex(OutputFileContents,file_pos,4);
    }
    
    return false;
}


void parseHexFile(std::vector<unsigned char> hexFileContents,           // input file
                  Converted_Assembly_Code &OutputAssemblyCode,          // output file
                  PIC18F_FULL_IS &Instruction_Set,                      // pic instruction set
                  bool bDisplayAssembly,                                // debug - display assembly code on console
                  bool bDisplayBinContents)                             // debug - display opcodes on console
{
    //;
    // read through hex file, convert opcodes to assembly
    for(uint64_t file_pos=0; file_pos < hexFileContents.size() ; )
    {
        if(hexFileContents[file_pos]==':')  // first character on a new line
        {
            file_pos++;                     // move past colon
            
            if(dissassembleDataEntry(file_pos, OutputAssemblyCode,hexFileContents,Instruction_Set, bDisplayAssembly,bDisplayBinContents) == true)
            {
                break;
            }
        }
        else
        {
            file_pos++;
        }
    }
}
