//
//  BasicFileEditing.cpp
//  annoyed
//
//  Created by Greg Norman on 27/1/2023.
//

#include "BasicFileEditing.hpp"

std::string remove_tabs(std::string next_line)
{
    for(auto it = 0; it < next_line.size(); it++)
    {
        if(next_line[it] == '\t')
        {
            next_line[it] = ' ';
        }
    }
    return next_line;
}
bool  check_data_bytes(char &data)
{
    if(data=='r')
    {
        data = '\r';
    }
    else if(data=='n')
    {
        data = '\n';
    }
    else if(data=='0')
    {
        data = '\0';
        return true;
    }
    else if(data=='\\')
    {
        data = '\\';
        return true;
    }
    else if(data=='t')
    {
        data = '\t';
        return true;
    }
    else
    {
        printf("lol what? %c\n",data);
    }
    return false;
}
// in a string that is formatting with literal escape characters
void check_for_escape_characters(uint8_t &counter, char *&data_bytes)
{
    if(data_bytes[counter] == '\\')
    {
        counter++;
        check_data_bytes(data_bytes[counter]);
    }
}
void Copy_FIRMWARE_FILE_to_Buffer(const char *Input_File_dir,std::vector<unsigned char> &OutputFileContents)
{
    std::ifstream input( Input_File_dir, std::ios::binary );

    if(!input)
    {
        std::cout << "Could not find file\n";
    }
    std::copy
    (
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>( ),
        std::back_inserter(OutputFileContents)
    );

}

