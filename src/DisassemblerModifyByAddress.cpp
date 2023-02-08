//
//  DisassemblerModifyByAddress.cpp
//  
//
//  Created by Greg Norman on 6/2/2023.
//

#include "DisassemblerModifyByAddress.hpp"

void modifyBasedOnAddress(Converted_Assembly_Code &OutputAssemblyCode, std::vector<uint64_t> AddressVector,std::vector<std::string> DataToConcat, bool addOnNewLine, bool prepend)
{
    // read through the program
    for (uint64_t  pos = 0;pos<OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size();pos++)
    {
        // grab each instruction's address
        char *Addr=copy_out_substring(0,8,OutputAssemblyCode.Address[pos].c_str());
        
        uint64_t Address = strtol(Addr, NULL, 16);
        
        free(Addr);
        Address=Address/2;
        
        // read through the data address list
        for (uint64_t  n = 0;n<AddressVector.size();n++)
        {
            // if the address match, then we add the extra data
            if(AddressVector[n] == Address)
            {
                if(prepend == true)
                {
                    std::string temp="\n"+ OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos];
                    
                    OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.at(pos) = DataToConcat[n] + temp;
                }
                else
                {
                    AddComments(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos], ';',DataToConcat[n].c_str());
                }
                break;
            }
        }
    }
}
