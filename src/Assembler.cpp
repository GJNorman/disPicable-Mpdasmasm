//
//  Assembler.cpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#include "Assembler.hpp"

extern char Global_working_directory[FILENAME_MAX];

TimerClass MyTimer(0);                                          // for benchmarking
Converted_Assembly_Code AssemblyCode;                           // stores Assembly program


void Assemble(const char *inputfiledir, bool bDisplayBinContents,bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set)
{
    MyTimer.updateTimerReference();
    clearIncludedFileDirectories();
    clearEQU();
    AssemblyCode = Copy_Over_Binary_File(inputfiledir,Instruction_Set);

    if(AssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size() == 0)
    {
        std::cout << "Error: File Not Found: " << inputfiledir << "\n";
        return;
    }
    if(!check_file_fits_memory(AssemblyCode,Instruction_Set))
    {
        std::cout << "ERROR - NOT ENOUGH FLASH MEMORY" << std::endl;
        return; 
    }
    
    std::vector<include_directory_t> includedFiles = checkForIncludedFiles();
    
    std::string Annoying = "/"; // gives an error to use as literal
    for(auto it: includedFiles)
    {
        if(it.type == INCLUDE_RELATIVE){
            std::string FileDir = Global_working_directory + Annoying + it.name;
            AssemblyCode.append(Copy_Over_Binary_File(FileDir.c_str(),Instruction_Set));
        }
    }
    //printEQUs();

    uint32_t START_ADDRESS=0;
    uint32_t address=0x00;                  // current program counter
    uint16_t address_upper_16bits=0;
    uint16_t check_sum=0x00;
    bool check_sum_required=true;
    
    
    output_Machine_Code("%s",":020000040000FA");

    size_t counter =0;
    for (auto Assembly_Instruction: AssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM)
    {

        if(true){
          //  std::cout<< AssemblyCode.Address[counter] << "\t\t"<<Assembly_Instruction<< "\n";
            counter++;
        }
        bool instruction_found =  processInstruction(Assembly_Instruction,
                                                     Instruction_Set,
                                                     address,
                                                     check_sum_required,
                                                     check_sum,
                                                     address_upper_16bits,
                                                     START_ADDRESS);
        if(instruction_found==false)
        {
            processDirective(Assembly_Instruction,
                             address,
                             check_sum_required,
                             check_sum,
                             Instruction_Set,
                             address_upper_16bits,
                             START_ADDRESS);
        }
        
        
    }
    padFile(address,check_sum);

    //End of FILE
    output_Machine_Code("%s","\r\n:00000001FF\r\n");
    
    char output_dir[FILENAME_MAX]="";
    snprintf(output_dir,sizeof(output_dir),"%s/AssembledCode.hex",Global_working_directory);
    printHexCode(bDisplayAssembly,output_dir);
    
    std::cout << "Total Time to Assemble: " <<MyTimer.CheckDuration() << " seconds\n";
}




