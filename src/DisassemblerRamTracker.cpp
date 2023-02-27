//
//  DisassemblerRamTracker.cpp
//  
//
//  Created by Greg Norman on 11/2/2023.
//

#include "DisassemblerRamTracker.hpp"


//static RAMTracker PIC_RAM_CONTENTS;

void RAMTracker::incrementRamBanksTotal()
{
    NumberOfRamBanks++;
}
void RAMTracker::initRamTracker()
{
    // user RAM indicated here
    std::cout << "There are " << NumberOfRamBanks <<" RAM banks, with " << NumberOfRamBanks*256<< " total bytes\n";
    
    uint16_t SFR_BANK = getSFRAccessBankOffset();

    std::cout << "SFRs stored in the access bank are offset by 0x" << std::hex << SFR_BANK << "\n";
    // SFR's are stored elsewhere
    NumberOfRamBanks=getHighestSFRBank();
    
    std::cout << "And the maximum address bank for an SFR is: 0x" << std::hex<<NumberOfRamBanks << "00\n";
    Contents.resize(NumberOfRamBanks+1);
    
    for(uint8_t index=0; index< NumberOfRamBanks+1; index++)
    {
        Contents[index].resize(256);
    }
    
    
    
}
/*
void RAMTracker::setRam(uint16_t address, uint8_t value)
{
    Contents[(address>>8)&0xff][address&0xff] = value;
}
void RAMTracker::setRam(uint8_t address, uint8_t bank, uint8_t value)
{
    Contents[bank][address] = value;
}*/
RAM_Follower_t *RAMTracker::getRam(uint16_t address)
{
    return &Contents[(address>>8)&0xff][address&0xff];
}
RAM_Follower_t *RAMTracker::getRam(uint8_t address, uint8_t bank)
{
    return &Contents[bank][address];
}
WREG_Follower_t *RAMTracker::getWREG()
{
    return &Shadow_WREG;
}
uint16_t getCurrentCommandFILE(PIC18F_FULL_IS &Instruction_Set, char *RAM_Address, size_t RAM_Address_Size)
{
    uint16_t address = 0;
    
    if(Instruction_Set.currentBank !=  RAMTracker::accessBank::ACCESS_BANK)
    {
        address = Instruction_Set.currentBank;
        address<<=8;
    }
    address+= Instruction_Set.currentFile;
    
    if(RAM_Address!=NULL)
        snprintf(RAM_Address, sizeof(RAM_Address), "0x%.3X", address);
    
    return address;
}
void RAMTracker::trackRegFromFDACommand(char *Instruction,PIC18F_FULL_IS &Instruction_Set, RAM_Follower_t *Output)
{
    // RAM Address will hold the literal value of the register address, not the 'equ'
    char RAM_Address[10];
    uint16_t address = getCurrentCommandFILE(Instruction_Set,RAM_Address,sizeof(RAM_Address));

    // get the value of this file
    RAM_Follower_t *CurrentFile = getRam(address);
    
    Output->REG.address = address;
    Output->type = FunctionArgumentTypes::RAM;
    
    if(Instruction_Set.currentCommand == PIC18F_IS::ADDWF)      // Add Wreg and F
    {
        Output->value = CurrentFile->value + Shadow_WREG.value;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::ANDWF)      // AND Wreg and F
    {
        Output->value = CurrentFile->value & Shadow_WREG.value;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::COMF)      // COMPLEMENT FILE
    {
        Output->value = ~CurrentFile->value + 1;
    }
    else if((Instruction_Set.currentCommand == PIC18F_IS::DECF) ||      // Decrement File
            (Instruction_Set.currentCommand == PIC18F_IS::DECFSZ)||     // Decrement File Skip if zero
            (Instruction_Set.currentCommand == PIC18F_IS::DCFSNZ))      // Decrement File Skip if not zero
    {
        Output->value = --CurrentFile->value;
    }
    else if((Instruction_Set.currentCommand == PIC18F_IS::INCF)||       // increment File
            (Instruction_Set.currentCommand == PIC18F_IS::INCFSZ)||     // increment file, skip is zero
            (Instruction_Set.currentCommand == PIC18F_IS::INFSNZ))      // increment file, skip if not zero
    {
        Output->value = ++CurrentFile->value;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::IORWF)      // Inclusive OR WREG with File
    {
        Output->value = CurrentFile->value | Shadow_WREG.value;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::MOVF)        // move File (optionally to WREG)
    {
        std::string REGISTER = getMostRecentEQU();

        if(REGISTER.find("0x") == std::string::npos)
        {
            Output->REG.Reg = REGISTER;
        }
        else
        {
            Output->REG.Reg = RAM_Address;
        }
        Output->value = CurrentFile->value;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::RLCF)      // Rotate left though carry
    {
        uint8_t carryBit = 0;
        if (CurrentFile->value & 0x80)
        {
            carryBit = 1;
        }
        Output->value = (CurrentFile->value<<1) + carryBit;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::RLNCF)      // Rotate left with no carry
    {
        Output->value = CurrentFile->value<<1;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::RRCF)      // Rotate right though carry
    {
        uint8_t carryBit = 0;
        if (CurrentFile->value & 0x01)
        {
            carryBit = 1;
        }
        Output->value =  (CurrentFile->value>>1) + carryBit;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::RRNCF)      // Rotate right with no carry
    {
        Output->value = CurrentFile->value>>1;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::SUBFWB)      // subtract file from wreg with borrow
    {
        // TODO - implement STATUS bits
        Output->value =  Shadow_WREG.value - CurrentFile->value;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::SUBWF)      // subtract wreg from file
    {
        Output->value =  CurrentFile->value - Shadow_WREG.value ;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::SUBWFB)      // subtract wreg from file with borrow
    {
        // TODO - implement STATUS bits
        Output->value =  CurrentFile->value - Shadow_WREG.value ;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::SWAPF)      // swap upper and lower nibbles
    {
        uint8_t lower_nibble = CurrentFile->value & 0xf;
        uint8_t upper_nibble =  CurrentFile->value &0xf0;
        
        lower_nibble<<=4;
        
        upper_nibble>>=4;
        Output->value =  lower_nibble|upper_nibble;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::XORWF)         // XOR FILE with working register
    {
        Output->value =  CurrentFile->value ^ Shadow_WREG.value ;
    }
    
    // set status bits
    memset(&STATUS, 0, sizeof(STATUS));
    
    if(Output->value == 0)
    {
        STATUS.ZERO = 1;
    }
    if(Output->value&0x80)
    {
        STATUS.NEGATIVE = 0;
    }
    
    //printf("%s\t\t\t%.4X    %.2X \n",Instruction, address,CurrentFile->value);
}
void RAMTracker::trackRegFromLITERALCommand(char *Instruction,PIC18F_FULL_IS &Instruction_Set)
{
    // destination is always WREG
    
    // file bits overlap with literal bits
    uint8_t literalValue = Instruction_Set.currentFile;
    if(Instruction_Set.currentCommand != PIC18F_IS::MULLW)  // will not change contents of WREG
        Shadow_WREG.type = literalValue;
    
    Shadow_WREG.REG.address = 0;
    
    if(Instruction_Set.currentCommand == PIC18F_IS::ANDLW)         // And literal with working register
    {
        Shadow_WREG.value &= literalValue;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::ADDLW)         // ADD literal to working register
    {
        Shadow_WREG.value += literalValue;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::IORLW)         // OR literal with working register
    {
        Shadow_WREG.value |= literalValue;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::MOVLW)         // Move literal to working register
    {
        size_t point1 = strstr(Instruction,"MOVLW")-Instruction + strlen("MOVLW") + 1;
        
        Shadow_WREG.value = strtol(&Instruction[point1], NULL, 16);
        
        Shadow_WREG.type = FunctionArgumentTypes::literal;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::MULLW)         // Multiply literal with working register
    {
        //uint16_t PROD = Shadow_WREG.value * literalValue;
        
        // TODO - output is stored in PRODH:PRODL
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::RETLW)         // Return literal to working register
    {
        Shadow_WREG.value = literalValue;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::SUBLW)         // subtract working register from literal
    {
        Shadow_WREG.value -= literalValue;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::XORLW)         // XOR literal with working register
    {
        Shadow_WREG.value ^= literalValue;
    }
 //   printf("%s\t\t\t%.4X    %.2X \n",Instruction, 0xffff,Shadow_WREG.value);
}
void RAMTracker::trackRegFromFACommand(char *Instruction,PIC18F_FULL_IS &Instruction_Set)
{
    // RAM Address will hold the literal value of the register address, not the 'equ'
    char RAM_Address[10];
    uint16_t address = getCurrentCommandFILE(Instruction_Set,RAM_Address,sizeof(RAM_Address));

    // get the value of this file
    RAM_Follower_t *CurrentFile = getRam(address);
    
   
    if(Instruction_Set.currentCommand == PIC18F_IS::CLRF)
    {
        CurrentFile->type = literal;
        CurrentFile->value = 0;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::CPFSEQ)
    {
        
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::CPFSGT)
    {
        
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::CPFSLT)
    {
        
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::MOVWF)
    {
        CurrentFile->type = literal;
        CurrentFile->value = Shadow_WREG.value;
        
        
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::MULWF)
    {
       // uint16_t PROD = CurrentFile->value * Shadow_WREG.value;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::NEGF)
    {
        CurrentFile->type = literal;
        CurrentFile->value = ~CurrentFile->value;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::SETF)
    {
        CurrentFile->type = literal;
        CurrentFile->value = 0xff;
    }
    else if(Instruction_Set.currentCommand == PIC18F_IS::TSTFSZ)
    {
        
    }
    
  //  printf("%s\t\t\t%.4X    %.2X \n",Instruction, address,CurrentFile->value);

}
void RAMTracker::trackWREG(char *Instruction,PIC18F_FULL_IS &Instruction_Set)
{
    // track changes made to the working register
        
    if(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[Instruction_Set.currentCommand] == PIC18F_FDA_CASE)
    {
        if(Instruction_Set.destinationBit == PIC18F_FULL_IS::destinationBits::WREG)
        {
            trackRegFromFDACommand(Instruction, Instruction_Set,&Shadow_WREG);
        }
        else
        {
            // get the current file
            uint16_t address = getCurrentCommandFILE(Instruction_Set,NULL,0);
            
            // get the value of this file
            RAM_Follower_t *CurrentFile = getRam(address);
            
            trackRegFromFDACommand(Instruction, Instruction_Set,CurrentFile);
        }
    }
    else if(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[Instruction_Set.currentCommand] == PIC18F_FA_CASE)
    {
        trackRegFromFACommand(Instruction, Instruction_Set);
    }
    else if(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[Instruction_Set.currentCommand] == PIC18F_LIT_CASE)
    {
        trackRegFromLITERALCommand(Instruction, Instruction_Set);
    }
       

}
uint8_t RAMTracker::getCurrentBank()
{
    return currentBank;
}
void RAMTracker::trackRamBank(char *Instruction,PIC18F_FULL_IS &Instruction_Set)
{
    if(Instruction_Set.currentCommand == PIC18F_IS::MOVLB)
    {
        currentBank = Instruction_Set.currentBSR;
    }
}
void RAMTracker::trackRAM(char *Instruction,PIC18F_FULL_IS &Instruction_Set)
{
    trackRamBank(Instruction,Instruction_Set);

    trackWREG(Instruction,Instruction_Set);

}

