//
//  AssemblerFilePreprocessing.hpp
//  annoyed
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

bool check_file_fits_memory(Converted_Assembly_Code &Machine_Code_File,PIC18F_FULL_IS &Instruction_Set);
std::string skip_inline_comments(std::string line_from_file,std::ifstream &input_file,char seperating_character, const char *CommentCharacter);

void process_binary_file_line(std::string &line,std::string &next_line,Converted_Assembly_Code &Machine_Code_File,PIC18F_FULL_IS &Instruction_Set, uint32_t &File_Position_Counter, bool &borrowed_byte_from_next_line,std::ifstream &input_file);

/*
  line is valid if it has any relevant ascii characters present
 */
bool check_assembly_file_line_is_valid(std::string line,Converted_Assembly_Code &Machine_Code_File,PIC18F_FULL_IS &Instruction_Set,uint32_t &File_Position_Counter);

void Convert_Databyte_into_hex_data(std::string line,std::string next_line,Converted_Assembly_Code &Machine_Code_File , uint32_t &File_Position_Counter,bool &borrowed_byte_from_next_line,PIC18F_FULL_IS &Instruction_Set);

void process_standard_line_from_binary(Converted_Assembly_Code &Machine_Code_File, uint32_t &File_Position_Counter, std::string line);

/*
    store user defined bytes 
 */
void find_define_byte_tags(Converted_Assembly_Code &Machine_Code_File,std::string line, const uint32_t File_Position_Counter,PIC18F_FULL_IS &Instruction_Set,uint8_t address_offset);



Converted_Assembly_Code Copy_Over_Binary_File(const char *inputfiledir,PIC18F_FULL_IS &Instruction_Set);
#endif /* AssemblerFilePreprocessing_hpp */
