//
//  DisassemblerFunctionStacks.cpp
//  
//
//  Created by Greg Norman on 3/2/2023.
//

#include "DisassemblerFunctionStacks.hpp"

static std::vector<FunctionStack_t> inputArguments;
std::vector<RegistersTracker_t> FunctionRegisterList;
std::vector<ListOfFunctionCalls_t> ListOfFunctionCalls;
static int CurrentFunctionCounter = 0;


static bool firstCall = true;

// provide the 'function number' (determined by its position in the file)
// and recieve a list of Registers used in that function
std::string grabSFRList(int funcNum)
{
    if(funcNum<FunctionRegisterList.size())
    {
        return FunctionRegisterList[funcNum].Reg;
    }
    else
    {
        return "NONE";
    }
}
std::string grabLiteralRegisterList(int funcNum)
{
    if(funcNum<FunctionRegisterList.size())
    {
        return FunctionRegisterList[funcNum].literalRegister;
    }
    else
    {
        return "NONE";
    }
}
static void pushEQU(std::string EQU)
{
    // make sure we haven't already recorded this regiser
    // sometimes you can end up with hundreds in large functions
    std::string Temp = " "+ EQU + ",";
    
    // literal registers will use hex addresses
    if(EQU.find("0x")==std::string::npos)
    {
        // special function register
        if(FunctionRegisterList[CurrentFunctionCounter].Reg.find(Temp) == std::string::npos)
        {
            FunctionRegisterList[CurrentFunctionCounter].Reg += EQU + ", ";
        }
    }
    else
    {
        // literal registers
        if(FunctionRegisterList[CurrentFunctionCounter].literalRegister.find(Temp) == std::string::npos)
        {
            FunctionRegisterList[CurrentFunctionCounter].literalRegister += EQU + ", ";
        }
    }
}
static void setFunctionRegisterList(bool &newFunction, uint32_t address)
{
    std::string EQU = getMostRecentEQU();
    if(newFunction == true)
    {
        newFunction = false;
       
        RegistersTracker_t newTracker ;
        
        newTracker.Reg = "SFRs: ";
        newTracker.literalRegister = "RAM: ";
        newTracker.address = address;
        FunctionRegisterList.push_back(newTracker);
    }
    if(EQU.empty() == false)
    {
        pushEQU(EQU);
        std::string secondEQU = getSecondMostRecentEQU();
        
        if(secondEQU.empty() == false)
        {
            pushEQU(secondEQU);
        }
    }
}
static void isolateCallAddresses(char* Instruction, uint32_t call_address)
{
    size_t endOfInstruction = (strstr(Instruction,"CALL")-Instruction) + strlen("CALL");
    
    while(Instruction[endOfInstruction] == ' ')
    {
        endOfInstruction++;
    }
    
    uint32_t Function_address = (uint32_t) strtol(&Instruction[endOfInstruction],NULL,16);
    
    ListOfFunctionCalls_t newCall ;
    
    newCall.address_of_call = call_address;
    newCall.address_of_function = Function_address;
    
    ListOfFunctionCalls.push_back(newCall);
}
// monitor stack loading
// we expect these cases
//
// MOVFF 0x123, POSTINC
//
// CLRF POSTINC
//
// SETF POSTINC
//
// movf 0x123, W
// movwf POSTINC
//
// movlw 0x7
// movwf POSTINC
// The final two cases are values transferred from the Working Register, which is handled in 'trackWREG'

