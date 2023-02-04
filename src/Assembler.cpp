//
//  Assembler.cpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#include "Assembler.hpp"

extern const char *Global_working_directory;

TimerClass MyTimer(0);                                          // for benchmarking
Converted_Assembly_Code AssemblyCode;                           // stores Assembly program





void Assemble(const char *inputfiledir, bool bDisplayBinContents,bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set)
{
    MyTimer.updateTimerReference();
    clearIncludedFileDirectories();
    clearEQU();
    AssemblyCode = Copy_Over_Binary_File(inputfiledir,Instruction_Set);

    if(AssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size() == 0)
    {
        std::cout << "Error: File Not Found: " << inputfiledir << "\n";
        return;
    }
    if(!check_file_fits_memory(AssemblyCode,Instruction_Set))
    {
        std::cout << "ERROR - NOT ENOUGH FLASH MEMORY" << std::endl;
        return; 
    }
    
    std::vector<include_directory_t> includedFiles = checkForIncludedFiles();
    
    std::string FuckingAnnoying = "/";
    for(auto it: includedFiles)
    {
        if(it.type == INCLUDE_RELATIVE){
            std::string FileDir = Global_working_directory + FuckingAnnoying + it.name;
            AssemblyCode.append(Copy_Over_Binary_File(FileDir.c_str(),Instruction_Set));
        }
    }
    //printEQUs();

    uint32_t START_ADDRESS=0;
    uint32_t address=0x00;                  // current program counter
    uint16_t address_upper_16bits=0;
    uint16_t check_sum=0x00;
    bool check_sum_required=true;
    
    
    output_Machine_Code("%s",":020000040000FA");

    size_t counter =0;
    for (auto Assembly_Instruction: AssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM)
    {

        if(true){
          //  std::cout<< AssemblyCode.Address[counter] << "\t\t"<<Assembly_Instruction<< "\n";
            counter++;
        }
        bool instruction_found =  processInstruction(Assembly_Instruction,
                                                     Instruction_Set,
                                                     address,
                                                     check_sum_required,
                                                     check_sum,
                                                     address_upper_16bits,
                                                     START_ADDRESS);
        if(instruction_found==false)
        {
            processDirective(Assembly_Instruction,
                             address,
                             check_sum_required,
                             check_sum,
                             Instruction_Set,
                             address_upper_16bits,
                             START_ADDRESS);
        }
        
        
    }
    //End of FILE
    output_Machine_Code("%s","\r\n:00000001FF\r\n");
    
    char output_dir[FILENAME_MAX]="";
    snprintf(output_dir,sizeof(output_dir),"%s/code.hex",Global_working_directory);
    printHexCode(bDisplayAssembly,output_dir);
    
    std::cout << "Total Time to Assemble: " <<MyTimer.CheckDuration() << " seconds\n";
}
void assemble_UnknownOrDB(uint16_t value,uint16_t address_upper_16bits, uint32_t &address, uint16_t &check_sum,bool &check_sum_required,PIC18F_FULL_IS &Instruction_Set )
{
    if(address % 16 !=0)
    {
        output_Machine_Code("%.2X%.2X",value&0xff,(value>>8)&0xff);
    }
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
    std::cout << Assembly_Instruction << "\n";
    // [optional label]  DB  'x','y'
    //                   DB  "asdasd"
    
    // look for quotes
    
    size_t nextComma = 0;
    do{
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


bool processInstruction(std::string &Assembly_Instruction,      // current line being processed
                        PIC18F_FULL_IS &Instruction_Set,        // PIC18 instruction set
                        uint32_t &address,                      // program counter
                        bool &check_sum_required,               // check sum needed at end of each line of hex file
                        uint16_t &check_sum,                    // current check sum
                        uint16_t &address_upper_16bits,         // for address header
                        uint32_t START_ADDRESS)
{
    static char *ReturnWord=NULL;                  // when the first byte of data comes from the previous line

    bool instruction_found = false;
    // isolate the instruction from any attached parameters
    size_t point1 = 0;
    size_t point2 = FindNextSpaceCharacter(Assembly_Instruction,point1);
    
    std::string command = Assembly_Instruction.substr(point1,point2-point1);
    
    for(size_t pos = 0; pos<Instruction_Set.PIC18F_MNEMONICS.size(); pos++)
    {
        if(strcasecmp(Instruction_Set.PIC18F_MNEMONICS[pos].c_str(),command.c_str())==0)
        {
            
            
            instruction_found=true;
            char header[45]="";
            
            formatHexHeader(header,check_sum_required,check_sum,address);

            if(ReturnWord!=NULL)
            {
                output_Machine_Code("%s%s",header,ReturnWord);
                check_sum=0x10;
                add_to_checksum(address&0xffff, check_sum);
                
                increment_assembler_address(address_upper_16bits,address,check_sum,check_sum_required,Instruction_Set.FLASH_size);
                
                uint16_t OPCODE = strtol(ReturnWord,NULL,16);
                
                add_to_checksum(OPCODE, check_sum);
                
                free(ReturnWord);
                ReturnWord=NULL;
            }
            else
            {
                if((address %16 == 0) || (address==0))
                {
                    output_Machine_Code("%s",header);
                    check_sum=0x10;
                    add_to_checksum(address, check_sum);
                }
            }
            
            //convert labels for branching instructions
            switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos])
            {
                case PIC18F_DW_CASE2:
                case PIC18F_CALL_CASE:
                case PIC18F_CTRL_CASE:
                case PIC18F_CTRL_CASE2:
                    ConvertLabelToAddress(Assembly_Instruction,START_ADDRESS);
                default:
                    break;
            }
            switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos])
            {
                    //double word instructions
                case PIC18F_DW_CASE:
                case PIC18F_DW_CASE2:
                case PIC18F_CALL_CASE:
                case PIC18F_LIT_CASE2:
                    ReturnWord = assemble_double_word_case(Assembly_Instruction, Instruction_Set,  pos, address&0xffff,check_sum);
                    
                    if(ReturnWord==NULL)
                    {
                        increment_assembler_address(address_upper_16bits,address,check_sum,check_sum_required,Instruction_Set.FLASH_size);//address+=2;
                    }
                    break;
                    
                    //no argument instructions
                case PIC18F_NOP_CASE:
                    output_Machine_Code("%s","FFFF");
                    add_to_checksum(0xffff, check_sum);
                    break;
                case PIC18F_NO_ARGS:
                    output_Machine_Code("%.2X%.2X",Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos]&0xff,(Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos]>>8)&0xff);
                    add_to_checksum(Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos], check_sum);
                    break;
                    
                    //single word statements
                default:
                    Assemble_PIC18f_SingleWord(Assembly_Instruction,Instruction_Set,pos,address&0xffff,check_sum);
                    break;
            }
            increment_assembler_address(address_upper_16bits,address,check_sum,check_sum_required,Instruction_Set.FLASH_size);//address+=2;
            
            break;
        }
    }
    return instruction_found;
}



