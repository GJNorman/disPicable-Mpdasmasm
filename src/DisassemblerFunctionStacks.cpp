//
//  DisassemblerFunctionStacks.cpp
//  annoyed
//
//  Created by Greg Norman on 3/2/2023.
//

#include "DisassemblerFunctionStacks.hpp"

static std::vector<FunctionStack_t> inputArguments;
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
            
            char formatting[100];
            if(it.type == INTEGER)
            {
                char temp[10] ;
                temp[0] = '(';
                temp[1] = it.value;
                temp[2] = ')';
                temp[3] = 0;
                
                // get rid of escape sequences
                switch(it.value)
                {
                        
                    case '\?':snprintf(temp,sizeof(temp),"(\\?)"); break;
                    case '\a':snprintf(temp,sizeof(temp),"(\\a)"); break;
                    case '\b':snprintf(temp,sizeof(temp),"(\\b)"); break;
                    case '\f':snprintf(temp,sizeof(temp),"(\\f)"); break;
                    case '\n':snprintf(temp,sizeof(temp),"(\\n)"); break;
                    case '\r':snprintf(temp,sizeof(temp),"(\\r)"); break;
                    case '\t':snprintf(temp,sizeof(temp),"(\\t)"); break;
                    case '\v':snprintf(temp,sizeof(temp),"(\\v)"); break;
                    case '\0':snprintf(temp,sizeof(temp),"(\\0)"); break;
                    default:
                        if((it.value < 30)||(it.value>127))
                        {
                            temp[0] = 0;
                        }
                        break;
                }
                
                snprintf(formatting, sizeof(formatting), "0x%.2X %s",it.value,temp);
            }
            else
            {
                snprintf(formatting, sizeof(formatting), "REG = '%s'",it.ArgumentName.c_str());
            }
            
            // avoid placing comma before first agrument
            if(addComma++ > 0)
            {
                char Comma[2] = ",";
                strcat(COMMENT,Comma);
            }
            
            strcat(COMMENT,formatting);
            

        }
        strcat(COMMENT,"}");
        
        OutputAssemblyCode.Comments.push_back(COMMENT);
        OutputAssemblyCode.CommentAddress.push_back((uint32_t)OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size());
    }
}