char *copy_file_to_buffer(const char *inputfiledir)
{
    FILE *fp = fopen(inputfiledir,"rb");
    if(!fp)
    {
        printf("File not found: %s\n",inputfiledir);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    uint64_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *ASM_BUFFER = (char*)calloc(sizeof(char),file_size*2+1);

    fread(ASM_BUFFER,sizeof(char) , file_size, fp);

    fclose(fp);

    return ASM_BUFFER;
}
char *copy_out_substring(size_t p1, size_t p2, const char* parent)
{
    char *temp = (char*)malloc(sizeof(char) * (p2-p1+1));
    strncpy(temp,parent+p1,p2-p1);
    temp[p2-p1]=0;

    return temp;
}
size_t findTermInCharString(size_t defaultValue, const char *parent_String,const char *searchTerm,size_t search_start_offset,size_t search_end_offset)
{

    if(searchTerm == NULL)
    {
        return defaultValue;
    }
    const char *point1 = strstr(&parent_String[search_start_offset],searchTerm);
    
    if(point1==NULL)
    {
        return ~0;
    }
    
    size_t p1 = point1-parent_String+search_end_offset;
    
    return p1;
}

char* return_substring(const char *parent_String,
                       const char *searchTerm1,
                       const char *searchTerm2,
                       size_t search_start_offset,
                       size_t search_end_offset,
                       size_t search_end_offset2)
{
    size_t p1 = findTermInCharString(0,parent_String,searchTerm1,search_start_offset,search_end_offset);;
    
    if(p1 == ~0)
    {
        return NULL;
    }
    size_t p2 = findTermInCharString(strlen(parent_String),parent_String,searchTerm2,p1,search_end_offset2);
    
    if(p2 == ~0)
    {
        return NULL;
    }
    
    if(p2-p1<0)
    {
        printf("ERROR PARSING: %s\n",parent_String);
        return NULL;
    }
    
    return copy_out_substring(p1,p2,parent_String);
}

void removeCharacters(const char *CharacterToRemove, const char *ReplacementCharacter, char *buffer)
{
    char *NextCarriage = strstr(buffer,CharacterToRemove);
    while(NextCarriage!= NULL)
    {
        size_t p1 = NextCarriage-buffer;//(strstr(buffer,"\r")-buffer);
        
        for (uint8_t index=0; index< strlen(ReplacementCharacter); index++)
            buffer[p1+index] =ReplacementCharacter[index];

        NextCarriage = strstr(&buffer[p1],CharacterToRemove);
       
    }
}
char* delete_all_the_stupid_carriage_returns(const char *input_file)
{
    
    size_t temp_file_name_length =strlen(input_file)+strlen("temp_____") + 1;
    char *temp_file=(char*)malloc(sizeof(char)*(temp_file_name_length));
    
    
    char *buffer = copy_file_to_buffer(input_file);
    
    
    
    if(buffer!=NULL)
    {
        char *main_name = return_substring(input_file, NULL, ".", 0,0, 0);
        
        if(main_name!=NULL)
        {
            snprintf(temp_file,temp_file_name_length,"%s_temp.txt",main_name);
            FILE * fp = fopen( temp_file,"wb");

            //printf("%s",buffer);
            removeCharacters("\r","\n ",buffer);
            removeCharacters("\t"," ",buffer);

            fprintf(fp,"%s\n",buffer);

            fclose(fp);
            free(main_name);
        }

    }
    return temp_file;
}
bool check_if_line_is_empty(std::string line_from_file)
{
    //look for any character other than a space, or a newline
    //tabs and carriage returns will have already been removed by this point
    for(uint16_t counter = 0; counter < line_from_file.size();counter++)
    {
        //any valid ascii character is accepted
        for (unsigned char  ascii_counter =0x0 ; ascii_counter <0xff ; ascii_counter++)
        {
            if((ascii_counter!= '\n'))
            {
                if(ascii_counter!=' ')
                {
                    if(line_from_file[counter] == ascii_counter)
                    {
                        return true;// line_from_file;
                    }
                }
            }
        }
    }
    //if the line has nothing useful, we'll copy over a comment character, the parent function will then loop for another cycle
    return false;
}
size_t CheckForLeadingSpaces(const char *data, size_t start_pos)
{
    size_t p1 = start_pos;
    
    while((data[p1] == ' ') && (p1<strlen(data)))
    {
        p1++;
    }
    
    return p1;
}
size_t RemoveLeadingSpaces(std::string &data, size_t start_pos)
{
   //remove leading spaces
    size_t p1 = CheckForLeadingSpaces(data.c_str(),start_pos);
   
   if((p1 != start_pos) && (p1 != data.size() ))
       data = data.substr(p1,data.size());
   
   return p1;
}


size_t FindNextSpaceCharacter(const char* data, size_t start_pos)
{
    size_t p1 = start_pos;
    
    while((data[p1] != ' ') && (p1<strlen(data) ))
    {
        p1++;
    }
    
    return p1;
}
// find the position of the first chracter that indicates that the current word has ended
// this will not work for strings literals
size_t FindEndOfCurrentWord(const char *data, size_t start_pos)
{
    size_t p1 = start_pos;
    
    while((data[p1] != ' ')&&
          (data[p1] != ',')&&
          (data[p1] != '}')&&
          (data[p1] != ')')&&
          (data[p1] != ':')&&
          (p1<strlen(data)))
    {
        p1++;
    }
    
    return p1;
    
}
size_t FindNextSpaceCharacter(std::string &data, size_t start_pos)
{
    size_t p1 = FindNextSpaceCharacter(data.c_str(),start_pos);
   
   return p1;
}

size_t findWhereCurrentWordBegan(const char* data,size_t currentPos)
{
    
    size_t p1 = currentPos;
    
    while((data[p1] != ' ') &&(data[p1] != ',')&&(data[p1] != '(') && (p1!=0))
    {
        p1--;
    }
    
    return p1;
}


char *removeEscapeCharacter(char character)
{
    char *temp = (char*)malloc(5);
    
    temp[0] = character;
    // get rid of escape sequences
    switch(character)
    {
            
        case '\?':snprintf(temp,sizeof(temp),"(\\?)"); break;
        case '\a':snprintf(temp,sizeof(temp),"(\\a)"); break;
        case '\b':snprintf(temp,sizeof(temp),"(\\b)"); break;
        case '\f':snprintf(temp,sizeof(temp),"(\\f)"); break;
        case '\n':snprintf(temp,sizeof(temp),"(\\n)"); break;
        case '\r':snprintf(temp,sizeof(temp),"(\\r)"); break;
        case '\t':snprintf(temp,sizeof(temp),"(\\t)"); break;
        case '\v':snprintf(temp,sizeof(temp),"(\\v)"); break;
        case '\0':snprintf(temp,sizeof(temp),"(\\0)"); break;
        default:
            break;
    }
    
    return temp;
}
