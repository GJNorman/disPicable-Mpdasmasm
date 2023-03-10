//
//  DisassemblerComments.cpp
//  
//
//  Created by Greg Norman on 4/2/2023.
//

#include "DisassemblerComments.hpp"

bool include_comments=true;
bool include_command_descriptions=false;

constexpr int commentSpacing = 52;
// for formatting data as comments
static void addData(char *&command_for_prompt, const char Assembly_Comment_Marker,const char *Description, int max_len)
{
    char *reference_point = command_for_prompt;
   
    // in case there are multyiple lines - well move straigh to the final one
    while(strstr(reference_point,"\n"))
    {
        reference_point = strstr(reference_point,"\n") + strlen("\n");
    }
    
    // if the line already has another comment, we will create a newline
    if(strstr(reference_point,";"))
    {
        snprintf(&reference_point[strlen(reference_point)], max_len-strlen(command_for_prompt), "\n");
        
        reference_point += strlen(reference_point); // move to end
    }
    
    // we now add the comment
    int command_width = commentSpacing - (int)(strlen(reference_point));
    
    snprintf(&reference_point[strlen(reference_point)],
             max_len-strlen(command_for_prompt),
             "%*c%s",command_width, Assembly_Comment_Marker,Description);
}

// adds an explanantion of what each command does
// i.e. MOVFF 0x50, 0x51    ;MOVFF fs,fd Move File Fs to File Fd
void AddComments(char *&command_for_prompt, const char Assembly_Comment_Marker,const char *Comment, int max_len)
{
    if(include_comments==true)
    {
        addData(command_for_prompt,Assembly_Comment_Marker,Comment,max_len);
    }
}

void AddAllComments(Converted_Assembly_Code &OutputAssemblyCode)
{
    for(size_t index=0; index< OutputAssemblyCode.Comments.size(); index++)
    {
        if(OutputAssemblyCode.CommentAddress[index] < OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size())
        {
            AddComments(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[OutputAssemblyCode.CommentAddress[index]],
                        ';',
                        OutputAssemblyCode.Comments[index].c_str());
        }
    }
}

// overloaded versions
static void addOverloaded(std::string &command_for_prompt, const char Assembly_Comment_Marker,const char *Description)
{
    // allocate the absolute maximum amount of size to the buffer,
    // if a comment is already detected on the command, then the next one will be placed underneath it
    
    constexpr int max_num_comments_per_line = 10;
    int allocationSize = sizeof(char)*((int)command_for_prompt.size() + (int)strlen(Description) + max_num_comments_per_line*commentSpacing +1);
    char *temp=(char*)malloc(sizeof(char)*(allocationSize));
    
    if(temp!= NULL)
    {
        snprintf(temp,command_for_prompt.size()+1 , "%s",command_for_prompt.c_str());
        
        addData(temp,Assembly_Comment_Marker,Description,allocationSize);
        
        command_for_prompt = temp;
        
        free(temp);
    }
}

void AddComments(std::string &command_for_prompt, const char Assembly_Comment_Marker,const char *Comment)
{
    if(include_comments==true)
    {
        addOverloaded(command_for_prompt,Assembly_Comment_Marker,Comment);
    }
}

