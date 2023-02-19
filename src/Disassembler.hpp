//
//  Disassembler.hpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#ifndef Disassembler_hpp
#define Disassembler_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdint.h>
#include "PIC18_IS.h"
#include "BasicFileEditing.hpp"
#include "AssembleDisassembleCommonFunctions.hpp"
#include "AssembleDirectives.hpp"
#include "AssemblerFilePreprocessing.hpp"
#include "DisassembleDirectives.hpp"
#include "DisassemblerInstructionProcessor.hpp"
#include "DisassemblerComments.hpp"
#include "DisassemblerFileWriter.hpp"
#include "DisassemblerHexFileParser.hpp"
#include "Timer.h"

void Disassemble(const char *hexFile,                   // input .hex file
                 const char* HeaderFile,                // input .inc file (optional)
                 bool bDisplayBinContents,              // debugging - display opcodes
                 bool bDisplayAssembly,                 // debugging - display assembled code
                 PIC18F_FULL_IS &Instruction_Set);      // pic instruction set

#endif /* Disassembler_hpp */
