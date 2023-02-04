//
//  MacroEngine.hpp
//  annoyed
//
//  Created by Greg Norman on 26/1/2023.
//

#ifndef MacroEngine_hpp
#define MacroEngine_hpp

#include <stdint.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "BasicFileEditing.hpp"
typedef struct{
                                    // #define SUM(x,y) x=x+y
    std::string tag;                // i.e. 'SUM'
    std::vector<std::string> args;  // '{x,y}'
    std::string definition;         // x=x+y
    size_t scope_start;             // i.e. line where #define statement is
    size_t scope_end;               // i.e. line where #undef statement is, or EOF
}Macro_definition_t;

enum {
    MACRO_DEFINITION = 1,
    INCLUDE_RELATIVE = 2,
    INCLUDE_ABSOLUTE = 3,
};
typedef struct{
    uint8_t type;           // relative or absolute
    std::string name;       // filename
}include_directory_t;
// returns a file name if the directive is '#include'
void createListOfUserMacros(std::vector<std::string> CODE_FULL_PROGRAM); // create the entire list
void checkPreprocesseorDirectives(std::string &SingleLine, size_t fileLine);    // create a single entry

bool checkForMacro(std::string &Line, size_t filePos);                          // check if a line is referencing a known macro
void printUserMacros();                                                         // std::cout macro list

std::vector<include_directory_t> checkForIncludedFiles();
void clearIncludedFileDirectories();
#endif /* MacroEngine_hpp */
