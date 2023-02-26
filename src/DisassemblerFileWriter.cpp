//
//  DisassemblerFileWriter.cpp
//  
//
//  Created by Greg Norman on 18/2/2023.
//

#include "DisassemblerFileWriter.hpp"

extern char Global_working_directory[FILENAME_MAX];

void AddRegisterToFunctionHeader(std::string RegistersList, FILE *fp, const char *functionHeader)
{
    size_t offset =0 ;
    while(strlen(&RegistersList.c_str()[offset])  > strlen(functionHeader))
    {
        offset += strlen(functionHeader) ;
        offset = findWhereCurrentWordBegan(RegistersList.c_str(), offset);
        
        const char InsertionString[] = "\n;     ";
        RegistersList.insert(offset, InsertionString);
        
        offset+=strlen("\n");
    }
    fprintf(fp,"\n;%s",RegistersList.c_str());
}
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
        
        const char functionHeader[] =";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;";
        if((strstr(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos].c_str(),"RET"))//RETFIE, RETURN
           ||(functionCounter==0))
        {
            
            // print a list of special function registers used in the function
            fprintf(fp,"%s",functionHeader);
            
            // add special function registers (i.e. ADCON1)
            std::string SFRs = grabSFRList(functionCounter);
            
            AddRegisterToFunctionHeader(SFRs, fp, functionHeader);
            
            // add any literal RAM address (i.e. 0x107)
            std::string literals = grabLiteralRegisterList(functionCounter);
            AddRegisterToFunctionHeader(literals, fp, functionHeader);
            
            fprintf(fp,"\n%s\n",functionHeader);
            
            
            functionCounter++;
        }
        
    }
    fclose(fp);

}
