//
//  AssemblerFilePreprocessing.cpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#include "AssemblerFilePreprocessing.hpp"

const char *DefineByteDirective = "DB ";
std::string skip_inline_comments(std::string line_from_file,std::ifstream &input_file,char seperating_character, const char *CommentCharacter);

void process_binary_file_line(std::string &line,std::string &next_line,Converted_Assembly_Code &Machine_Code_File,PIC18F_FULL_IS &Instruction_Set, uint32_t &File_Position_Counter, bool &borrowed_byte_from_next_line,std::ifstream &input_file);

/*
  line is valid if it has any relevant ascii characters present
 */
bool check_assembly_file_line_is_valid(std::string line,Converted_Assembly_Code &Machine_Code_File,PIC18F_FULL_IS &Instruction_Set,uint32_t &File_Position_Counter);

void Convert_Databyte_into_hex_data(std::string line,std::string next_line,Converted_Assembly_Code &Machine_Code_File , uint32_t &File_Position_Counter,bool &borrowed_byte_from_next_line,PIC18F_FULL_IS &Instruction_Set);

void process_standard_line_from_binary(Converted_Assembly_Code &Machine_Code_File, uint32_t &File_Position_Counter, std::string line);

/*
    store user defined bytes
 */
void find_define_byte_tags(Converted_Assembly_Code &Machine_Code_File,std::string line, const uint32_t File_Position_Counter,PIC18F_FULL_IS &Instruction_Set,uint8_t address_offset);



bool check_file_fits_memory(Converted_Assembly_Code &Machine_Code_File,PIC18F_FULL_IS &Instruction_Set)
{
    uint32_t max_size = 0;

    for(auto address : Machine_Code_File.Address)
    {
        uint32_t total_size= strtol(address.c_str(),NULL,16) & 0xffff;
        if((total_size > max_size)&&(strstr(address.c_str(),"NOP")==NULL))
        {
            max_size = total_size;
        }

    }
    max_size+=2;    //max size points to start of instruction address, we need it to point to end of the isntruction
    
    printf("The program uses 0x%.4X of 0x%.4X Bytes of Memory\nThere are %d bytes (%d Instructions) remaining\n",max_size,Instruction_Set.FLASH_size ,Instruction_Set.FLASH_size -max_size+1,(signed)(Instruction_Set.FLASH_size -max_size+1)/2);
    //each line uses 2 bytes of memory
    if(max_size > Instruction_Set.FLASH_size + 2)
    {
   //     return false;
    }
    return true;
}
/*
    make sure that the comment marker is not part of a string
 
    " Hello World; "                | This is allowed
 
    " Hello World"  ; some comment  | only the comment will be removed
 
    " Hello World;" ; some comment  | only comment should be removed
 */
