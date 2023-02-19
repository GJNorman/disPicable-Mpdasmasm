//
//  DisassemblerRamTracker.cpp
//  
//
//  Created by Greg Norman on 11/2/2023.
//

#include "DisassemblerRamTracker.hpp"

void RAMTracker::initRamTracker(size_t NumberOfRamBanks)
{
    this->Contents.resize(NumberOfRamBanks);
    
    for(uint8_t index=0; index< NumberOfRamBanks; index++)
    {
        this->Contents[index].resize(256);
    }
}
void RAMTracker::setRam(uint16_t address, uint8_t value)
{
    this->Contents[(address>>8)&0xff][address&0xff] = value;
}
void RAMTracker::setRam(uint8_t address, uint8_t bank, uint8_t value)
{
    this->Contents[bank][address] = value;
}

uint8_t RAMTracker::getRam(uint16_t address)
{
    return this->Contents[(address>>8)&0xff][address&0xff];
}
uint8_t RAMTracker::getRam(uint8_t address, uint8_t bank)
{
    return this->Contents[bank][address];
}
