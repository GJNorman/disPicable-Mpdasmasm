//
//  DisassemblerRamTracker.hpp
//  
//
//  Created by Greg Norman on 11/2/2023.
//

#ifndef DisassemblerRamTracker_hpp
#define DisassemblerRamTracker_hpp

#include <iostream>
#include <vector>
#include <stdint.h>
#include "PIC18_IS.h"

typedef struct{
    std::string Reg;                // for holding special function registers (ADCON1 etc.)
    uint32_t address;               // and the function address
    std::string literalRegister;    // for literal ram registers (0x107 etc.)
}RegistersTracker_t;


typedef struct{
    uint8_t value;          // literal value
    uint8_t bank;           // ram bank of address
    uint8_t type;           // literal or register
    RegistersTracker_t REG; // for registers moved to WREG
}WREG_Follower_t;

enum FuncionArgumentTypes
{
    literal = 0,
    RAM = 1
};

class RAMTracker
{
    inline static WREG_Follower_t Shadow_WREG;
    inline static size_t NumberOfRamBanks;
    inline static std::vector<std::vector<uint8_t>> Contents;
    inline static uint8_t currentBank;
    
    enum accessBank{
        ACCESS_BANK = 255
    };
    static void trackWREG(char *Instruction,PIC18F_FULL_IS &Instruction_Set);
    static void trackRamBank(char *Instruction,PIC18F_FULL_IS &Instruction_Set);
public:
    // allocate space for each ram bank
    static void initRamTracker();


    static void trackRAM(char *Instruction,PIC18F_FULL_IS &Instruction_Set);
    
    static WREG_Follower_t *getWREG();
    static void setRam(uint8_t address, uint8_t bank, uint8_t value);
    static void setRam(uint16_t address, uint8_t value);
    
    static uint8_t getRam(uint8_t address, uint8_t bank);
    static uint8_t getRam(uint16_t address);
    static uint8_t getCurrentBank();
    // to be handled during file preprocessing only
    static void incrementRamBanksTotal();
    
};


#endif /* DisassemblerRamTracker_hpp */
