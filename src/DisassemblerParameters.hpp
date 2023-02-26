//
//  DisassemblerParameters.hpp
//  
//
//  Created by Greg Norman on 2/2/2023.
//

#ifndef DisassemblerParameters_hpp
#define DisassemblerParameters_hpp

#include <iostream>
#include <stdint.h>

void find_opcode_parameters(uint16_t MachineCode_byte,
                            uint8_t &b_or_d,
                            uint8_t &Access,
                            uint16_t &file,
                            uint8_t b_or_d_mask,
                            uint8_t access_mask,
                            uint8_t file_mask,
                            uint8_t b_or_d_bitshift,
                            char &Access_or_bank_RAM);           // outputs as either 'A' or 'B'

uint16_t check_n(int16_t n,
                 uint16_t bitmask,
                 uint16_t negative_sign_mask,
                 uint32_t program_counter);

#endif /* DisassemblerParameters_hpp */
