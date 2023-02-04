//
//  Disassembler.cpp
//  annoyed
//
//  Created by Greg Norman on 27/1/2023.
//

#include "Disassembler.hpp"

const char Assembly_Comment_Marker = ';';
extern const char *Global_working_directory;
TimerClass MyTimerDissassembler;


void generate_asm(std::vector<uint8_t> &MachineCode,
                  uint32_t &address,
                  std::vector<unsigned char> &HEX_FILE_CONTENTS,
                  uint64_t file_position,
                  bool &final_byte_is_double_word,
                  Converted_Assembly_Code &OutputAssemblyCode,
                  PIC18F_FULL_IS &Instruction_Set,
                  bool bdisplayASM,bool bDisplayBinContents,
                  uint16_t Address_Upper_16_bits);
uint32_t Convert_num_to_hex( std::vector<unsigned char> input,uint64_t &vec_start, uint8_t return_len);

void finalise_command(char *&command_for_prompt,Converted_Assembly_Code &OutputAssemblyCode, const char *Description, uint16_t Address_Upper_16_bits,uint32_t device_mem_size,uint16_t n );
bool dissassembleDataEntry(uint64_t &file_pos,
                           Converted_Assembly_Code &OutputAssemblyCode,
                           std::vector<unsigned char> &OutputFileContents,
                           PIC18F_FULL_IS &Instruction_Set,
                           bool bDisplayAssembly,
                           bool bDisplayBinContents);

static void ReadMCUHeader(PIC18F_FULL_IS &Instruction_Set)
{
    clearIncludedFileDirectories();
    
    char fileDir[FILENAME_MAX] = "";
    
    snprintf(fileDir,FILENAME_MAX,"%s/%s.inc",Global_working_directory,Instruction_Set.MCU.c_str());
    
    Copy_Over_Binary_File(fileDir,Instruction_Set);
    
}

void WriteAssemblyCodeToFile(Converted_Assembly_Code &OutputAssemblyCode, bool bDisplayBinContents, bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set)
{
    char output_dir[320];
    snprintf(output_dir,sizeof(output_dir),"%s/code2.asm",Global_working_directory);
    FILE *fp = fopen(output_dir,"w");
    
    fprintf(fp,"#include \"%s.inc\"\n",Instruction_Set.MCU.c_str());
    
    
    for (uint64_t  pos = 0;pos<OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size();pos++)
    {
        
        if(bDisplayBinContents)
        {
            std::cout<<OutputAssemblyCode.Address[pos] << " ";
        }
        if(bDisplayAssembly)
        {
            std::cout<< OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos]<<std::endl;
        }
        fprintf(fp,"%s\n",OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos].c_str());
        
        if(strstr(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos].c_str(),"RET"))//RETFIE, RETURN
        {
            fprintf(fp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n;\n;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n");
        }
        
    }
    fclose(fp);

}

void Disassemble(std::vector<unsigned char> &OutputFileContents, bool bDisplayBinContents,bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set)
{
    MyTimerDissassembler.updateTimerReference();
    Converted_Assembly_Code OutputAssemblyCode;
    clearEQU();
    ReadMCUHeader(Instruction_Set);
    
    for(uint64_t file_pos=0; file_pos < OutputFileContents.size() ; )
    {
        if(OutputFileContents[file_pos]==':')  // first character on a new line
        {
            file_pos++; // move past colon
            
            if(dissassembleDataEntry(file_pos, OutputAssemblyCode,OutputFileContents,Instruction_Set, bDisplayAssembly,bDisplayBinContents) == true)
            {
                break;
            }
        }
        else
        {
            file_pos++;
        }
    }
    std::cout << MyTimerDissassembler.CheckDuration() << "  seconds\n";
    MyTimerDissassembler.updateTimerReference();
    
    markTableReads(OutputAssemblyCode,Instruction_Set);

    AddAllComments(OutputAssemblyCode);
    
    // add labels
    addLabelsToAssemblyCode(OutputAssemblyCode);

    WriteAssemblyCodeToFile(OutputAssemblyCode,bDisplayBinContents, bDisplayAssembly,Instruction_Set);
    
    std::cout << MyTimerDissassembler.CheckDuration() << "  seconds\n";
}


uint32_t Convert_num_to_hex( std::vector<unsigned char> input,uint64_t &vec_start, uint8_t return_len)
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
    static bool final_byte_is_double_word = false;
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

        //line_counter+=number_of_bytes/2;
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
            generate_asm(Data_bytes,address,OutputFileContents,file_pos, final_byte_is_double_word,OutputAssemblyCode,Instruction_Set,bDisplayAssembly,bDisplayBinContents,Address_Upper_16_bits);
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



