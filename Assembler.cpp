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

    cleanupPreviousWork();
    
    AssemblyCode = Copy_Over_Binary_File(inputfiledir,Instruction_Set);

    if(checkValidityOfFile(AssemblyCode,inputfiledir,Instruction_Set) == false)
    {
        return ;
    }

    appendIncludedFiles(AssemblyCode,Instruction_Set);

    Address_And_Checksum_t CurrentAddress = {0};
    
    output_Machine_Code(":020000040000FA");

    
    for (auto Assembly_Instruction: AssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM)
    {

        if(bDisplayAssembly == true){
            static size_t counter =0;
            std::cout<< AssemblyCode.Address[counter] << "\t\t"<<Assembly_Instruction<< "\n";
            counter++;
        }
        bool instruction_found =  processInstruction(Assembly_Instruction,Instruction_Set,CurrentAddress);
        if(instruction_found==false)
        {
            processDirective(Assembly_Instruction,CurrentAddress,Instruction_Set);
        }
        
        
    }

    padFile(CurrentAddress);

    //End of FILE
    output_Machine_Code("%s","\r\n:00000001FF\r\n");
    
    char output_dir[FILENAME_MAX];
    snprintf(output_dir,sizeof(output_dir),"%s/AssembledCode.hex",Global_working_directory);
    printHexCode(bDisplayAssembly,output_dir);
    
    std::cout << "Total Time to Assemble: " <<MyTimer.CheckDuration() << " seconds\n";
}




