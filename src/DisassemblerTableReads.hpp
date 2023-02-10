//
//  DisassemblerTableReads.hpp
//  
//
//  Created by Greg Norman on 2/2/2023.
//

#ifndef DisassemblerTableReads_hpp
#define DisassemblerTableReads_hpp

#include <iostream>
#include "PIC18_IS.h"
#include "AssemblerFilePreprocessing.hpp"
#include "DisassemblerModifyByAddress.hpp"  // for adding ascii data at the address pointed to by the TBLPTR


void markAllAsciiData(Converted_Assembly_Code &OutputAssemblyCode);

// add a comment indicating the data stored at the address targeted by a tblrd
void markTableReads(Converted_Assembly_Code &OutputAssemblyCode,PIC18F_FULL_IS &Instruction_Set);

// keep track of the data laoded into the various table pointer registers
void trackTableReads(char *&command_for_prompt, Converted_Assembly_Code &OutputAssemblyCode,uint16_t n );

// save the values loaded into table pointer registers
void setTablePointerValue(uint8_t &value, uint16_t &valueToSet, std::string &Assembly_Instruction);

// class to store a table pointer address, which is stored in (upto) 3 registers
// there is a overloaded ++ and -- operator to deal with INCF and DECF commands
class TABLE_READ_ADDR_t{
public:
    uint16_t HIGH;
    uint16_t LOW;
    uint16_t UPPER;
    uint8_t currentSettingType;
    // a direct type is set using
    // an instruction like movlw
    // an input argument is set using MOVFF PLUSx TBLPTRy
    enum TableReadTypes{
        direct = 1,
        InputArgument = 2,
    };
    TABLE_READ_ADDR_t &operator++(int)
    {
        this->LOW++;
        if(this->LOW > 0xff)
        {
            this->LOW = 0;
            this->HIGH++;
            if(this->HIGH > 0xff)
            {
                this->HIGH = 0;
            }
        }
        
        return *this;
    }
    TABLE_READ_ADDR_t &operator--(int)
    {
       
        if(this->LOW > 0)
        {
            this->LOW--;
        }
        else
        {
            this->LOW = 0;
            
            if(this->HIGH > 0)
            {
                this->HIGH--;
            }
            else
            {
                this->HIGH =0;
            }
        }
        
        return *this;
    }
    void reset()
    {
        this->LOW = 0;
        this->HIGH = 0;
        this->UPPER = 0;
    }
};
#endif /* DisassemblerTableReads_hpp */
