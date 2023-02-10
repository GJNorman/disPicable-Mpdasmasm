//
//  ArgumentParser.cpp
//  
//
//  Created by Greg Norman on 10/2/2023.
//

#include "ArgumentParser.hpp"

extern char Global_working_directory[FILENAME_MAX];

uint32_t printFunctionSelector()
{
    uint32_t function_selector = 0;
    std::cout << "Select Mode: "<< function_selector++ <<" -> Convert Hex File to PIC Assembly\n";
    std::cout << "             "<< function_selector++ <<" -> Convert PIC Assembly to Hex File\n";
    std::cout << "             "<< function_selector++ <<" -> Compare Original and Generated Hex Files\n";
    std::cout << "             "<< function_selector++ <<" -> Quit\n";
    std::cout << "             "<< function_selector++ <<" -> Change Settings\n";
    
    printf("Select Mode: ");
    scanf("%u",&function_selector);
    return function_selector;
}
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
void getFileLists(std::vector<std::string> &hexFileList,std::vector<std::string> &headerList,std::vector<std::string> &asmList)
{
    const std::filesystem::path path{Global_working_directory};
    for (auto const& dir_entry : std::filesystem::directory_iterator{ path })
    {
        /*
         For generating files for CMakeList.txt
         if(strstr(dir_entry.path().c_str(),".cpp"))
        {
            std::string temp =dir_entry.path().c_str();
            
            size_t pos = temp.find("annoyed/");
            
            if(pos != std::string::npos){
                pos += strlen("annoyed/");
                std::cout << "target_sources(${PROJECT_NAME} PRIVATE "<< temp.substr(pos,temp.size() - pos) << ")\n";
            }
        }*/
        if(strstr(dir_entry.path().c_str(),".hex"))         // firmware files
        {
            std::string temp = dir_entry.path().c_str();
            
            hexFileList.push_back(temp);
        }
        else if(strstr(dir_entry.path().c_str(),".inc"))    // MCU headers
        {
            std::string temp = dir_entry.path().c_str();
            
            headerList.push_back(temp);
        }
        else if(strstr(dir_entry.path().c_str(),".asm"))    // assembly source
        {
            std::string temp = dir_entry.path().c_str();
            
            asmList.push_back(temp);
        }
        
    }
}
static void CheckFileArg(char * FileName, const char *fileExt, std::vector<std::string> &fileList, std::string ErrorPrompt)
{
    bool validArg = false;
    if(FileName!=NULL)
    {
        if(strstr(FileName,fileExt)!=NULL)
        {
            std::string temp = FileName;
            if(FileName[0] != '/')  // relative directory
            {
                std::string annoying = "/";
                temp = Global_working_directory+annoying + FileName;
            }
            fileList.push_back(temp);
            validArg = true;
        }
    }
    if(validArg==false){
        std::cout << ErrorPrompt << std::endl;
        exit(0);
    }
}
void check_INC_FILE(char * FileName, std::vector<std::string> &headerList)
{
    CheckFileArg(FileName,".inc", headerList,".inc file is invalid\n");
}
void check_ASM_File(char * FileName, std::vector<std::string> &asmList)
{
    CheckFileArg(FileName,".asm", asmList,".asm file is invalid\n");
}
void check_HEX_File(char * FileName, std::vector<std::string> &hexList)
{
    CheckFileArg(FileName,".hex", hexList,".hex file is invalid\n");
}
uint32_t checkFunctionArgument(const char *function)
{
    uint32_t file_selector = ~0;
    
    if(strstr(function, "dasm") != NULL){
        file_selector = DISASSEMBLER;
    }
    else if (strstr(function, "asm") != NULL)
    {
        file_selector = ASSEMBLER;
    }
    else if  (strstr(function, "cmp") != NULL)
    {
        file_selector = COMPARE_FILES;
    }
    
    return file_selector;
}
