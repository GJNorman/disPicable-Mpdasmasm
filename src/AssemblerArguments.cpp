//
//  AssemblerArguments.cpp
//  
//
//  Created by Greg Norman on 28/1/2023.
//

#include "AssemblerArguments.hpp"



static std::vector<NumberSystemDecoder_t> NumberSystemDecoder;
void initNumberSystemDecoder_t();
void addNewBase(uint8_t base,std::string nomenclature,bool leading);

/*###################################################################################
 Functions for decoding different number systems
 ###################################################################################*/
void addNewBase(uint8_t base,std::string nomenclature,bool leading)
{
    NumberSystemDecoder_t newNum = {.base = base,.nomenclature=nomenclature,.leading = leading};
    
    NumberSystemDecoder.push_back(newNum);
}
void initNumberSystemDecoder_t()
{
    addNewBase(16,"0x",true);   // 0x37
    addNewBase(16,"h'",true);   // h'37
   // addNewBase(16,"h",false);   // 37h
    
    addNewBase(10,"0d",true);   // 0d37
    addNewBase(10,"d'",true);   // d'37
    
    addNewBase(2,"0b",true);    // 0b10
    addNewBase(2,"b'",true);    // b'10
}
// where f represents a file address
char *check_for_f_based_on_number_system(const char *Assembly_Instruction,NumberSystemDecoder_t decoder,  uint8_t &f_base)
{
    char *f_prm = NULL;
    
    const char *Check = strstr(Assembly_Instruction,decoder.nomenclature.c_str());
    
    if(Check!=NULL)
    {
        if(decoder.leading == true)
        {
            // check points to start of number
            size_t p1 = Check-Assembly_Instruction;
            
            if(strstr(Assembly_Instruction+p1,","))
            {
                f_prm = return_substring(Assembly_Instruction,decoder.nomenclature.c_str(), ",", 0,strlen(decoder.nomenclature.c_str()), 0);
            }
            else
            {
                f_prm = return_substring(Assembly_Instruction,decoder.nomenclature.c_str(), NULL, 0,strlen(decoder.nomenclature.c_str()), 0);
            }
            
        }
        else
        {
            // check points to end of number
            size_t start_of_num = findWhereCurrentWordBegan(Assembly_Instruction,Check-Assembly_Instruction);
            
            f_prm = copy_out_substring(start_of_num, Check-Assembly_Instruction, Assembly_Instruction);
            
        }
    }
    if(f_prm!=NULL)
    {
        f_base = decoder.base;
    }
    return f_prm;
}
uint8_t find_b_value_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address)
{
    
    
    return 0;
}
/*###################################################################################
    For commands that utilise an 'f' parameter
 
 i.e.   ADDWF f,d,a
        BCF f,b,a
        MOVFW f,a
 ###################################################################################*/
uint32_t find_f_value_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address)
{
    uint32_t f=0;
    uint8_t f_base = 16;
    char *f_prm = NULL;
    
    //      MOVWF TRISE, A
    //      MOVWF ANSEL, A
    //      CALL  LABEL, 0
    if(NumberSystemDecoder.empty() == true)
    {
        initNumberSystemDecoder_t();
    }

    for(auto baseSystem: NumberSystemDecoder)
    {
        f_prm = check_for_f_based_on_number_system(Assembly_Instruction,baseSystem,f_base);
        if(f_prm!=NULL)
        {
            break;
        }
    }

    if(f_prm==NULL) //SFRs
    {
        // SFR is between commnd and a comma
        f_prm = return_substring(Assembly_Instruction,Instruction_Set.PIC18F_MNEMONICS[pos].c_str(), ",", 0,strlen(Instruction_Set.PIC18F_MNEMONICS[pos].c_str()), 0);

        if(f_prm==NULL)
        {
            // SFR is between command and end of instruction
            f_prm = return_substring(Assembly_Instruction,Instruction_Set.PIC18F_MNEMONICS[pos].c_str(), NULL, 0,strlen(Instruction_Set.PIC18F_MNEMONICS[pos].c_str()), 0);
        }
        if(strtol(f_prm,NULL,16)==1)
        {
            f=1;
        }
        else
        {
            f=convert_SFR_for_assembler(f_prm,Instruction_Set,0);
        }

    }
    else //f is a raw value
    {
        f = (strtol(f_prm, NULL, f_base)&0xffff);
    }
    if(f_prm!=NULL)
        free(f_prm);
    //N needs futher processing
    if((Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_CTRL_CASE)||(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_CTRL_CASE2))
    {
        uint32_t n = check_n_for_assembly(f, address&0xffff);
        if((Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_CTRL_CASE))
        {
            return n&0xff;
        }
        return n&0x7ff;//Used by BRA
    }

    return f;
}

/*###################################################################################
    For commands that utilise a 'd' or a 'b' parameter
 
 i.e.   ADDWF f,d,a
        BCF f,b,a
 
        d is an optional parameter
 
        from IS manual
        ' If ‘d’ is ‘1’, the result is stored back in register ‘f’ (default).'
 
 ###################################################################################*/
