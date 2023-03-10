//
//  AssemblerMachineCodeWriter.cpp
//  
//
//  Created by Greg Norman on 28/1/2023.
//

#include "AssemblerMachineCodeWriter.hpp"



static std::ostringstream hexBuffer;                            // stores hex data


/*
 copy hex data to buffer
 */
void output_Machine_Code(const char *Format_Specifier, ...)
{
    static char buffer[1000];                                       // used for formatting string
    
    va_list args;
    va_start(args, Format_Specifier);

    vsnprintf(buffer,sizeof(buffer),Format_Specifier,args);

    hexBuffer<<buffer;
    va_end(args);
}

/*
 copy the hex code to the output file specified
 
 optionally print it to std::cout
 */
void printHexCode(bool display_on_screen, const char *output_dir)
{
    if(display_on_screen == true)
    {
        std::cout<<hexBuffer.str();
    }
    
    if(output_dir!=NULL)
    {
        FILE *fp = fopen(output_dir,"wb");
        fprintf(fp,"%s",hexBuffer.str().c_str());
        
        fclose(fp);
    }
    hexBuffer.clear();
}

std::ostringstream *grabMachineCodeBuffer()
{
    
    return &hexBuffer;
}
void clearMachineCodeBuffer()
{
    hexBuffer.str(std::string());   // some fancy man on the internet told me that this is the only way to clear an ostringstream
                                    // because hexBuffer.clear() only clears the error state
                                    // which is an incredibly sensible and well thought out decision
                                    // that leads to no ambiguity at all
                                    // this method of clearing a ostringstream is much more intuitive
                                    // i think we can all agree its pretty swell!
}
