//
//  DisassemblerHexFileParser.hpp
//
//
//  Created by Greg Norman on 19/2/2023.
//

#ifndef DisassemblerHexFileParser_hpp
#define DisassemblerHexFileParser_hpp

#include <iostream>
#include <vector>
#include <string>
#include "AssemblerFilePreprocessing.hpp"
#include "PIC18_IS.h"
#include "DisassemblerInstructionProcessor.hpp"
#include "DisassembleDirectives.hpp"

void parseHexFile(std::vector<unsigned char> hexFileContents,           // input file
                  Converted_Assembly_Code &OutputAssemblyCode,          // output file
                  PIC18F_FULL_IS &Instruction_Set,                      // pic instruction set
                  bool bDisplayAssembly,                                // debug - display assembly code on console
                  bool bDisplayBinContents)                             // debug - display opcodes on console
;
#endif /* DisassemblerHexFileParser_hpp */
