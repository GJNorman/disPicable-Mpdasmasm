//
//  DisassemblerComments.hpp
//  annoyed
//
//  Created by Greg Norman on 4/2/2023.
//

#ifndef DisassemblerComments_hpp
#define DisassemblerComments_hpp

#include <stdio.h>
#include <string>
#include "AssemblerFilePreprocessing.hpp"
// add comments that are generated during the disassembling process
// for example, addresses of table reads and function arguments
// adds an explanantion of what each command does
// i.e. MOVFF 0x50, 0x51    ;MOVFF fs,fd Move File Fs to File Fd
void AddComments(char *&command_for_prompt, const char Assembly_Comment_Marker,const char *Comment);

// add all the disassembler generated comments at once
void AddAllComments(Converted_Assembly_Code &OutputAssemblyCode);

// overloads for std::string
void AddComments(std::string &command_for_prompt, const char Assembly_Comment_Marker,const char *Comment);
#endif /* DisassemblerComments_hpp */
