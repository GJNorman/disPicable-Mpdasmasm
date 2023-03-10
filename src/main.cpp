//
//  main.cpp
//
//
//  Created by Greg Norman on 19/5/21.
//

#include <iostream>
#include <iterator>
#include <vector>
#include <string.h>
#include <stdarg.h>
#include <sstream>
#ifdef _WIN32
#include<malloc.h>  // xcode gives an error
#endif
#include "PIC18_IS.h"
#include "BasicFileEditing.hpp"
#include "Disassembler.hpp"
#include "Assembler.hpp"
#include "Timer.h"
#include <filesystem>
#include "ArgumentParser.hpp"



char Global_working_directory[FILENAME_MAX];

void compare_generated_and_original_hex_files(const char *OrignialFileDir,const char *GeneratedFileDir);

std::vector<std::string> hexFileList;
std::vector<std::string> headerList;
std::vector<std::string> asmList;


/*
 input arguments are used to specify
 
    1    - function     i.e.   "dasm" or "asm"
    2    - header file, i.e.   pic18f45k50.inc      or (for comparison)     originalFile.hex
    3    - sourcefile   i.e.   myasmfile.asm                                generatedFile.hex
    
 
    
 */
int main(int argc, char **argv)
{
    
    constexpr int min_num_arguments = 4;    // less than this and the program will look into the local directory and present a list for the user to select files/functions from
    
    // set working directory
    snprintf(Global_working_directory, sizeof(Global_working_directory), "%s",std::filesystem::current_path().c_str() );
    
    PIC18F_FULL_IS Instruction_Set;

    bool bDisplayBinContents = false;
    bool bDisplayAssembly = false;

    uint32_t file_selector = 0;
    uint32_t headerSelector = 0;
    uint32_t function_selector = 0;

    
    if(argc < min_num_arguments)
    {
        // prompt user for required files
        getFileLists(hexFileList,headerList,asmList);
        function_selector = printFunctionSelector();

        selectFile(headerSelector,headerList);
        
        if(function_selector == ASSEMBLER)
        {
            selectFile(file_selector,asmList);
        }
        else if (function_selector == DISASSEMBLER)
        {
            selectFile(file_selector,hexFileList);
        }
        else if(function_selector!=COMPARE_FILES){
            
            std::cout << "select original file\n";
            selectFile(file_selector,hexFileList);
            std::cout << "select generated file:\n";
            selectFile(headerSelector,hexFileList);
        }
    }
    else
    {
        constexpr int FunctionArgument = 1;
        constexpr int File1Argument = 2;
        constexpr int File2Argument = 3;
        // use argv for selecting inputs
        function_selector = checkFunctionArgument(argv[FunctionArgument]);
        
        if(function_selector!=COMPARE_FILES)
        {
            check_INC_FILE(argv[File1Argument],headerList);
            
            if(function_selector == ASSEMBLER)
            {
                check_ASM_File(argv[File2Argument],asmList);
            }
            else if (function_selector == DISASSEMBLER)
            {
                check_HEX_File(argv[File2Argument],hexFileList);
            }
        }
        else
        {
            check_HEX_File(argv[File1Argument],hexFileList);
            check_HEX_File(argv[File2Argument],hexFileList);
            
            file_selector = 0;
            headerSelector = 1;
        }
    }

    if(function_selector==DISASSEMBLER)
    {
        Instruction_Set.SetupRegisterNames(headerList[headerSelector].c_str());
        Disassemble(hexFileList[file_selector].c_str(),headerList[headerSelector].c_str(),  bDisplayBinContents, bDisplayAssembly,Instruction_Set);
    }
    if(function_selector==ASSEMBLER)
    {
        Assemble(asmList[file_selector].c_str(), bDisplayBinContents, bDisplayAssembly,Instruction_Set);
    }
    if(function_selector==COMPARE_FILES)
    {
        compare_generated_and_original_hex_files(hexFileList[file_selector].c_str(),hexFileList[headerSelector].c_str());
    }
    if(function_selector==4)
    {
        char temp;
        printf("Display Assembly Code (y/n)? ");

        scanf(" %c",&temp);

        if((temp=='Y')||(temp=='y'))
        {
            bDisplayAssembly=true;
        }
        else
        {
            bDisplayAssembly=false;
        }

        printf("Display Binary Code (y/n)? ");
        scanf(" %c",&temp);
        if((temp=='Y')||(temp=='y'))
        {
            bDisplayBinContents=true;
        }
        else
        {
            bDisplayBinContents=false;
        }
        printf("Display Binary:     %s\n", bDisplayBinContents ? "true" : "false");
        printf("Display Assembly:   %s\n", bDisplayAssembly ? "true" : "false");
    }

    return 0;
}

void compare_generated_and_original_hex_files(const char *OrignialFileDir,const char *GeneratedFileDir)
{
    FILE *f_org = fopen(OrignialFileDir,"rb");
    FILE *f_gen = fopen(GeneratedFileDir,"rb");

    char line_org[1024]="";
    char line_gen[1024]="";

    int total_errors=0;
    while(fgets(line_org,1000,f_org))
    {
        if(fgets(line_gen,1000,f_gen)==NULL)
        {
            printf("Generated Hex file is shorter\n");
            break;
        }
        if(strcmp(line_gen,line_org)!=0)
        {
            printf("Error: \nGenLine%sOrgLine%s",line_gen,line_org);
            total_errors++;
        }
    }
    printf("There are %d total errors\n",total_errors);

    fclose(f_org);
    fclose(f_gen);
}










