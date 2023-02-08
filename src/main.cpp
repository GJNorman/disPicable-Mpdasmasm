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
#include "PIC18_IS.h"
#include "BasicFileEditing.hpp"
#include "Disassembler.hpp"
#include "Assembler.hpp"
#include "Timer.h"
#include <filesystem>


#ifdef _WIN32
#include<malloc.h>
const char *Global_working_directory = "C:/Users/Administrator/Desktop/Assembler/Assembler";
#endif

#ifdef __APPLE__
const char *Global_working_directory ="/Users/gregnorman/Desktop/annoyed";
#endif

void compare_generated_and_original_hex_files(const char *OrignialFileDir,const char *GeneratedFileDir);
void getFileLists();
void selectFile(uint32_t &file_selector,std::vector<std::string> &fileList);
std::vector<std::string> hexFileList;
std::vector<std::string> headerList;
std::vector<std::string> asmList;

//print a list of files in the current directory for the user to choose from
void selectFile(uint32_t &file_selector,std::vector<std::string> &fileList)
{
    bool validOption = false;
    
    uint32_t counter=0;
    for(auto it: fileList)
    {
        std::cout << counter <<"\t\t" << it << "\n";
        counter++;
    }
    while(validOption == false)
    {
        std::cout << "Please Select File: ";
        scanf("%d",&file_selector);
        
        if(file_selector>fileList.size())
        {
            std::cout << "Invalid option\n";
        }
        else
        {
            std::cout <<"Selected: " << fileList[file_selector] <<"\n";
            validOption = true;
        }
    }
    
}
// search through current directory, grab files with certain extensions
void getFileLists()
{
    std::cout << "Current path is " << std::filesystem::current_path() << '\n';
    const std::filesystem::path path{Global_working_directory}; // (3)};
    for (auto const& dir_entry : std::filesystem::directory_iterator{ path })
    {
        if(strstr(dir_entry.path().c_str(),".hex"))
        {
            std::string temp = dir_entry.path().c_str();
            
            hexFileList.push_back(temp);
        }
        else if(strstr(dir_entry.path().c_str(),".inc"))
        {
            std::string temp = dir_entry.path().c_str();
            
            headerList.push_back(temp);
        }
        else if(strstr(dir_entry.path().c_str(),".asm"))
        {
            std::string temp = dir_entry.path().c_str();
            
            asmList.push_back(temp);
        }
        
    }
}
int main()
{
    PIC18F_FULL_IS Instruction_Set;

    bool bDisplayBinContents = false;
    bool bDisplayAssembly = false;

    uint32_t file_selector = 0;

    printf("Display Binary:     %s\n", bDisplayBinContents ? "true" : "false");
    printf("Display Assembly:   %s\n", bDisplayAssembly ? "true" : "false");

    char filedir[300]="";

    uint32_t headerSelector = 0;
    
    getFileLists();
    selectFile(headerSelector,headerList);
    
    while(1)
    {
        uint32_t function_selector = 0;
        std::cout << "Select Mode: "<< function_selector++ <<" -> Convert Hex File to PIC Assembly\n";
        std::cout << "             "<< function_selector++ <<" -> Convert PIC Assembly to Hex File\n";
        std::cout << "             "<< function_selector++ <<" -> Compare Original and Generated Hex Files\n";
        std::cout << "             "<< function_selector++ <<" -> Quit\n";
        std::cout << "             "<< function_selector++ <<" -> Change Settings\n";
        
        printf("Select Mode: ");
        scanf("%u",&function_selector);

        
        if(function_selector==0)
        {
            selectFile(file_selector,hexFileList);
            
            Instruction_Set.SetupRegisterNames(headerList[headerSelector].c_str());
            
            std::vector<unsigned char> FileContents;
            snprintf(filedir,sizeof(filedir),"%s",hexFileList[file_selector].c_str());

            Copy_FIRMWARE_FILE_to_Buffer(filedir,FileContents);
            Disassemble(FileContents,  bDisplayBinContents, bDisplayAssembly,Instruction_Set);

            FileContents.clear();
        }
        if(function_selector==1)
        {
            selectFile(file_selector,asmList);
            snprintf(filedir,sizeof(filedir),"%s",asmList[file_selector].c_str());
            Assemble(filedir, bDisplayBinContents, bDisplayAssembly,Instruction_Set);
            
        }

        if(function_selector==2)
        {
            uint32_t firstFile = 0;
            uint32_t secondFile = 0 ;
            
            std::cout << "select original file\n";
            selectFile(firstFile,hexFileList);
            std::cout << "select generated file:\n";
            selectFile(secondFile,hexFileList);
            
            snprintf(filedir,sizeof(filedir),"%s",hexFileList[firstFile].c_str());
            char generatedFileDir[300]="";
            snprintf(generatedFileDir,sizeof(filedir),"%s",hexFileList[secondFile].c_str());
            compare_generated_and_original_hex_files(filedir,generatedFileDir);
        }
        if(function_selector==3)
        {
            break;
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










