//
//  DisassemblerTableReads.cpp
//  
//
//  Created by Greg Norman on 2/2/2023.
//

#include "DisassemblerTableReads.hpp"

std::vector<std::string> TableReadData;
std::vector<uint64_t> TableReadAddress;

void markAllAsciiData(Converted_Assembly_Code &OutputAssemblyCode)
{
    modifyBasedOnAddress(OutputAssemblyCode, TableReadAddress,TableReadData, false, false);
}
void markAsciiData(const char *formatBuffer,uint32_t Address)
{
    TableReadData.push_back(formatBuffer);
    TableReadAddress.push_back(Address);
}
// after each TBLRD command add a comment indicating the first character being read
void markTableReads(Converted_Assembly_Code &OutputAssemblyCode,PIC18F_FULL_IS &Instruction_Set)
{
    char formatBuffer[100] ;        // holds the ascii character at the location of the TBLRD command
   
    // find ascii data used in table reads
    for(size_t  it = 0 ; it< OutputAssemblyCode.TBLRD_Target_Addresses.size(); it++)
    {
        if(OutputAssemblyCode.TBLRD_Target_Addresses[it] < OutputAssemblyCode.OPCODES.size())
        {
            if(OutputAssemblyCode.TableReadArgumentType[it] == TABLE_READ_ADDR_t::TableReadTypes::direct)
            {
                char temp[10];
                
                removeEscapeCharacter(OutputAssemblyCode.OPCODES[OutputAssemblyCode.TBLRD_Target_Addresses[it]],temp,sizeof(temp));
                
                snprintf(formatBuffer, sizeof(formatBuffer), "address = %.4X, data = '%s' ",OutputAssemblyCode.TBLRD_Target_Addresses[it] ,temp);
                
                if(OutputAssemblyCode.TBLRD_Target_Addresses[it]!=0)
                {
                    markAsciiData(formatBuffer,OutputAssemblyCode.TBLRD_Target_Addresses[it]>>1);
                }
            }
            else
            {
                snprintf(formatBuffer, sizeof(formatBuffer), "Passed as argument?");
            }
        }
        else    // most likely passed as a argument to a function
        {
            snprintf(formatBuffer, sizeof(formatBuffer), "Passed as argument? or from external flash");
        }
        
        // add a comment at the address of the table read command
        OutputAssemblyCode.Comments.push_back(formatBuffer);
        OutputAssemblyCode.CommentAddress.push_back(OutputAssemblyCode.AddressOfTableReadCommand[it]);
        
    }
}


void setTablePointerValue(uint8_t &value, uint16_t &valueToSet, std::string &Assembly_Instruction)
{
    if(Assembly_Instruction.find("SETF")    !=  std::string::npos)          // SET FILE
    {
        valueToSet=0xff;
    }
    else if(Assembly_Instruction.find("CLRF")    !=  std::string::npos)     // CLEAR FILE
    {
        valueToSet = 0;
    }
    else if(Assembly_Instruction.find("INCF")    !=  std::string::npos)     // INCREMENT FILE
    {
        valueToSet++;
    }
    else if(Assembly_Instruction.find("DECF")    !=  std::string::npos)     // DECREMENT FILE
    {
        valueToSet--;
    }
    else if(Assembly_Instruction.find("MOVWF")    !=  std::string::npos)    // MOVE WREG TO FILE
    {
        valueToSet = value;
    }
    else                                                                    // Other instructions can't be easily tracked
    {
        valueToSet = 0;
    }
    
    value = 0;
}

void trackTableReads(char *command_for_prompt, Converted_Assembly_Code &OutputAssemblyCode,uint16_t n,PIC18F_FULL_IS &Instruction_Set )
{
    static uint8_t previousMovlwValue = 0;
    static TABLE_READ_ADDR_t lastTBLRDAddr = {};
    
    std::string Assembly_Instruction = command_for_prompt;
    
    // look for instructions that modify the table pointer
    if(Assembly_Instruction.find("TBLPTR")!=std::string::npos)
    {
        // make sure it is not a MOVFF command, this makes the address untrackable
        if(Assembly_Instruction.find("MOVFF")==std::string::npos)
        {
            if(Assembly_Instruction.find("TBLPTRH")!=std::string::npos)
            {
                setTablePointerValue(previousMovlwValue, lastTBLRDAddr.HIGH,Assembly_Instruction);
            }
            else if(Assembly_Instruction.find("TBLPTRU")!=std::string::npos)
            {
                setTablePointerValue(previousMovlwValue, lastTBLRDAddr.UPPER,Assembly_Instruction);
            }
            else if (Assembly_Instruction.find("TBLPTR")!=std::string::npos)
            {
                setTablePointerValue(previousMovlwValue, lastTBLRDAddr.LOW,Assembly_Instruction);
            }
            
        }
        else
        {
            if(Assembly_Instruction.find("PLUS") != std::string::npos)
            {
                lastTBLRDAddr.currentSettingType = TABLE_READ_ADDR_t::TableReadTypes::InputArgument;
                // the instruction is being passed as an input argument to a function
            }
            
            lastTBLRDAddr.reset();
            previousMovlwValue = 0 ;
        }
    }
    // TBLRD*, TBLRD*+,TBLRD+*, TBLLRD*-
    else if(Assembly_Instruction.find("TBLRD")!=std::string::npos)
    {
        // table has been read
        uint32_t address = lastTBLRDAddr.LOW + (lastTBLRDAddr.HIGH<<8) + (lastTBLRDAddr.UPPER<<16);

        //if(address!=0)  // invalid address
        {
            // save the address from the table pointer
            OutputAssemblyCode.TBLRD_Target_Addresses.push_back(address);
            
            // record the current program counter, so that we can add a comment with the indicated ascii data later
            OutputAssemblyCode.AddressOfTableReadCommand.push_back((uint32_t)(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size()));

            OutputAssemblyCode.TableReadArgumentType.push_back(lastTBLRDAddr.currentSettingType);
        }
        
        if(Assembly_Instruction.find("TBLRD*+")!=std::string::npos)
        {
            lastTBLRDAddr++;
        }
        else if (Assembly_Instruction.find("TBLRD*-")!=std::string::npos)
        {
            lastTBLRDAddr--;
        }
     }
     if(Assembly_Instruction.find("MOVLW")!=std::string::npos)
     {
         // movlw command; we will grab the value and wait for TBLPTR registers to be used
         // this is so we can determine if ascii data is being read from FLASH memory

         size_t point2 = Assembly_Instruction.find("MOVLW") + strlen("MOVLW") + 1;
         
         previousMovlwValue = strtol(Assembly_Instruction.substr(point2, Assembly_Instruction.size() - point2).c_str(), NULL, 16);
         
         lastTBLRDAddr.currentSettingType = TABLE_READ_ADDR_t::TableReadTypes::direct;
         
     }
     else
     {
         previousMovlwValue = 0;
     }
}
