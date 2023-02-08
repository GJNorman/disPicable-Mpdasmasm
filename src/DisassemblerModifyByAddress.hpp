//
//  DisassemblerModifyByAddress.hpp
//
//
//  Created by Greg Norman on 6/2/2023.
//

#ifndef DisassemblerModifyByAddress_hpp
#define DisassemblerModifyByAddress_hpp

#include <iostream>
#include "AssemblerFilePreprocessing.hpp"
#include <vector>
#include <string>
#include <stdint.h>
#include "DisassemblerComments.hpp"
void modifyBasedOnAddress(Converted_Assembly_Code &OutputAssemblyCode, std::vector<uint64_t> AddressVector,std::vector<std::string> DataToConcat, bool addOnNewLine, bool prepend);
#endif /* DisassemblerModifyByAddress_hpp */
