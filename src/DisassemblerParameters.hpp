//
//  DisassemblerParameters.hpp
//  annoyed
//
//  Created by Greg Norman on 2/2/2023.
//

#ifndef DisassemblerParameters_hpp
#define DisassemblerParameters_hpp

#include <iostream>
#include <stdint.h>
void find_opcode_parameters(uint16_t MachineCode_byte,
                            uint8_t &prm1,
                            uint8_t &prm2,
                            uint16_t &prm3,
                            uint8_t prm1_operand,
                            uint8_t prm2_operand,
                            uint8_t prm3_operand,
                            uint8_t prm_bitshift,
                            char &Access_or_bank_RAM);


uint16_t check_n(int16_t n,
                 uint16_t bitmask,
                 uint16_t negative_sign_mask,
                 uint32_t program_counter);

#endif /* DisassemblerParameters_hpp */
