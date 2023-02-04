//
//  AssembleDirectives.cpp
//  annoyed
//
//  Created by Greg Norman on 28/1/2023.
//

#include "AssembleDirectives.hpp"



// for lines that include a ':' character, we read until
static char *GrabPainInAssData(const char* Assembly_Instruction)
{
    size_t p1 =(strstr(Assembly_Instruction,":")-Assembly_Instruction)+1;
    size_t p2 = strlen(Assembly_Instruction);

    const char *CheckForSpace = strstr(Assembly_Instruction," ");
    if(CheckForSpace!=NULL)
    {
        p2 = CheckForSpace-Assembly_Instruction;
        if(p2 < p1) // space is in an anooying place
        {
            //remove leading white spaces
            size_t p3 = CheckForLeadingSpaces(Assembly_Instruction,0);
            
            // read until the end of the word
            p2 = FindNextSpaceCharacter(Assembly_Instruction, p3+1);
        }
    }

    char *temp =copy_out_substring(p1,p2,Assembly_Instruction);
    return temp;
}

void assemble_non_program_data(uint16_t REG_VALUE, const char *Assembly_Instruction,  uint32_t &address,uint16_t &address_upper_16bits, uint16_t &checksum,bool &check_sum_required, uint32_t FLASH_size,bool IsEEPROM)
{
    if(check_sum_required==true)
    while(address % 16 !=0)
    {
        increment_assembler_address(address_upper_16bits,address,checksum,check_sum_required, FLASH_size);//address+=2;
        output_Machine_Code("%s","FFFF");
        checksum+=0xff+0xff;
    }

    uint16_t check_sum = 0x2 +0x4 + REG_VALUE;

    if(check_sum_required==true)
    {
        output_Machine_Code("%.2X\r\n:0200000400%.2X%.2X",(1+(~checksum)) & 0xff,REG_VALUE,(1+(~check_sum)) & 0xff);
        checksum=0;
    }

    else
        output_Machine_Code("\r\n:0200000400%.2X%.2X",REG_VALUE,(1+(~check_sum)) & 0xff);

    char *temp = GrabPainInAssData(Assembly_Instruction);
    
    check_sum=0;
    for(size_t counter=0; counter<strlen(temp);counter+=2)
    {
        char temp2[5]="";
        snprintf(temp2,sizeof(temp2),"%c%c",temp[counter],temp[counter+1]);

        check_sum+=strtol(temp2,NULL,16);
    }
    check_sum+=(unsigned int)strlen(temp)/2;
    output_Machine_Code("\r\n:%.2X000000%s%.2X",(unsigned int)strlen(temp)/2,temp,(1+(~check_sum)) & 0xff);
    check_sum_required=false;

    free(temp);

    return;
}


void assemble_EEPROM_data(uint16_t EEPROM_START_ADD_U16, const char *Assembly_Instruction, uint32_t &address,uint16_t &address_upper_16bits, uint16_t &checksum,bool &check_sum_required, uint32_t FLASH_size)
{
    static uint8_t first_eeprom_bank = 0;
    //fill the remaing flash memory with NOPs
    if(first_eeprom_bank==0)
    {
        first_eeprom_bank=1;
        
        uint32_t remainder =(address % 0x10);
        
        FLASH_size = address + remainder - 1;
        
        while(!increment_assembler_address(address_upper_16bits,address,checksum,check_sum_required, FLASH_size))
        {
            output_Machine_Code("%s","0000");
            
            if((address % 16 ==0)&&(address<FLASH_size))
            {
                output_Machine_Code("%.2X\r\n:10%.4X00",(1+(~checksum)) & 0xff,address);
                check_sum_required=false;
                checksum=0x10;
                add_to_checksum(address, checksum);
            }
        }
    }


    char *temp = GrabPainInAssData(Assembly_Instruction);
    
    uint16_t check_sum =0;
    for(size_t counter=0; counter<strlen(temp);counter+=2)
    {
        char temp2[10]="";
        snprintf(temp2,sizeof(temp2),"%c%c",temp[counter],temp[counter+1]);
        check_sum+=strtol(temp2,NULL,16);
        
        if(counter==0)
            snprintf(temp2,sizeof(temp2),"\r\n:%c%c",temp[counter],temp[counter+1]);
        output_Machine_Code("%s",temp2);
        
    }
    output_Machine_Code("%.2X",(1+(~check_sum)) & 0xff);
    check_sum_required=false;

    free(temp);
}


// set current address according to ORG directive
void processORG(uint32_t &address, std::string Assembly_Instruction, uint32_t &START_ADDRESS,uint16_t &check_sum)
{
    uint32_t temp_address = address;

    char *ORG_address = return_substring(Assembly_Instruction.c_str(), "ORG", NULL, 0,strlen("ORG"), 0);

    if(ORG_address!=NULL)
    {
        address = strtol(ORG_address,NULL,16)&0xffff;     ///copy ORG address into working address
        START_ADDRESS = address;
        free(ORG_address);
    }
    else
    {
        std::cout << "Unable to Parse ORG \n";
    }
    if(temp_address > address)
    {
        std::cout <<"Error on ORG - "  << Assembly_Instruction <<", whose address (" << temp_address << ") " << "is less than current Address: " <<address << std::endl;
    }
    while(temp_address !=address)
    {
        if(temp_address % 16 ==0)
        {
            output_Machine_Code("%.2X\r\n",(1+(~check_sum)) & 0xff);
            output_Machine_Code(":10%.4X00",temp_address&0xffff);
            check_sum=0x10;
            add_to_checksum(temp_address, check_sum); //add NOP instructions
        }
        output_Machine_Code("0000");
        temp_address+=2;
    }
}
