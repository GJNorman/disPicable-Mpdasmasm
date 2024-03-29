//
//  BasicFileEditing.cpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#include "BasicFileEditing.hpp"

// replace tabs with spaces
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

void CopyBufferToFile(const char *Input_File_dir,std::vector<unsigned char> &OutputFileContents)
{
    std::ofstream output_file(Input_File_dir);
    for (const auto &e : OutputFileContents) output_file << e ;
}
// copies binary file contents into ram Buffer
void CopyFileToBuffer(const char *Input_File_dir,std::vector<unsigned char> &OutputFileContents,std::ios_base::openmode mode)
{
    //std::ios::binary
    std::ifstream input( Input_File_dir,mode);

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

/*
    return a substring found between the terms "searchTerm1" and "searchTerm2"
 
    the offsets can be used to include "searchTerm1/2" in the substring
 
 
    "x=15;"
 
     return_substring("x=15;","x=",";",0,strlen("x="),0);
     
     will return "15"
 */
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

// takes one character and replces it with another. There is no realloc in this function, so strings must be the same size
void removeCharacters(const char *CharacterToRemove, const char *ReplacementCharacter, char *buffer)
{
    char *NextCarriage = strstr(buffer,CharacterToRemove);
    int counter=0;
    while(NextCarriage!= NULL)
    {
        size_t p1 = NextCarriage-buffer;
        
        for (uint8_t index=0; index< strlen(ReplacementCharacter); index++)
            buffer[p1+index] =ReplacementCharacter[index];

        NextCarriage = strstr(&buffer[p1],CharacterToRemove);
        
        counter++;
       
    }
    std::cout << "we removed: " << counter << "\n";
}
void removeCharacters(const char CharacterToRemove, const char ReplacementCharacter, std::vector<unsigned char> &buffer)
{
    
    for(size_t index=0 ; index< buffer.size(); ++index)
    {
        if(buffer[index] == CharacterToRemove)
        {
            buffer[index] = ReplacementCharacter;
        }
        
    }
}

// some text editors play a fun game called "insert a bunch of silly nonsense"
// this is our way of fighting back against tyranny
char* delete_all_the_stupid_carriage_returns(const char *input_file)
{
    size_t temp_file_name_length =strlen(input_file)+strlen("temp_____") + 1;
    char *temp_file=(char*)malloc(sizeof(char)*(temp_file_name_length));
        
   // char *buffer = copy_file_to_buffer(input_file);

    std::vector<unsigned char> buffer;
    CopyFileToBuffer(input_file, buffer, std::ifstream::in);
    if(buffer.size()!=0)
    {
        char *main_name = return_substring(input_file, NULL, ".", 0,0, 0);
        
        if(main_name!=NULL)
        {
            snprintf(temp_file,temp_file_name_length,"%s_temp.txt",main_name);

            removeCharacters('\r',' ',buffer);
            removeCharacters('\t',' ',buffer);

            CopyBufferToFile(temp_file,buffer);

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
    
    while((data[p1] == ' ') && (p1<strlen(data)))   // this is not the same as "FindNextSpaceCharacter"!
    {
        p1++;
    }
    
    return p1;
}
//remove leading spaces
size_t RemoveLeadingSpaces(std::string &data, size_t start_pos)
{

    size_t p1 = CheckForLeadingSpaces(data.c_str(),start_pos);
   
   if((p1 != start_pos) && (p1 != data.size() ))
       data = data.substr(p1,data.size());
   
   return p1;
}

// either find the next space, or else the end of the string
size_t FindNextSpaceCharacter(const char* data, size_t start_pos)
{
    size_t p1 = start_pos;
    
    while((data[p1] != ' ') && (p1<strlen(data) ))
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

// currentPos is pointing to the end of a word
// we read backwards until we find a charcter that would indicate the start of a word (or the beginning of the string)
size_t findWhereCurrentWordBegan(const char* data,size_t currentPos)
{
    
    size_t p1 = currentPos;
    
    while((data[p1] != ' ') &&(data[p1] != ',')&&(data[p1] != '(') && (p1!=0))
    {
        p1--;
    }
    
    return p1;
}

// adds an escape slash to an escape character for literal formatting
void removeEscapeCharacter(char character, char* dst, size_t dst_size)
{
    // get rid of escape sequences
    switch(character)
    {
            
        case '\?':snprintf(dst,dst_size,"\\?"); break;
        case '\a':snprintf(dst,dst_size,"\\a"); break;
        case '\b':snprintf(dst,dst_size,"\\b"); break;
        case '\f':snprintf(dst,dst_size,"\\f"); break;
        case '\n':snprintf(dst,dst_size,"\\n"); break;
        case '\r':snprintf(dst,dst_size,"\\r"); break;
        case '\t':snprintf(dst,dst_size,"\\t"); break;
        case '\v':snprintf(dst,dst_size,"\\v"); break;
        case '\0':snprintf(dst,dst_size,"\\0"); break;
        default:
            dst[0] = character;
            dst[1] = '\0';
            break;
    }
    
    return ;
}
