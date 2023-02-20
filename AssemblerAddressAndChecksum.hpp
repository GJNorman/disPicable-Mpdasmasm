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


typedef struct{
    uint32_t START_ADDRESS; // ORG address
    uint32_t Max_Address;   // Flash mem size
    uint16_t upper_16_bits;
    uint32_t lower_32_bits;
    uint16_t check_sum;
    bool check_sum_required;
    
}Address_And_Checksum_t;
void add_to_checksum(uint16_t OPCODE, uint16_t &checksum);

bool increment_assembler_address(Address_And_Checksum_t &Address,
                                 uint32_t FLASH_size);

// header will specifier address and number of databytes
// we will always write a complete line, so num_bytes = 0x10
void formatHexHeader(char *header,Address_And_Checksum_t &Address);


// add padding to finish off a line
// this is done with NOP instructions
void padFile(Address_And_Checksum_t &Address);
#endif /* AssemblerAddressAndChecksum_hpp */
