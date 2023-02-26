//
//  Disassembler.cpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#include "Disassembler.hpp"
extern char Global_working_directory[FILENAME_MAX];
TimerClass MyTimerDissassembler;


// read in the header file passed as an input argument
static void ReadMCUHeader(const char * HeaderFile,PIC18F_FULL_IS &Instruction_Set)
{
    if(HeaderFile == NULL)
    {
        return;
    }
    clearIncludedFileDirectories();
    
    char fileDir[FILENAME_MAX];
    
    if(HeaderFile[0]!='/')  // relative directory
    {
        snprintf(fileDir,sizeof(fileDir),"%s/%s",Global_working_directory,HeaderFile);
    }
    else                    // absolute directory
    {
        snprintf(fileDir,sizeof(fileDir),"%s",HeaderFile);
    }
    Copy_Over_Binary_File(fileDir,Instruction_Set);
    
}


void Disassemble(const char *hexFile,                   // input .hex file
                 const char *HeaderFile,                // input .inc file (optional)
                 bool bDisplayBinContents,              // debugging - display opcodes
                 bool bDisplayAssembly,                 // debugging - display assembled code
                 PIC18F_FULL_IS &Instruction_Set)       // pic instruction set
{

    MyTimerDissassembler.updateTimerReference();
    
    std::vector<unsigned char> hexFileContents;                 // holds contents of input file (".hex")
    
    Copy_FIRMWARE_FILE_to_Buffer(hexFile,hexFileContents);
    
    Converted_Assembly_Code OutputAssemblyCode;                 // holds contents of output file (".asm")
    
    clearEQU();
    
    ReadMCUHeader(HeaderFile,Instruction_Set);
  
    // read through hex file, convert opcodes to assembly
    parseHexFile(hexFileContents,OutputAssemblyCode,Instruction_Set,bDisplayAssembly, bDisplayBinContents);
    
    HighlightSFRs(OutputAssemblyCode);
    
    markTableReads(OutputAssemblyCode,Instruction_Set);

    AddAllComments(OutputAssemblyCode);
    
    markAllAsciiData(OutputAssemblyCode);
    
    addLabelsToAssemblyCode(OutputAssemblyCode);

    WriteAssemblyCodeToFile(OutputAssemblyCode,HeaderFile,bDisplayBinContents, bDisplayAssembly,Instruction_Set);
    
    std::cout << MyTimerDissassembler.CheckDuration() << "  seconds\n";
}



