//
//  MacroEngine.h
//  annoyed
//
//  Created by Greg Norman on 26/1/2023.
//
/*
#ifndef MacroEngine_h
#define MacroEngine_h

#include <stdint.h>
#include <string>
#include <vector>

typedef struct{
    
    char *tag;
    size_t scope_start;     // i.e. line where #define statement is
    size_t scope_end;       // i.e. line where #undef statement is, or EOF
}Macro_definition_t;


static std::vector<Macro_definition_t> MacroList;

void createListOfUserMacros(std::vector<std::string> ASSEMBLY_CODE_FULL_PROGRAM)
{
    size_t line_counter = 0;
    for(auto NextLine : ASSEMBLY_CODE_FULL_PROGRAM)
    {
        // check for a potential preprocessor directive
        if(NextLine.find("#")!= std::string::npos)
        {
            if(NextLine.find("#define")!= std::string::npos)
            {
                
            }
        }
        line_counter++;
    }
    
}
*/
//#endif /* MacroEngine_h */
