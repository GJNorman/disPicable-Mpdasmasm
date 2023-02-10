//
//  AssembleDirectives.cpp
//  
//
//  Created by Greg Norman on 28/1/2023.
//

#include "AssembleDirectives.hpp"


// EEPROM, config bits etc.
void assemble_non_program_data(uint16_t REG_VALUE,
                               const char *Assembly_Instruction,
                               uint32_t &address,
                               uint16_t &address_upper_16bits,
                               uint16_t &checksum,
                               bool &check_sum_required,
                               uint32_t FLASH_size,
                               bool IsEEPROM);

// process eeprom directive
void assemble_EEPROM_data(uint16_t EEPROM_START_ADD_U16,
                          const char *Assembly_Instruction,
                          uint32_t &address,
                          uint16_t &address_upper_16bits,
                          uint16_t &checksum,
                          bool &check_sum_required,
                          uint32_t FLASH_size);


// process ORG directive
void processORG(uint32_t &address,
                std::string Assembly_Instruction,
                uint32_t &START_ADDRESS,
                uint16_t &check_sum);

void  assemble_DB(std::string Assembly_Instruction,
                  uint16_t address_upper_16bits,
                  uint32_t &address,
                  uint16_t &check_sum,
                  bool &check_sum_required,
                  PIC18F_FULL_IS &Instruction_Set);

void assemble_UnknownOrDB(uint16_t value,
                          uint16_t address_upper_16bits,
                          uint32_t &address,
                          uint16_t &check_sum,
                          bool &check_sum_required,
                          PIC18F_FULL_IS &Instruction_Set );

// for lines that include a ':' character, we read until
static char *GrabLabelledData(const char* Assembly_Instruction)
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
void processDirective(std::string Assembly_Instruction,
                      uint32_t &address,
                      bool &check_sum_required,
                      uint16_t &check_sum,
                      PIC18F_FULL_IS &Instruction_Set,
                      uint16_t &address_upper_16bits,
                      uint32_t &START_ADDRESS)
{
    
    // usually due to ascii data, or maybe code to be trasnferred to some other kind of chip
    if(strstr(Assembly_Instruction.c_str(),"UNKNOWN"))
    {
        char *unknown_command = return_substring(Assembly_Instruction.c_str(),"UNKNOWN[","]",0,strlen("UNKNOWN["),0);

        uint16_t value=strtol(unknown_command,NULL,16);
        
        assemble_UnknownOrDB(value, address_upper_16bits, address, check_sum,check_sum_required,Instruction_Set );

        free(unknown_command);
    }
    else if(strstr(Assembly_Instruction.c_str(),"ORG"))
    {
        processORG(address, Assembly_Instruction, START_ADDRESS,check_sum);
    }
    else if(strstr(Assembly_Instruction.c_str(),"DEVICE_ID"))
    {
        assemble_non_program_data(Instruction_Set.Device_ID_Address, Assembly_Instruction.c_str(),address,address_upper_16bits, check_sum,check_sum_required,Instruction_Set.FLASH_size,false);
    }
    else if(strstr(Assembly_Instruction.c_str(),"CONFIG"))
    {
        assemble_non_program_data(Instruction_Set.Config_Address, Assembly_Instruction.c_str(),address,address_upper_16bits, check_sum,check_sum_required,Instruction_Set.FLASH_size,false);
    }
    else if((strstr(Assembly_Instruction.c_str(),"db"))||(strstr(Assembly_Instruction.c_str(),"DB")))
    {
        assemble_DB(Assembly_Instruction, address_upper_16bits, address, check_sum, check_sum_required, Instruction_Set);
    }
    else if(strstr(Assembly_Instruction.c_str(),"EEPROM_DIRECTIVE"))
    {
        assemble_EEPROM_data(Instruction_Set.EEPROM_START_ADDR, Assembly_Instruction.c_str(), address,address_upper_16bits, check_sum, check_sum_required, Instruction_Set.FLASH_size);
    }
}

