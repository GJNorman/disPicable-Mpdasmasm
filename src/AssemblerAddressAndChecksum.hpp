//
//  AssemblerAddressAndChecksum.hpp
//  
//
//  Created by Greg Norman on 28/1/2023.
//

#ifndef AssemblerAddressAndChecksum_hpp
#define AssemblerAddressAndChecksum_hpp

#include <iostream>

#include <stdint.h>
#include "AssemblerMachineCodeWriter.hpp"
void add_to_checksum(uint16_t OPCODE, uint16_t &checksum);

bool increment_assembler_address(uint16_t &upper_16_bits,
                                 uint32_t &lower_32_bits,
                                 uint16_t &check_sum,
                                 bool &check_sum_required,
                                 uint32_t FLASH_size);

// header will specifier address and number of databytes
// we will always write a complete line, so num_bytes = 0x10
void formatHexHeader(char *header,bool &check_sum_required,uint16_t &check_sum, uint32_t &address);


// add padding to finish off a line
// this is done with NOP instructions
void padFile(uint32_t &address, uint16_t &check_sum);
#endif /* AssemblerAddressAndChecksum_hpp */
