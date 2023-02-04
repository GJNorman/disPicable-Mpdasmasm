//
//  DisassemblerComments.cpp
//  annoyed
//
//  Created by Greg Norman on 4/2/2023.
//

#include "DisassemblerComments.hpp"

bool include_comments=true;
bool include_command_descriptions=false;

// for formatting data as comments
static void addData(char *&command_for_prompt, const char Assembly_Comment_Marker,const char *Description)
{
    
    
    // if the line already has another comment, we will create a newline
    if(strstr(command_for_prompt,";"))
    {
        snprintf(command_for_prompt, 1024, "%s\n%*c",command_for_prompt,52, Assembly_Comment_Marker);
    }
    else
    {
        int command_width = 52 - (int)strlen(command_for_prompt);
        snprintf(command_for_prompt, 1024, "%s%*c",command_for_prompt,command_width, Assembly_Comment_Marker);
    }

    strcat(command_for_prompt,Description);
}

// adds an explanantion of what each command does
// i.e. MOVFF 0x50, 0x51    ;MOVFF fs,fd Move File Fs to File Fd
void AddComments(char *&command_for_prompt, const char Assembly_Comment_Marker,const char *Comment)
{
    if(include_comments==true)
    {
        addData(command_for_prompt,Assembly_Comment_Marker,Comment);
    }
}

void AddAllComments(Converted_Assembly_Code &OutputAssemblyCode)
{
    for(size_t index=0; index< OutputAssemblyCode.Comments.size(); index++)
    {
        AddComments(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[OutputAssemblyCode.CommentAddress[index]],
                    ';',
                    OutputAssemblyCode.Comments[index].c_str());
    }
}
// overloaded versions
static void addOverloaded(std::string &command_for_prompt, const char Assembly_Comment_Marker,const char *Description)
{
    char *temp=(char*)malloc(sizeof(char)*(command_for_prompt.size() +2));
    
    snprintf(temp,command_for_prompt.size() + 1, "%s",command_for_prompt.c_str());
    
    addData(temp,Assembly_Comment_Marker,Description);
    
    command_for_prompt.clear();
    command_for_prompt.resize(strlen(temp)+1);
    command_for_prompt = temp;
    
    free(temp);
}

void AddComments(std::string &command_for_prompt, const char Assembly_Comment_Marker,const char *Comment)
{
    if(include_comments==true)
    {
        addOverloaded(command_for_prompt,Assembly_Comment_Marker,Comment);
    }
}