void recordOpcode(Converted_Assembly_Code &OutputAssemblyCode,uint16_t OPCODE)
{
    OutputAssemblyCode.OPCODES.push_back(OPCODE&0xff); // keep track of opcodes
    OutputAssemblyCode.OPCODES.push_back((OPCODE>>8)&0xff); // keep track of opcodes
}
void generate_asm(std::vector<uint8_t> &MachineCode,
                  uint32_t &address,
                  std::vector<unsigned char> &HEX_FILE_CONTENTS,
                  uint64_t file_position,
                  bool &final_byte_is_double_word,
                  Converted_Assembly_Code &OutputAssemblyCode,
                  PIC18F_FULL_IS &Instruction_Set,
                  bool bdisplayASM,bool bDisplayBinContents,
                  uint16_t Address_Upper_16_bits)
{

    char temp[5]="";
    bool Instruction_found = false;
    size_t command_for_prompt_len_max = 1024;
    char *command_for_prompt=NULL;
    command_for_prompt=(char*)calloc(sizeof(char),command_for_prompt_len_max);

    int start_point =0 ;
    if(final_byte_is_double_word==true)
    {
        start_point=4;
        final_byte_is_double_word=false;
        address+=2;
    }
    for(uint8_t counter=start_point;counter<MachineCode.size();counter+=4)
    {
        snprintf(temp,sizeof(temp),"%c%c%c%c",MachineCode[counter+2],MachineCode[counter+3],MachineCode[counter+0],MachineCode[counter+1]);
        uint16_t n = (int)strtol(temp, NULL, 16);
        
        recordOpcode(OutputAssemblyCode,n);
        Instruction_found = false;

        snprintf(command_for_prompt,command_for_prompt_len_max,"%.4x%.4x    %s    ",Address_Upper_16_bits,0xffff & address,temp);

        address+=2;
        uint16_t array_pos=0;
        uint16_t WORD_2 = 0;
        for(auto it =  Instruction_Set.PIC18F_OPCODES_UPPER_VAL.begin(); it != Instruction_Set.PIC18F_OPCODES_UPPER_VAL.end(); it++)
        {
            if((n>=Instruction_Set.PIC18F_OPCODES_LOWER_VAL[array_pos]) && (n<=Instruction_Set.PIC18F_OPCODES_UPPER_VAL[array_pos]))
            {
                
                OutputAssemblyCode.Address.push_back(command_for_prompt);

                char temp2[5]="";
                switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[array_pos])
                {
                    //double word instructions
                    case PIC18F_DW_CASE:
                    case PIC18F_DW_CASE2:
                    case PIC18F_CALL_CASE:
                    case PIC18F_LIT_CASE2:
                            
                        if(counter+(unsigned)7<MachineCode.size())//sometimes double word instructions are spread accross two file records
                        {
                            snprintf(temp2,sizeof(temp2),"%c%c%c%c",MachineCode[counter+6],MachineCode[counter+7],MachineCode[counter+4],MachineCode[counter+5]);
                        }
                        else //data is inside the next record
                        {
                            //find the next entry
                            while(HEX_FILE_CONTENTS[file_position]!=':')
                            {
                                file_position++;
                            }
                            //and copy over the bytes
                            snprintf(temp2,sizeof(temp2),"%c%c%c%c",HEX_FILE_CONTENTS[file_position+11],HEX_FILE_CONTENTS[file_position+12],HEX_FILE_CONTENTS[file_position+9],HEX_FILE_CONTENTS[file_position+10]);
                            final_byte_is_double_word=true;
                        }
                        // copy second word to opcode buffer
                        WORD_2 = strtol(temp2, NULL, 16);
                        recordOpcode(OutputAssemblyCode,WORD_2);
                        
                        if(disassemble_PIC18F_DoubleWord(n,WORD_2, Instruction_Set.PIC18F_MNEMONICS[array_pos], Instruction_Set.PIC18F_OPCODES_DECODE_CASE[array_pos], command_for_prompt,address&0xffff)==true)
                        {
                            counter+=4;//double word
                            address+=2;
                        }
                        else    //sometimes ascii data aliases double word instructions, if this occurs the above statement will return false
                        {
                            final_byte_is_double_word=false;
                        }

                        break;

                    //no operation instructions
                    case PIC18F_NOP_CASE:

                        //ascii data will alias as NOP instructions, so only "FFFF" is treated as NOP
                        if(strcmp(temp,"FFFF")==0)
                        {
                            strcat(command_for_prompt,"NOP");
                        }
                        //Everything else becomes "unknown" so that no information is lost
                        else
                        {
                            strcat(command_for_prompt,"UNKNOWN[");
                            strcat(command_for_prompt,temp);
                            strcat(command_for_prompt,"]");
                        }
                        break;
                    case PIC18F_NO_ARGS:
                        strcat(command_for_prompt,Instruction_Set.PIC18F_MNEMONICS[array_pos].c_str());
                        break;

                    //single word statements
                    default:
                        disassemble_with_PIC18f_SingleWord(n,Instruction_Set,array_pos, command_for_prompt,address&0xffff);
                        break;
                }

                finalise_command(command_for_prompt,OutputAssemblyCode,Instruction_Set.PIC18F_DESCRIPTIONS[array_pos].c_str(),Address_Upper_16_bits,Instruction_Set.FLASH_size,n);
                Instruction_found = true;
                break;
            }

            array_pos++;
        }
        if(Instruction_found==false)
        {
            OutputAssemblyCode.Address.push_back(command_for_prompt);
            uint8_t upperByte = 0xff&(n>>8);
            uint8_t lowerByte = n & 0xff;
            char *UPPER =removeEscapeCharacter(upperByte);
            char *LOWER =removeEscapeCharacter(lowerByte);
            snprintf(command_for_prompt,command_for_prompt_len_max,"    DB \"%s\"\n    DB \"%s\"",UPPER,LOWER);
            OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(command_for_prompt);
            
            free(UPPER);
            free(LOWER);
        }
    }
    free(command_for_prompt);
    return;
}




