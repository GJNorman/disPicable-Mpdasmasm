//
//  AssemblerMachineCodeWriter.hpp
//  annoyed
//
//  Created by Greg Norman on 28/1/2023.
//

#ifndef AssemblerMachineCodeWriter_hpp
#define AssemblerMachineCodeWriter_hpp

#include <iostream>
#include <sstream>
#include <stdarg.h>
void printHexCode(bool display_on_screen, const char *outputDir);
void output_Machine_Code(const char *Format_Specifier, ...);
void clearMachineCodeBuffer();
std::ostringstream *grabMachineCodeBuffer();
#endif /* AssemblerMachineCodeWriter_hpp */
