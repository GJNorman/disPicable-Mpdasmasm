//
//  DisassemblerRamTracker.cpp
//  annoyed
//
//  Created by Greg Norman on 11/2/2023.
//

#include "DisassemblerRamTracker.hpp"

std::vector<std::vector<uint8_t>> RAM_DATA;

void initRamTracker(size_t NumberOfRamBanks)
{
    RAM_DATA.resize(NumberOfRamBanks);
    
    for(size_t index=0; index<NumberOfRamBanks;index++)
    {
        RAM_DATA[index].resize(255);
    }
}
void setRam(uint16_t address, uint8_t value)
{
    RAM_DATA[(address>>8)&0xff][address&0xff] = value;
}
void setRam(uint8_t address, uint8_t bank, uint8_t value)
{
    RAM_DATA[bank][address] = value;
}

uint8_t getRam(uint16_t address)
{
    return RAM_DATA[(address>>8)&0xff][address&0xff];
}
uint8_t getRam(uint8_t address, uint8_t bank)
{
    return RAM_DATA[bank][address];
}