void assemble_non_program_data(uint16_t REG_VALUE, const char *Assembly_Instruction,  uint32_t &address,uint16_t &address_upper_16bits, uint16_t &checksum,bool &check_sum_required, uint32_t FLASH_size,bool IsEEPROM)
{
    if(check_sum_required==true)
    {
        // to get here we must have finished the main program data, so finish the current line before continueing
        padFile(address, checksum);
    }

    uint16_t check_sum = 0x2 +0x4 + REG_VALUE;

    if(check_sum_required==true)
    {
        output_Machine_Code("%.2X\r\n:0200000400%.2X%.2X",(1+(~checksum)) & 0xff,REG_VALUE,(1+(~check_sum)) & 0xff);
        checksum=0;
    }

    else
        output_Machine_Code("\r\n:0200000400%.2X%.2X",REG_VALUE,(1+(~check_sum)) & 0xff);

    char *temp = GrabLabelledData(Assembly_Instruction);
    
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
        
        padFile(address, checksum);
    }

    output_Machine_Code("%s","\r\n:");
    
    char *temp = GrabLabelledData(Assembly_Instruction);
    
    uint16_t check_sum =0;
    for(size_t counter=0; counter<strlen(temp);counter+=2)
    {
        char temp2[10];
        snprintf(temp2,sizeof(temp2),"%c%c",temp[counter],temp[counter+1]);
        check_sum+=strtol(temp2,NULL,16);
                
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
    // we will fill the space between the current address and the new ORG address with NOPs
    while(temp_address !=address)
    {
        padFile(temp_address, check_sum);
        
        // start of the next line
        output_Machine_Code(":10%.4X00",temp_address&0xffff);
        check_sum = 10;
        add_to_checksum(temp_address, check_sum);
    }
}

void assemble_UnknownOrDB(uint16_t value,uint16_t address_upper_16bits, uint32_t &address, uint16_t &check_sum,bool &check_sum_required,PIC18F_FULL_IS &Instruction_Set )
{
    if(address % 16 !=0)
    {
        output_Machine_Code("%.2X%.2X",value&0xff,(value>>8)&0xff);
    }
    
    // we are at the end of the current line
    else
    {
        if(check_sum_required==true)
        {
            output_Machine_Code("%.2X\r\n:10%.4X00",(1+(~check_sum))&0xff,address&0xffff);
        }
        else
        {
            output_Machine_Code("\r\n:10%.4X00",address&0xffff);
        }

        check_sum_required=true;
        check_sum=0x10;
        add_to_checksum(address, check_sum);
        output_Machine_Code("%.2X%.2X",value&0xff,(value>>8)&0xff);

    }
    add_to_checksum(value, check_sum);
    increment_assembler_address(address_upper_16bits,address,check_sum,check_sum_required,Instruction_Set.FLASH_size);

}
void  assemble_DB(std::string Assembly_Instruction,uint16_t address_upper_16bits, uint32_t &address, uint16_t &check_sum,bool &check_sum_required,PIC18F_FULL_IS &Instruction_Set)
{
    // [optional label]  DB  'x','y'
    //                   DB  "asdasd"
    
    // look for quotes
    
    size_t nextComma = 0;
    do{
        
        // data could be bounded by either ' or "
        size_t startBracket = Assembly_Instruction.find("'",nextComma);  // bounded by '
        size_t endBracket = 0;
        if(startBracket == std::string::npos)
        {
            startBracket = Assembly_Instruction.find("\"",nextComma); // bounded by "
            endBracket =Assembly_Instruction.find("\"",startBracket+1);
        }
        else
        {
            endBracket =Assembly_Instruction.find("'",startBracket+1);
        }
        if(startBracket == std::string::npos)
        {
            std::cout << "invalid DB: '" << Assembly_Instruction << "'\n";
            
            return;
        }
        startBracket++; //move past quote character
        
        std::string value = Assembly_Instruction.substr(startBracket,endBracket-startBracket);
        
        for(uint8_t index=0; index<value.size() ;index++)
        {
            assemble_UnknownOrDB(value[index], address_upper_16bits, address, check_sum, check_sum_required, Instruction_Set );
        }
        nextComma = Assembly_Instruction.find(",",nextComma+1);
        
    }while( nextComma != std::string::npos );
    
   
    //output_Machine_Code("%.2X",);
}