uint8_t find_d_value_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, bool &optionalArgumentPresent)
{
    uint8_t d_or_b=1;   // defined as 1 by default
    char *d_prm = return_substring(Assembly_Instruction, ",", ",", 0, strlen(","), 0);
    if(d_prm!=NULL)
    {
        optionalArgumentPresent = true;
        if(strstr(d_prm,"0x")==NULL)    // not a literal
        {
            
            // check for a bitfield
            std::string temp = d_prm;
            
            RemoveLeadingSpaces(temp, 0);
            
            d_or_b = 0xff & processEQUBitForAssembler(temp);

        }
        else
        {
            char *d_val = return_substring(d_prm, "0x",NULL, 0, strlen("0x"), 0);

            if(d_val!=NULL)
            {
                d_or_b= strtol(d_val, NULL, 16);
                free(d_val);
            }
        }
        free(d_prm);
    }
    return d_or_b;
}

/*###################################################################################
 // 'a' selects either access bank or GPR bank
 // the default value is ACCESS (0)
 // argument is optional
 ###################################################################################*/

uint8_t find_a_value_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, size_t a_offset)
{
    uint8_t a=0;
    // A is always the final parameter
    // there will always be a comma before it
    char *a_prm = return_substring(Assembly_Instruction, ",", NULL, (strstr(&Assembly_Instruction[0],",")-Assembly_Instruction)+a_offset, strlen(","), 0);
    if(a_prm!=NULL)
    {
        
        //a can be either 1 or 0;
        if(strstr(a_prm,"ACCESS"))
        {
            a=0;
        }
        else if(strstr(a_prm,"BANK"))
        {
            a=1;
        }
        else if(strstr(a_prm,"A"))
        {
            a=0;
        }
        else if(strstr(a_prm,"B"))
        {
            a=1;
        }
        else
        {
            a = strtol(a_prm, NULL, 16);//che sera sera
        }
        free(a_prm);
    }


    return a;
}

/*###################################################################################
 LFSR 2, 0x500
 
 where '2' means FSR2
 
 and 0x500 is the 16-bit memory address
 
 ###################################################################################*/
uint8_t find_LFSR_f_value(const char *Assembly_Instruction)
{
    char *f_prm = return_substring(Assembly_Instruction, "LFSR", ",", 0, strlen("LFSR"), 0);
    if(f_prm==NULL)
    {
        return 0;
    }
    uint8_t f = strtol(f_prm,NULL,16);
    free(f_prm);

    return f;
}
/*###################################################################################
 find FSR from EQU
 ###################################################################################*/
uint16_t check_if_SFR_or_RAW_REG(const char *REGISTER,PIC18F_FULL_IS & Instruction_Set)
{
    uint16_t value = convert_SFR_for_assembler(REGISTER,Instruction_Set,0xf00);

    return value;
}
/*###################################################################################
 
 ###################################################################################*/
uint32_t convert_SFR_for_assembler(const char *SFR_REG,PIC18F_FULL_IS & Instruction_Set, uint32_t offset)
{
    // remove leading white space
    size_t p1=CheckForLeadingSpaces(SFR_REG,0);
    
    uint32_t f=0;
    uint16_t counter=0;

    // if a 'high()' or 'low()' directive has been used, we will find it here
    bool high = true;
    const char *HIGH_LOW_DIRECTIVE = strstr(SFR_REG,"high(");
    
    if(HIGH_LOW_DIRECTIVE == NULL)
    {
        HIGH_LOW_DIRECTIVE = strstr(SFR_REG,"low(");
        if(HIGH_LOW_DIRECTIVE!=NULL)
        {
            HIGH_LOW_DIRECTIVE = &HIGH_LOW_DIRECTIVE[strlen("low(")];
        }
        high = false;
    }
    else
    {
        HIGH_LOW_DIRECTIVE = &HIGH_LOW_DIRECTIVE[strlen("high(")];
    }
    if(HIGH_LOW_DIRECTIVE != NULL)
    {
        uint8_t bitshift_value = 0;
    
        const char *end = strstr(HIGH_LOW_DIRECTIVE, ")");
    
        if(end!=NULL)
        {
            if(high == true)
            {
                bitshift_value=8;
            }
    
            size_t search_term_size =end-HIGH_LOW_DIRECTIVE ;
            char search_term_db[search_term_size + 1];
            
            snprintf(search_term_db,search_term_size+1, "%s",HIGH_LOW_DIRECTIVE);
            std::cout<<search_term_db<<"\n";
            // we'll check if its a tag for a DB
            for( counter =0; counter < Instruction_Set.Define_Byte_Tags.size();counter++)
            {
                if(strlen(search_term_db) == (Instruction_Set.Define_Byte_Tags[counter].size()))
                {
                    if(strncasecmp(search_term_db,Instruction_Set.Define_Byte_Tags[counter].c_str(),strlen(&SFR_REG[p1]))==0)
                    {
                        f = (Instruction_Set.Define_byte_tag_positions[counter]>>bitshift_value)&0xff;
                        
                        return f;
                    }
                }
            }
        }
        //no point continuing if there was no match found
        printf ("Unable to identify db tag '%.s' \n",&SFR_REG[p1]);
        return f;
    }
    // compare to the list of MCU registers and user variables
    std::string temp = &SFR_REG[p1];
    uint32_t temp_f = processEQUforAssembler(temp);
    if(temp_f != ~0)
    {
        f = temp_f;
    }

    return f;
}


/*###################################################################################
 // n is used in branching instructions
 ###################################################################################*/

uint32_t check_n_for_assembly(int32_t f, uint32_t program_counter)
{
    int32_t n=f-program_counter-2;

    return (n>>1);
}
