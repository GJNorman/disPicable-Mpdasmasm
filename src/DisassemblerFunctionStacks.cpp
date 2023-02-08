//
//  DisassemblerFunctionStacks.cpp
//  
//
//  Created by Greg Norman on 3/2/2023.
//

#include "DisassemblerFunctionStacks.hpp"
//TODO
/*
 
 MOVF 0x39, 0, 0                                ;address = 0837, data = 'n'
 MOVWF POSTINC1, A                              ;address = 0838, data = '∆'
 CALL LABEL_0x77C0 , 0                          ;args={0xFF ,0xFF }
 */
static std::vector<FunctionStack_t> inputArguments;




std::vector<std::vector<std::string>> FunctionRegisterList;
void keepTrackFunctionRegsiters()
{
    // will hold either and SFR or Regsiter address
    std::string EQU = getMostRecentEQU();
}


// keep track of how software stacks are being loaded (FSRs)
void watchFunctionStacks(char* Instruction,Converted_Assembly_Code &OutputAssemblyCode)
{
    static uint8_t previousMovlwValue = 0;
    if(strstr(Instruction,"MOVLW"))
    {
        size_t point1 = strstr(Instruction,"MOVLW")-Instruction + strlen("MOVLW") + 1;
        
        previousMovlwValue =strtol(&Instruction[point1], NULL, 16);
    }
    else if(strstr(Instruction,"POSTINC"))
    {
        if(strstr(Instruction,"FSR") == NULL)   // MOVFF FSR2, POSTINC1  ; this would indicate a context save, not stack loading
        {
            FunctionStack_t newStack;
            
            newStack.value = previousMovlwValue;
            newStack.type = INTEGER;
            if(strstr(Instruction,"CLRF"))
            {
                previousMovlwValue = 0;
                newStack.value = 0;
            }
            else if(strstr(Instruction,"MOVFF"))
            {
                previousMovlwValue = 0;
                
                size_t point1 = strstr(Instruction,"MOVFF")-Instruction + strlen("MOVFF") + 1;
                
                size_t point2 = strstr(&Instruction[point1], ",")-Instruction;
                
                char temp[point2-point1+1];
                strncpy(temp,&Instruction[point1],point2-point1);
                temp[point2-point1] = 0;
                
                newStack.ArgumentName = temp;
                newStack.type = REGISTER;
            }
            else if(strstr(Instruction,"SETF"))
            {
                previousMovlwValue = 0xff;
                newStack.value = 0xff;
            }
            
            inputArguments.push_back(newStack);
        }
    }
    else if(strstr(Instruction,"POSTDEC"))
    {
        inputArguments.clear();
    }
    else if(strstr(Instruction,"CALL"))
    {
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
            if(it.type == INTEGER)
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
}


/*
 
 read through the finalised file, looking for function calls
 
 then find the function being called, read through it to scan for SFRs
 
 */
void HighlightFSRs(Converted_Assembly_Code &OutputAssemblyCode)
{
    for(size_t it =0; it < OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size() ; it++)
    {
        if(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[it].find("CALL"))
        {
            // go through list of labels; and compar to call instruction label
            
            for(size_t labels = 0; labels < OutputAssemblyCode.LABEL_STRINGS.size(); labels++)
            {
                if(strstr(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[it].c_str(),  OutputAssemblyCode.LABEL_STRINGS[labels].c_str()))
                {
                    // being reading that address
                   // while(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM)
                }
            }
        }
    }
}
