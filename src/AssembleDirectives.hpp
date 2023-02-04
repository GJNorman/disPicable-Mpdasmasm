//
//  AssembleDirectives.hpp
//
//
//  Created by Greg Norman on 28/1/2023.
//

#ifndef AssembleDirectives_hpp
#define AssembleDirectives_hpp

#include <iostream>
#include "AssemblerMachineCodeWriter.hpp"
#include "BasicFileEditing.hpp"
#include "AssemblerAddressAndChecksum.hpp"


// EEPROM, config bits etc.
void assemble_non_program_data(uint16_t REG_VALUE,
                               const char *Assembly_Instruction,
                               uint32_t &address,
                               uint16_t &address_upper_16bits,
                               uint16_t &checksum,
                               bool &check_sum_required,
                               uint32_t FLASH_size,
                               bool IsEEPROM);

// process eeprom directive
void assemble_EEPROM_data(uint16_t EEPROM_START_ADD_U16,
                          const char *Assembly_Instruction,
                          uint32_t &address,
                          uint16_t &address_upper_16bits,
                          uint16_t &checksum,
                          bool &check_sum_required,
                          uint32_t FLASH_size);


// process ORG directive
void processORG(uint32_t &address,
                std::string Assembly_Instruction,
                uint32_t &START_ADDRESS,
                uint16_t &check_sum);



#endif /* AssembleDirectives_hpp */
