//
//  PIC10_IS.h
//  
//
//  Created by Greg Norman on 5/2/2023.
//

#ifndef PIC10_IS_h
#define PIC10_IS_h

#include <vector>
#include <string>
#include <stdint.h>
enum {
    PIC10F_FA_CASE,
    PIC10F_F_CASE,
    PIC10F_DW_CASE,
    PIC10F_DW_CASE2,
    PIC10F_BIT_CASE,
    PIC10F_CTRL_CASE,
    PIC10F_CTRL_CASE2,
    PIC10F_CALL_CASE,
    PIC10F_NO_ARGS,
    PIC10F_NOP_CASE,
    PIC10F_LIT_CASE,
    PIC10F_LIT_CASE2,
    PIC10F_RET_CASE,
    PIC10F_TBLRD_CASE  // table read is normally a NO_ARG, but the compiler will look at the address to be read and treat it as ascii
                            
};
class PIC10F_IS{

public:


    std::vector<std::string> PIC10F_MNEMONICS;
    std::vector<uint16_t> PIC10F_OPCODES_LOWER_VAL;
    std::vector<uint16_t> PIC10F_OPCODES_UPPER_VAL;
    std::vector<unsigned char> PIC10F_OPCODES_DECODE_CASE;
    std::vector<std::string> PIC10F_DESCRIPTIONS ;
    
    void push(std::string MNEM, uint16_t OPLOW, uint16_t OPHIGH, unsigned char CASE, std::string Description)
    {
        this->PIC10F_MNEMONICS.push_back(MNEM);
        this->PIC10F_OPCODES_LOWER_VAL.push_back(OPLOW);
    }
    void init()
    {
        this->push("ADDWF", 0b000111000000,b000111111111,PIC10_FA_CASE,"ADDWF f,d ; Add WREG and f");
        this->push("ANDWF", 0b000101000000,b000101111111,PIC10_FA_CASE,"");
        this->push("CLRF",  0b000001100000,b000001111111,PIC10_F_CASE,"");
        /*
         ={
            "ADDWF",    "ANDWF",    "CLRF",     "CLRW",     "COMF",                 ///1
            "DECF",     "DECFSZ",   "INCF",     "INCFSZ",   "IORWF",
            "MOVF",     "MOVWF",     "RLF",     "RRF",      "SUBWF",
            "SWAPF",
        };
         */
    }

};
#endif /* PIC10_IS_h */
