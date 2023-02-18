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


std::string grabFSRList(int funcNum)
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

static void setFunctionRegisterList(bool &newFunction, uint32_t address)
{
    std::string EQU = getMostRecentEQU();
    if(newFunction == true)
    {
        newFunction = false;
       
        RegistersTracker_t newTracker ;
        
        newTracker.Reg = "SFRs: ";
        newTracker.address = address;
        FunctionRegisterList.push_back(newTracker);
    }
    if(EQU.empty() == false)
    {
        // make sure we haven't already recorded this regiser
        // sometimes you can end up with hundreds in large functions
        std::string Temp = " "+ EQU + ",";
        
        if(FunctionRegisterList[CurrentFunctionCounter].Reg.find(Temp) == std::string::npos)
            FunctionRegisterList[CurrentFunctionCounter].Reg += EQU + ", ";
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
// keep track of how software stacks are being loaded (FSRs)
void watchFunctionStacks(char* Instruction,Converted_Assembly_Code &OutputAssemblyCode)
{
    
    static WREG_Follower_t Shadow_WREG ;
    static bool firstCall = true;
    
    uint32_t CurrentAddress = (uint32_t)OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size();
    setFunctionRegisterList(firstCall,(uint32_t)strtol(OutputAssemblyCode.Address[OutputAssemblyCode.Address.size()-1].c_str(),NULL,16));
                            
    if(strstr(Instruction,"MOVLW"))
    {
        size_t point1 = strstr(Instruction,"MOVLW")-Instruction + strlen("MOVLW") + 1;
        
        Shadow_WREG.value =strtol(&Instruction[point1], NULL, 16);
        Shadow_WREG.type = FuncionArgumentTypes::literal;
    }
    else if(strstr(Instruction,"MOVF "))
    {
        size_t point1 = strstr(Instruction,"MOVF")-Instruction + strlen("MOVF") + 1;
        
        size_t point2 = strstr(&Instruction[point1], ",")-Instruction;
        char temp[point2-point1+1];
        strncpy(temp,&Instruction[point1],point2-point1);
        temp[point2-point1] = 0;
        
        Shadow_WREG.REG.Reg = temp;
        Shadow_WREG.type = FuncionArgumentTypes::RAM;
    }
    if(strstr(Instruction,"POSTINC"))
    {
        if(strstr(Instruction,"FSR") == NULL)   // MOVFF FSR2, POSTINC1  ; this would indicate a context save, not stack loading
        {
            FunctionStack_t newStack;
            newStack.type = Shadow_WREG.type;
            if( Shadow_WREG.type == literal)
            {
                newStack.value = Shadow_WREG.value;
            }
            else
            {
                newStack.ArgumentName = Shadow_WREG.REG.Reg;
            }
            
            
            if(strstr(Instruction,"CLRF"))
            {
                Shadow_WREG.value = 0;
                newStack.value = 0;
            }
            else if(strstr(Instruction,"MOVFF"))
            {
                Shadow_WREG.value = 0;
                
                size_t point1 = strstr(Instruction,"MOVFF")-Instruction + strlen("MOVFF") + 1;
                
                size_t point2 = strstr(&Instruction[point1], ",")-Instruction;
                
                char temp[point2-point1+1];
                strncpy(temp,&Instruction[point1],point2-point1);
                temp[point2-point1] = 0;
                
                newStack.ArgumentName = temp;
                newStack.type = FuncionArgumentTypes::RAM;
            }
            else if(strstr(Instruction,"SETF"))
            {
                Shadow_WREG.value = 0xff;
                newStack.value = 0xff;
                newStack.type = literal;
            }
            
            inputArguments.push_back(newStack);
        }
    }
    else if((strstr(Instruction,"POSTDEC"))||(strstr(Instruction,"RET")))
    {
        inputArguments.clear();
        
        if(strstr(Instruction,"RET"))
        {
            firstCall = true;
            CurrentFunctionCounter++;
        }
    }
    else if(strstr(Instruction,"CALL"))
    {
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
    else if (strstr(Instruction,"PLUS"))
    {
        if(strstr(Instruction,"MOVFF")==NULL)
        {
            // check last movlw
        }
    }
}


/*
 
indicate which registers are being used in function calls, at the point where the function is called
 */
void HighlightFSRs(Converted_Assembly_Code &OutputAssemblyCode)
{
    for (auto it : ListOfFunctionCalls)
    {
        for(auto reg_it: FunctionRegisterList)
        {
            if(it.address_of_function == reg_it.address)
            {
                OutputAssemblyCode.Comments.push_back(reg_it.Reg);
                OutputAssemblyCode.CommentAddress.push_back(it.address_of_call);
            }
        }
    }
}
/*void highlightLocalVariables(Converted_Assembly_Code &OutputAssemblyCode)
{
    for (auto it : ListOfFunctionCalls)
    {
        for(size_t index = it.address_of_call; index< OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size() ; index++)
        {
            
        }
    }
}
*/