bool checkIfCommentCharacterIsPartOfAStringOrChar(std::string line_from_file, size_t &comment_Position, const char *commentMarker)
{
    
    bool thisIsAString = false;
    
    char searchChar[2] = "\'";
    size_t MakeSureThisIsntPartOfAStringOrChar = line_from_file.find("'");  // character definition ' '
    
    if(MakeSureThisIsntPartOfAStringOrChar == std::string::npos)
    {
        searchChar[0] ='\"';
        MakeSureThisIsntPartOfAStringOrChar = line_from_file.find("\""); // string definition " "
    }
    if(MakeSureThisIsntPartOfAStringOrChar != std::string::npos)
    {
        if(MakeSureThisIsntPartOfAStringOrChar < comment_Position)
        {
            // make sure the comment isn't after the terminating character
            
            MakeSureThisIsntPartOfAStringOrChar = line_from_file.find(searchChar,MakeSureThisIsntPartOfAStringOrChar+1);
            
            if(MakeSureThisIsntPartOfAStringOrChar!=std::string::npos)
            {
                if(MakeSureThisIsntPartOfAStringOrChar > comment_Position)
                {
                    thisIsAString = true;
                    
                    // in case there is a comment after a string containing a comment marker
                    comment_Position = MakeSureThisIsntPartOfAStringOrChar+1;
                    comment_Position = line_from_file.find(commentMarker,comment_Position);
                    
                    // remove inline comments
                    if(comment_Position != std::string::npos)
                    {
                        thisIsAString = checkIfCommentCharacterIsPartOfAStringOrChar(line_from_file.substr(comment_Position,line_from_file.size()),comment_Position,commentMarker);
                    }
                }
            }
        }
    }
    return thisIsAString;
}
// for skipping in line comments
std::string skip_inline_comments(std::string line_from_file,std::ifstream &input_file,char seperating_character, const char *CommentCharacter)
{

    // search for the next valid line
    bool lineIsNotEmpty = false;
    
    while(lineIsNotEmpty == false)
    {
        std::string temp;
        line_from_file.clear();
        
        /*
            Read in the next chunk of code
         */
        
            bool entireEntryHasBeenRead = true; //assume a single line statement
            bool multiLineCommentReadIn = false;
            do
            {
                entireEntryHasBeenRead = true;
                if(!getline(input_file, temp,seperating_character))
                {
                    return "END OF FILE";
                }
                
                /*
                 
                    Disable reading in data if a multi line comment is detected
                 */
                    if(temp.find("/*")!=std::string::npos)
                    {
                        multiLineCommentReadIn = true;
                    }
                    
                    
                    if(multiLineCommentReadIn == false)
                    {
                        line_from_file+=temp;
                        
                        if(line_from_file.c_str()[line_from_file.size()-1] == '\\')
                        {
                            //line_from_file = line_from_file.substr(0,line_from_file.size()-1);
                            entireEntryHasBeenRead = false;
                        }
                    }
                    else
                    {
                        entireEntryHasBeenRead = false;
                    }
                    
                    if(temp.find("*/")!=std::string::npos)
                    {
                        multiLineCommentReadIn = false;
                    }
                
            }while(entireEntryHasBeenRead == false);//  // '\' indicates one line spread accross multiple

        /*
         
         remove leading white spaces
         */

            RemoveLeadingSpaces(line_from_file, 0);
        /*
         Check that the code is valid
         */
            lineIsNotEmpty = check_if_line_is_empty(line_from_file);
            
            if( lineIsNotEmpty == true )
            {
                const char *inlineCommentCharacters[] = {";","//"};
                
                for(uint8_t index=0; index< 2; index++)
                {
                    size_t remove_comment = line_from_file.find(inlineCommentCharacters[index]);
                    
                    // remove inline comments
                    if(remove_comment != std::string::npos)
                    {
                        if(checkIfCommentCharacterIsPartOfAStringOrChar(line_from_file,remove_comment,inlineCommentCharacters[index]) == false)
                        {
                            //remove comment
                            line_from_file = line_from_file.substr(0,remove_comment);
                            
                            // check if the comment-cleaned line is empty
                            lineIsNotEmpty = check_if_line_is_empty(line_from_file);
                        }
                        
                    }
                }
            }
    }
    return line_from_file;
}

bool check_assembly_file_line_is_valid(std::string line,Converted_Assembly_Code &Machine_Code_File,PIC18F_FULL_IS &Instruction_Set,uint32_t &File_Position_Counter)
{
    for(uint32_t it = 0; it<Instruction_Set.PIC18F_MNEMONICS.size(); it++)
    {
        if((strstr(line.c_str(),Instruction_Set.PIC18F_MNEMONICS[it].c_str()))
           || (strstr(line.c_str(), "ORG"))      //Memory origin
           || (strstr(line.c_str(),"UNKNOWN")))  //unknown command
        {
            process_standard_line_from_binary(Machine_Code_File, File_Position_Counter, line);
            return true;
        }
    }
    return false;
}

//copy the line from the assembly file into the assembly code struct
//record the instructions address
void process_standard_line_from_binary(Converted_Assembly_Code &Machine_Code_File, uint32_t &File_Position_Counter, std::string line)
{
    Machine_Code_File.ASSEMBLY_CODE_FULL_PROGRAM.push_back(line);
    
    //store the memory address
        char temp[10]="";
        snprintf(temp,sizeof(temp), "%.4X",File_Position_Counter);
        Machine_Code_File.Address.push_back(temp);
        File_Position_Counter+=2;
    
    
    //check for double word instructions
        uint8_t index=0;
        if(check_against_list_of_instructions({"MOVFF", "CALL", "GOTO", "LFSR"}, line.c_str(),  index))
        {
            File_Position_Counter+=2;
        }
        
    //org directive overwrites calculated address
        if(strstr(line.c_str(),"ORG"))
        {
            char *ORG_address = return_substring(line.c_str(), "ORG", NULL, 0,strlen("ORG"), 0);
            if(ORG_address!=NULL)
            {
                File_Position_Counter = strtol(ORG_address,NULL,16)&0xffff;     //copy ORG address into working address
                free(ORG_address);
            }
        }
}

