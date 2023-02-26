//
//  DisassemblerRamTracker.cpp
//  
//
//  Created by Greg Norman on 11/2/2023.
//

#include "DisassemblerRamTracker.hpp"


static RAMTracker PIC_RAM_CONTENTS;

void RAMTracker::incrementRamBanksTotal()
{
    NumberOfRamBanks++;
}
void RAMTracker::initRamTracker()
{
    std::cout << "There are " << NumberOfRamBanks <<" RAM banks, with " << NumberOfRamBanks*256<< " total bytes\n";
    Contents.resize(NumberOfRamBanks);
    
    for(uint8_t index=0; index< NumberOfRamBanks; index++)
    {
        Contents[index].resize(256);
    }
}
void RAMTracker::setRam(uint16_t address, uint8_t value)
{
    Contents[(address>>8)&0xff][address&0xff] = value;
}
void RAMTracker::setRam(uint8_t address, uint8_t bank, uint8_t value)
{
    Contents[bank][address] = value;
}
uint8_t RAMTracker::getRam(uint16_t address)
{
    return Contents[(address>>8)&0xff][address&0xff];
}
uint8_t RAMTracker::getRam(uint8_t address, uint8_t bank)
{
    return Contents[bank][address];
}
WREG_Follower_t *RAMTracker::getWREG()
{
    return &Shadow_WREG;
}
void RAMTracker::trackWREG(char *Instruction,PIC18F_FULL_IS &Instruction_Set)
{
    // track changes made to the working register
        
            // for F,d,a commands
            if(Instruction_Set.destinationBit == PIC18F_FULL_IS::destinationBits::WREG)
            {
                if(Instruction_Set.currentCommand == PIC18F_IS::MOVF)        // move File (optionally to WREG)
                {
                    size_t point1 = strstr(Instruction,"MOVF")-Instruction + strlen("MOVF") + 1;
                    
                    size_t point2 = strstr(&Instruction[point1], ",")-Instruction;
                    char temp[point2-point1+1];
                    strncpy(temp,&Instruction[point1],point2-point1);
                    temp[point2-point1] = 0;
                    
                    if(strstr(temp, "0x"))
                    {
                        Shadow_WREG.REG.Reg = temp;
                    }
                    // grab the entire address by referencing the BSR
                    else
                    {
                        uint32_t address = 0;
                        if(Instruction_Set.currentBank != ACCESS_BANK)
                        {
                            address = Instruction_Set.currentBank;
                            address<<=8;
                        }
                        address+= Instruction_Set.currentFile;
                        
                        char RAM_Address[10];
                        
                        snprintf(RAM_Address, sizeof(RAM_Address), "0x%.3X", address);
                        Shadow_WREG.REG.Reg = RAM_Address;  
                        
                    }
                    
                    Shadow_WREG.type = FuncionArgumentTypes::RAM;
                }
            }
        
        // destination is always WREG
            else if(Instruction_Set.currentCommand == PIC18F_IS::ANDLW)         // And literal with working register
            {
                
            }
            else if(Instruction_Set.currentCommand == PIC18F_IS::ADDLW)         // ADD literal to working register
            {
                
            }
            else if(Instruction_Set.currentCommand == PIC18F_IS::IORLW)         // OR literal with working register
            {
                
            }
            else if(Instruction_Set.currentCommand == PIC18F_IS::MOVLW)         // Move literal to working register
            {
                size_t point1 = strstr(Instruction,"MOVLW")-Instruction + strlen("MOVLW") + 1;
                
                Shadow_WREG.value = strtol(&Instruction[point1], NULL, 16);
                Shadow_WREG.type = FuncionArgumentTypes::literal;
            }
            else if(Instruction_Set.currentCommand == PIC18F_IS::MULLW)         // Multiply literal with working register
            {
                
            }
            else if(Instruction_Set.currentCommand == PIC18F_IS::RETLW)         // Return literal to working register
            {
                
            }
            else if(Instruction_Set.currentCommand == PIC18F_IS::SUBLW)         // subtract working register from literal
            {
                
            }
            else if(Instruction_Set.currentCommand == PIC18F_IS::XORLW)         // XOR literal with working register
            {
                
            }
    
    
        else if(strstr(Instruction,"WREG"))    // WREG can be treated as any other file
        {
            if(Instruction_Set.currentCommand == PIC18F_IS::CLRF)
            {
                Shadow_WREG.value = 0;
                Shadow_WREG.type = FuncionArgumentTypes::literal;
            }
            else if(Instruction_Set.currentCommand == PIC18F_IS::SETF)
            {
                Shadow_WREG.value = 0xff;
                Shadow_WREG.type = FuncionArgumentTypes::literal;
            }
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
    //   //::WREG, destinationBit
    // RAMTracker::accessBank::ACCESS_BANK   // currentBank
    trackWREG(Instruction,Instruction_Set);
    
    // values moved from WREG
    // movwf
}
