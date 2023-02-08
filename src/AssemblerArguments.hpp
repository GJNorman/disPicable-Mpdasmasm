//
//  AssemblerArguments.hpp
//  
//
//  Created by Greg Norman on 28/1/2023.
//

#ifndef AssemblerArguments_hpp
#define AssemblerArguments_hpp

#include <iostream>
#include <vector>
#include <string>
#include <stdint.h>
#include "PIC18_IS.h"
#include "BasicFileEditing.hpp"
//#include "AssembleDisassembleCommonFunctions.hpp"   // equis
#include "EQUs.hpp"

//
// for decoding the base number system
typedef struct
{
    uint8_t base;
    std::string nomenclature;
    bool leading;               // leading      i.e. 0b1010101
                                // or trailing  i.e. 37h
}NumberSystemDecoder_t;

/*
    For commands that utilise an 'f' parameter
 
        ADDWF f,d,a
        BCF f,b,a
 */
uint32_t find_f_value_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address);


uint8_t find_d_value_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, bool &optionalArgumentPresent);
uint8_t find_a_value_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, size_t a_offset);

// n is used in branching instructions
uint32_t check_n_for_assembly(int32_t f, uint32_t program_counter);
uint8_t find_LFSR_f_value(const char *Assembly_Instruction);
uint16_t check_if_FSR_or_RAW_REG(const char *REGISTER,PIC18F_FULL_IS & Instruction_Set);
uint32_t convert_FSR_for_assembler(const char *SFR_REG,PIC18F_FULL_IS & Instruction_Set, uint32_t offset);

//where number system has the format "0x" / "0d" / "0b"
//                                    "h'"   "d'"  "b'"
//                        or trailing  h      d     b
char *check_for_f_based_on_number_system(const char *Assembly_Instruction, NumberSystemDecoder_t decoder, uint8_t &f_base);
#endif /* AssemblerArguments_hpp */
