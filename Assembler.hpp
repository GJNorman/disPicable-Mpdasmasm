//
//  Assembler.hpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#ifndef Assembler_hpp
#define Assembler_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdint.h>
#include "PIC18_IS.h"
#include "BasicFileEditing.hpp"
#include "MacroEngine.hpp"
#include "AssembleDisassembleCommonFunctions.hpp"

#include "AssemblerFilePreprocessing.hpp"
#include "AssemblerInstructionProcessor.hpp"

#include "AssembleDirectives.hpp"
#include "AssemblerMachineCodeWriter.hpp"
#include "Timer.h"

void Assemble(const char *inputfiledir, bool bDisplayBinContents,bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set);



#endif /* Assembler_hpp */
