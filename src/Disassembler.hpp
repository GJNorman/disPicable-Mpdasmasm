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
#include "Timer.h"

void Disassemble(std::vector<unsigned char> &OutputFileContents, bool bDisplayBinContents,bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set);

#endif /* Disassembler_hpp */