// example
// Label:   DB      'x',1,2,3
//          DB      "hello"

void find_define_byte_tags(Converted_Assembly_Code &Machine_Code_File,std::string line, const uint32_t File_Position_Counter,PIC18F_FULL_IS &Instruction_Set,uint8_t address_offset)
{

    size_t p1 =  CheckForLeadingSpaces(line.c_str(),0);

    size_t p2 = FindNextSpaceCharacter(line, p1) ;
    
    if(line[p2] == ':') // colon is optional and not used when calling label
    {
        p2--;
    }

    if(address_offset==2)
    {
        address_offset=1;
    }

    // save the label and address
    Instruction_Set.Define_Byte_Tags.push_back(line.substr(p1,p2-p1));
    Instruction_Set.Define_byte_tag_positions.push_back(File_Position_Counter-address_offset);
    
    //printf("%.4X\t%.4X\t '%s'\n",File_Position_Counter,File_Position_Counter-address_offset,line.substr(p1,p2-p1).c_str());
        
}

// example inputs
//  label           Directive       Value
//  ScanPrompt:     DB              "\r\n LOGFILE SCAN:\0"
//
void Convert_Databyte_into_hex_data(std::string line,std::string next_line,Converted_Assembly_Code &Machine_Code_File , uint32_t &File_Position_Counter,bool &borrowed_byte_from_next_line,PIC18F_FULL_IS &Instruction_Set)
{
    char * data_bytes = return_substring(line.c_str(), "\"", "\"", 0, strlen("\""), 0);
    
    if(data_bytes!=NULL)
    {
        char temp[20]="";
                            //because of little endian and some laziness
        char byte_two[3]="";
        char byte_one[3]="";
        
        uint8_t starting_position_in_db = 0;
        if(borrowed_byte_from_next_line==true)
        {
            if(data_bytes[0] == '\\')
            {
                starting_position_in_db++;
            }
            starting_position_in_db++;
            borrowed_byte_from_next_line=false;
        }
        
        find_define_byte_tags(Machine_Code_File,line,File_Position_Counter,Instruction_Set,starting_position_in_db);
        
        uint8_t original_string_length = strlen(data_bytes);        //There might be null characters before the end of the string
        
        
        for(uint8_t counter=starting_position_in_db; counter < original_string_length; )
        {
            check_for_escape_characters(counter,data_bytes);
            snprintf(byte_two,sizeof(byte_two),"%.2X",data_bytes[counter]);

            if(counter+1 < original_string_length)
            {
                counter++;
                check_for_escape_characters(counter,data_bytes);
                snprintf(byte_one,sizeof(byte_one),"%.2X",data_bytes[counter]);
            }
            else
            {
                // if the next line also contains a defined byte directive, then we can save space by smooshing them together
                if(strstr(next_line.c_str(),DefineByteDirective))
                {
                    borrowed_byte_from_next_line = true;
                    char *next_byte = return_substring(next_line.c_str(), "\"", "\"", 0, strlen("\""), 0);
                    
                    uint8_t next_byte_counter=0;
                    
                    check_for_escape_characters(next_byte_counter,next_byte);
                    
                    snprintf(byte_one,sizeof(byte_one),"%.2X",next_byte[next_byte_counter]);
                    free(next_byte);
                }
                else
                {
                    snprintf(byte_one,sizeof(byte_one),"FF");
                }
            }

            snprintf(temp,sizeof(temp),"UNKNOWN[%s%s]",byte_one,byte_two);
            counter++;
            
            Machine_Code_File.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);
            File_Position_Counter+=2;
                
            snprintf(temp,sizeof(temp), "%.4X",File_Position_Counter);
            Machine_Code_File.Address.push_back(temp);
           
        }
        free(data_bytes);
    }
}
void process_binary_file_line(std::string &line,std::string &next_line,Converted_Assembly_Code &Machine_Code_File,PIC18F_FULL_IS &Instruction_Set, uint32_t &File_Position_Counter, bool &borrowed_byte_from_next_line,std::ifstream &input_file)
{
    /*
            Directives
     */
    if(strstr(line.c_str(),DefineByteDirective))    // data storage directive
    {
        Convert_Databyte_into_hex_data(line,next_line,Machine_Code_File,File_Position_Counter, borrowed_byte_from_next_line,Instruction_Set);
    }
    else if((strstr(line.c_str(),"CONFIG_BITS"))||((strstr(line.c_str(),"DEVICE_ID"))||((strstr(line.c_str(),"EEPROM_DIRECTIVE")))))
    {
        Machine_Code_File.ASSEMBLY_CODE_FULL_PROGRAM.push_back(line);
        Machine_Code_File.Address.push_back("0000");
    }
    
    // TODO - completely fix this
    else if((strstr(line.c_str(),":")))     //reserved for labels
    {
        Machine_Code_File.LABEL_POSITIONS.push_back(File_Position_Counter);
        Machine_Code_File.LABEL_STRINGS.push_back(line);
    }
    else if(line.find("#")!=std::string::npos)
    {
        checkPreprocesseorDirectives(line,File_Position_Counter);
    }
    else if(line.find("equ ") != std::string::npos)
    {
        addNewEQU(line);
    }
    else if((line.find("DB ")!= std::string::npos)||(line.find("db ")!= std::string::npos))
    {
        process_standard_line_from_binary( Machine_Code_File,  File_Position_Counter, line);
    }
    
    //check for macro invocation
    else if(checkForMacro(line,File_Position_Counter) == true)
    {
        size_t p1 = 0;
        size_t p2 = 0;
        while( (p2 = line.find("\\",p1))!=std::string::npos)    //used for multiline macros
        {
            process_standard_line_from_binary(Machine_Code_File,File_Position_Counter,line.substr(p1,p2-p1));
            p1 = p2+1;
            
        }
    }

    //check that there is an instruction on the line
    else if(!check_assembly_file_line_is_valid(line,Machine_Code_File,Instruction_Set,File_Position_Counter))
    {
        std::cout << "Unknown Instruction or Directive: " << line<< std::endl;
    }

}
Converted_Assembly_Code Copy_Over_Binary_File(const char *inputfiledir,PIC18F_FULL_IS &Instruction_Set)
{
    
    std::cout << "Reading File: "<< inputfiledir << "\n";
    Converted_Assembly_Code Machine_Code_File;

    // some programs will add carriage returns to each line
    char *new_file = delete_all_the_stupid_carriage_returns(inputfiledir);
   
    if(new_file == NULL)
    {
        std::cout << "File Not Found\n";
        return Machine_Code_File;
    }

    // load the file
    std::ifstream input_file;
    input_file.open(new_file);
    
    
    std::string line = "FIRST ITERATION";       //prevent processing on first operation
    std::string next_line ="";                      //we need to use 2 lines for the "db" directives,
                                                //if the strings are odd (including null) then we can borrow the first byte of the next line if it is also a "db"
    bool borrowed_byte_from_next_line = false;  //this is because we normally use two bytes per instruction. but a db may be an odd number
    
    char seperating_character = '\n';
    
    uint32_t File_Position_Counter = 0 ;        // keep track of which line we're on

    
    while(1)
    {
        next_line = skip_inline_comments(next_line,input_file,seperating_character, ";");
        
        if(next_line == "END OF FILE")
        {
            break;
        }
        if(line!="FIRST ITERATION")
        {
            process_binary_file_line(line, next_line,Machine_Code_File, Instruction_Set, File_Position_Counter, borrowed_byte_from_next_line,input_file);
        }
        line = next_line;
    }
    next_line=";";
    process_binary_file_line(line,next_line,Machine_Code_File, Instruction_Set, File_Position_Counter, borrowed_byte_from_next_line,input_file);
    
    free(new_file);
    input_file.close();

    return Machine_Code_File;
}
