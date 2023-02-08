//
//  AssemblerMachineCodeWriter.hpp
//  
//
//  Created by Greg Norman on 28/1/2023.
//

#ifndef AssemblerMachineCodeWriter_hpp
#define AssemblerMachineCodeWriter_hpp

#include <iostream>
#include <sstream>
#include <stdarg.h>
#include "AssemblerAddressAndChecksum.hpp"
// print to one or both of std::out, or a file
void printHexCode(bool display_on_screen, const char *outputDir);

// copies assembled hex file data to buffer
void output_Machine_Code(const char *Format_Specifier, ...);

// clear hex file buffer
void clearMachineCodeBuffer();

// get access to hex file buffer
std::ostringstream *grabMachineCodeBuffer();

// add "NOP"s until we reach the end of a line
void padFile(uint32_t &address, uint16_t &check_sum);
#endif /* AssemblerMachineCodeWriter_hpp */
