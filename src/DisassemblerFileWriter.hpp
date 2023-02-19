//
//  DisassemblerFileWriter.hpp
//  
//
//  Created by Greg Norman on 18/2/2023.
//

#ifndef DisassemblerFileWriter_hpp
#define DisassemblerFileWriter_hpp

#include <iostream>
#include "AssemblerFilePreprocessing.hpp"
#include "DisassemblerFunctionStacks.hpp"
#include "PIC18_IS.h"

void WriteAssemblyCodeToFile(Converted_Assembly_Code &OutputAssemblyCode,   // code to be written to file
                             const char *HeaderFile,                        // optional - name of the header file to #include
                             bool bDisplayBinContents,                      // for debugging - display opcodes
                             bool bDisplayAssembly,                         // for debugging - display assembly code
                             PIC18F_FULL_IS &Instruction_Set);              // associated instruction set
#endif /* DisassemblerFileWriter_hpp */
