//
//  DisassemblerLabels.cpp
//  annoyed
//
//  Created by Greg Norman on 2/2/2023.
//

#include "DisassemblerLabels.hpp"
void addLabelsToAssemblyCode(Converted_Assembly_Code &OutputAssemblyCode)
{
    for (uint64_t  pos = 0;pos<OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size();pos++)
    {
        char *Addr=copy_out_substring(0,8,OutputAssemblyCode.Address[pos].c_str());
        
        uint64_t Address = strtol(Addr, NULL, 16);
        
        free(Addr);
        Address=Address/2;
        
        for (uint64_t  n = 0;n<OutputAssemblyCode.LABEL_POSITIONS.size();n++)
        {
            if(OutputAssemblyCode.LABEL_POSITIONS[n] == Address)
            {
                std::string temp = OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos];
                
                OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.at(pos) = OutputAssemblyCode.LABEL_STRINGS[n] + "\n" + temp;
                
                break;
            }
        }
    }
}
