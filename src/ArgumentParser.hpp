//
//  ArgumentParser.hpp
//
//
//  Created by Greg Norman on 10/2/2023.
//

#ifndef ArgumentParser_hpp
#define ArgumentParser_hpp

#include <iostream>
#include <stdint.h>
#include <vector>
#include <string>
#include <filesystem>

enum FunctionSelectors{
    
    DISASSEMBLER = 0,
    ASSEMBLER = 1,
    COMPARE_FILES = 3,
};

// these functions are called if the user doesn't provide files as input arguments
uint32_t printFunctionSelector();
// create lists of files with relevant extension in the working directory
void getFileLists(std::vector<std::string> &hexFileList,std::vector<std::string> &headerList,std::vector<std::string> &asmList);

// present a list of files for the user to select from
void selectFile(uint32_t &file_selector,std::vector<std::string> &fileList);

// command line arguments
void check_INC_FILE(char * FileName, std::vector<std::string> &headerList);
void check_HEX_File(char * FileName, std::vector<std::string> &hexList);
void check_ASM_File(char * FileName, std::vector<std::string> &asmList);
// find out if this is the assembler or disassembler
uint32_t checkFunctionArgument(const char *function);


#endif /* ArgumentParser_hpp */
