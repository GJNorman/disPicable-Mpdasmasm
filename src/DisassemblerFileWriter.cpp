//
//  DisassemblerFileWriter.cpp
//  
//
//  Created by Greg Norman on 18/2/2023.
//

#include "DisassemblerFileWriter.hpp"

extern char Global_working_directory[FILENAME_MAX];

void WriteAssemblyCodeToFile(Converted_Assembly_Code &OutputAssemblyCode, const char *HeaderFile,bool bDisplayBinContents, bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set)
{
    char output_dir[FILENAME_MAX];
    snprintf(output_dir,sizeof(output_dir),"%s/DisassembledCode.asm",Global_working_directory);
    FILE *fp = fopen(output_dir,"w");
    
    // MCU header
    if(HeaderFile != NULL)
    {
        if(HeaderFile[0] != '/')
        {
            fprintf(fp,"#include \"%s\"\n",HeaderFile);
        }
        else
        {
            fprintf(fp,"#include <%s>\n",HeaderFile);
        }
    }
    uint32_t functionCounter = 0;
    for (uint64_t  pos = 0;pos<OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size();pos++)
    {
        
        if(bDisplayBinContents)
        {
            std::cout<<OutputAssemblyCode.Address[pos] << " ";
        }
        if(bDisplayAssembly)
        {
            std::cout<< OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos]<<std::endl;
        }
        else if(bDisplayBinContents)
        {
            std::cout<<std::endl;
        }
        
        fprintf(fp,"%s\n",OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos].c_str());
        
        if((strstr(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos].c_str(),"RET"))//RETFIE, RETURN
           ||(functionCounter==0))
        {
            // print a list of special function registers used in the function
            fprintf(fp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n;");
            std::string SFRs = grabFSRList(functionCounter);
            functionCounter++;
            fprintf(fp,"%s",SFRs.c_str());
            fprintf(fp,"\n;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n");
        }
        
    }
    fclose(fp);

}
