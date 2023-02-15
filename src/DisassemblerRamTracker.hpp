//
//  DisassemblerRamTracker.hpp
//  annoyed
//
//  Created by Greg Norman on 11/2/2023.
//

#ifndef DisassemblerRamTracker_hpp
#define DisassemblerRamTracker_hpp

#include <iostream>
#include <vector>
#include <stdint.h>

// allocate memory
void initRamTracker(size_t NumberOfRamBanks);
void setRam(uint8_t address, uint8_t bank, uint8_t value);
void setRam(uint16_t address, uint8_t value);

uint8_t getRam(uint8_t address, uint8_t bank);
uint8_t getRam(uint16_t address);
#endif /* DisassemblerRamTracker_hpp */
