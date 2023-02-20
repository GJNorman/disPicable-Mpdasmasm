//
//  AssemblerFilePreprocessing.hpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#ifndef AssemblerFilePreprocessing_hpp
#define AssemblerFilePreprocessing_hpp

#include <iostream>
#include <vector>
#include <string>
#include "PIC18_IS.h"
#include "BasicFileEditing.hpp"
#include "AssembleDisassembleCommonFunctions.hpp"
#include "MacroEngine.hpp"
#include "AssembleDirectives.hpp"
#include "EQUs.hpp"
class Converted_Assembly_Code{
public:
    // data for label
    std::vector<uint64_t> LABEL_POSITIONS;                          // label address
    std::vector<std::string> LABEL_STRINGS;                         // label name
    
    // code data
    std::vector<std::string> Address;                               // address of command
    std::vector<std::string> ASSEMBLY_CODE_FULL_PROGRAM;            // assembly (readable) code
    std::vector<uint16_t> OPCODES;                                  // hex opcodes
    
    // comments
    std::vector<std::string> Comments;
    std::vector<uint32_t> CommentAddress;                        // address of comment
    
    // table reads
    std::vector<uint32_t> TBLRD_Target_Addresses;                   // possible location of ascii data
    std::vector<uint32_t> AddressOfTableReadCommand;                // address that the TBLRD command is located at
    std::vector<uint8_t> TableReadArgumentType;                     // argument as literal or indirect value

    void append(Converted_Assembly_Code NewCode)
    {
        for (auto it: NewCode.LABEL_POSITIONS)
            this->LABEL_POSITIONS.push_back(it);
        for (auto it: NewCode.LABEL_STRINGS)
            this->LABEL_STRINGS.push_back(it);
        for (auto it: NewCode.Address)
            this->Address.push_back(it);
        for (auto it: NewCode.ASSEMBLY_CODE_FULL_PROGRAM)
            this->ASSEMBLY_CODE_FULL_PROGRAM.push_back(it);
    }
};


// compare the used memory to the installed amount
bool check_file_fits_memory(Converted_Assembly_Code &Machine_Code_File,PIC18F_FULL_IS &Instruction_Set);

// add #included files to main .asm
void appendIncludedFiles(Converted_Assembly_Code &AssemblyCode,PIC18F_FULL_IS &Instruction_Set);

// remove any data left over from previous assembling events
void cleanupPreviousWork();

// make sure the read-in file is usable
bool checkValidityOfFile(Converted_Assembly_Code AssemblyCode,const char *inputfiledir,PIC18F_FULL_IS &Instruction_Set);

// copy 'inputfiledir' into memory
// remove comments and do some preprocessing of macros and directives
Converted_Assembly_Code Copy_Over_Binary_File(const char *inputfiledir,PIC18F_FULL_IS &Instruction_Set);
#endif /* AssemblerFilePreprocessing_hpp */