void finalise_command(char *&command_for_prompt,
                      Converted_Assembly_Code &OutputAssemblyCode,
                      const char *Description,
                      uint16_t Address_Upper_16_bits,
                      uint32_t device_mem_size,
                      uint16_t n )
{
    bool include_comments=false;
    size_t end_of_meta_data=16;
    
    trackTableReads(command_for_prompt,OutputAssemblyCode,n);
    watchFunctionStacks(command_for_prompt,OutputAssemblyCode);
    //00007efa    EF6E    GOTO 0x719a
    //12345678901234567890
    uint8_t index =0 ;
    const std::vector<std::string> instructionList = {"GOTO","BC","BN","BNN","BNC","BNOV","BNZ","BOV","BRA","BZ","CALL","RCALL"};
    
    size_t endOfCommand = check_against_list_of_instructions(instructionList, &command_for_prompt[20],  index);
    
    if(endOfCommand!=0) // zero indicates command not present
    {
        const char *Branching_Command = instructionList[index].c_str();
        
        //find the position to move to
        size_t p1 = (strstr(command_for_prompt,Branching_Command)-command_for_prompt)+strlen(Branching_Command);
        size_t p2 = strlen(command_for_prompt);
        size_t x = (strstr(command_for_prompt,"x")-command_for_prompt)+1;//mem address

        char *nextComma = strstr(&command_for_prompt[p1],",");
        
        if(nextComma!=NULL)
        {
            p2 = nextComma-command_for_prompt;
        }

        char *pre_label = copy_out_substring(end_of_meta_data,p1,command_for_prompt);

        char *label_address = copy_out_substring(x-strlen("0x"),p2,command_for_prompt);

        char *comment = copy_out_substring(p2,strlen(command_for_prompt),command_for_prompt);

        char *Addr= copy_out_substring(x,p2,command_for_prompt);

        OutputAssemblyCode.LABEL_POSITIONS.push_back(strtol(Addr, NULL, 16)/2);

        size_t label_len = strlen(label_address)+strlen(" LABEL_:")+strlen(command_for_prompt)+100;
        char label[label_len+1];
        
        snprintf(label,label_len,"%s LABEL_%s %s",pre_label,label_address,comment);

        //the label may be refering to external memory
        if((unsigned)strtol(label_address,NULL,16)>device_mem_size)
        {
            snprintf(label,label_len,"%sLABEL_%s %s//External Memory",pre_label,label_address,comment);
            std::cout <<"Address (" << label_address  << ") Exceeds Internal Storage (" << device_mem_size << ")\n";
        }

        /*
         optionally add comments explaining what each command does
         */
        if(include_comments==true)
        {
            OutputAssemblyCode.CommentAddress.push_back((uint32_t)OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size());
            OutputAssemblyCode.Comments.push_back(Description);
        }
        OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(label);

        strtok(label_address," ");
        snprintf(label,label_len,"LABEL_0x%.4X:",(unsigned)strtol(label_address,NULL,16));
        OutputAssemblyCode.LABEL_STRINGS.push_back(label);

        //free(Branching_Command);
        free(label_address);
        free(comment);
        free(Addr);
        free(pre_label);
    }
    else
    {
        if(include_comments==true)
        {
            OutputAssemblyCode.CommentAddress.push_back((uint32_t)OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size());
            OutputAssemblyCode.Comments.push_back(Description);
        }

        char *temp =copy_out_substring(end_of_meta_data,strlen(command_for_prompt),command_for_prompt);

        OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);

        free(temp);
    }

}