static void trackStackLoading(char*Instruction,PIC18F_FULL_IS &Instruction_Set)
{
    if(strstr(Instruction,"FSR") == NULL)   // MOVFF FSR2, POSTINC1  ; this would indicate a context save, not stack loading
    {
        FunctionStack_t newStack;
        
        WREG_Follower_t *Shadow_WREG = RAMTracker::getWREG();
        
        newStack.type = Shadow_WREG->type;
        
        if( Shadow_WREG->type == literal)
        {
            newStack.value = Shadow_WREG->value;
        }
        else
        {
            newStack.ArgumentName = Shadow_WREG->REG.Reg;
        }
        
        if(Instruction_Set.currentCommand == PIC18F_IS::PIC_COMMANDS::CLRF) // Clear File, File = 0
        {
            newStack.value = 0;
            newStack.type = literal;
        }
        else if(Instruction_Set.currentCommand == PIC18F_IS::PIC_COMMANDS::MOVFF)   // Move File to File, Fd = Fs
        {
            size_t point1 = strstr(Instruction,"MOVFF")-Instruction + strlen("MOVFF") + 1;
            
            size_t point2 = strstr(&Instruction[point1], ",")-Instruction;
            
            char temp[point2-point1+1];
            strncpy(temp,&Instruction[point1],point2-point1);
            temp[point2-point1] = 0;
            
            newStack.ArgumentName = temp;
            newStack.type = FuncionArgumentTypes::RAM;
        }
        else if(Instruction_Set.currentCommand == PIC18F_IS::PIC_COMMANDS::SETF)     // set file, file = ~0
        {
            newStack.value = 0xff;
            newStack.type = literal;
        }
        
        inputArguments.push_back(newStack);
    }
}
static void handleFunctionCall(char *Instruction, Converted_Assembly_Code &OutputAssemblyCode)
{
    uint32_t CurrentAddress = (uint32_t)OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size();
    isolateCallAddresses(Instruction,CurrentAddress);
    uint8_t addComma = 0;
    char COMMENT[100];
    
    snprintf(COMMENT,sizeof(COMMENT),"args={");
    
    for(auto it: inputArguments)
    {
        // avoid placing comma before first agrument
        if(addComma++ > 0)
        {
            snprintf(&COMMENT[strlen(COMMENT)], sizeof(COMMENT) - strlen(COMMENT), "%s", ",");
        }
        if(it.type == literal)
        {
            char temp[10] ;
            
            removeEscapeCharacter(it.value,temp,sizeof(temp));
                            
            snprintf(&COMMENT[strlen(COMMENT)], sizeof(COMMENT) - strlen(COMMENT), "0x%.2X '%s'",it.value,temp);
        }
        else
        {
            snprintf(&COMMENT[strlen(COMMENT)], sizeof(COMMENT) - strlen(COMMENT), "REG = '%s'",it.ArgumentName.c_str());
        }
    }
    snprintf(&COMMENT[strlen(COMMENT)], sizeof(COMMENT) - strlen(COMMENT), "%s", "}");
    
    OutputAssemblyCode.Comments.push_back(COMMENT);
    OutputAssemblyCode.CommentAddress.push_back((uint32_t)OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size());
    
    inputArguments.clear();
}
static void handleFunctionReturns(char *Instruction)
{
    inputArguments.clear();
    
    if(strstr(Instruction,"RET"))
    {
        firstCall = true;
        CurrentFunctionCounter++;
    }
}
// keep track of how software stacks are being loaded (FSRs)
void watchFunctionStacks(char* Instruction,Converted_Assembly_Code &OutputAssemblyCode,PIC18F_FULL_IS &Instruction_Set)
{
    setFunctionRegisterList(firstCall,(uint32_t)strtol(OutputAssemblyCode.Address[OutputAssemblyCode.Address.size()-1].c_str(),NULL,16));
        
    if(strstr(Instruction,"POSTINC"))
    {
        trackStackLoading(Instruction,Instruction_Set);
    }
    
    // postdecrement indicates (normally) that the software stack is being reset after a function is returned
    // "RET" is for RETURN, or RETFIE (end of a function)
    else if((strstr(Instruction,"POSTDEC"))||(strstr(Instruction,"RET")))
    {
        handleFunctionReturns(Instruction);
    }
    
    // CALL and RCALL are used to call a function - indicating that our stack is fully loaded
    else if(strstr(Instruction,"CALL"))
    {
        handleFunctionCall(Instruction, OutputAssemblyCode);
    }
    
    // PLUSW1/2/3 will tell us how input arguments loaded onto FSR's are being parsed from the stack
    else if (strstr(Instruction,"PLUSW"))
    {
        if(Instruction_Set.currentCommand != PIC18F_IS::PIC_COMMANDS::MOVFF)
        {
            // check last movlw
            
            
        }
    }
}


/*
 
indicate which registers are being used in function calls, at the point where the function is called
 */
void HighlightSFRs(Converted_Assembly_Code &OutputAssemblyCode)
{
    for (auto it : ListOfFunctionCalls)
    {
        for(auto reg_it: FunctionRegisterList)
        {
            if(it.address_of_function == reg_it.address)
            {
                // Special function registers
                if(reg_it.Reg.empty() == false)
                {
                    OutputAssemblyCode.Comments.push_back(reg_it.Reg);
                    OutputAssemblyCode.CommentAddress.push_back(it.address_of_call);
                }
                // literal registers
                if(reg_it.literalRegister.empty() == false)
                {
                    OutputAssemblyCode.Comments.push_back(reg_it.literalRegister);
                    OutputAssemblyCode.CommentAddress.push_back(it.address_of_call);
                }
            }
        }
    }
}
