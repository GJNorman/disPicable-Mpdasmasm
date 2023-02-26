//
//  MacroEngine.cpp
//  
//
//  Created by Greg Norman on 26/1/2023.
//

#include "MacroEngine.hpp"

extern char Global_working_directory[FILENAME_MAX];
static std::vector<Macro_definition_t> MacroList;
static std::vector<include_directory_t> includesList;

static bool debugMacros = false;

std::vector<include_directory_t> checkForIncludedFiles()
{
    return includesList;
}
void clearIncludedFileDirectories()
{
    includesList.clear();
}
Macro_definition_t findArgumentsInMacro(std::string &MacroWithArgs, size_t initialOffset)
{
    Macro_definition_t NewDef ;
    
    RemoveLeadingSpaces(MacroWithArgs,initialOffset);
    
    //initialOffset=0;
    // find the end of the definition
    // #define A B      //seperated by white space
    // #define C        //sperated by new line
    size_t p3 = FindNextSpaceCharacter(MacroWithArgs,initialOffset);

    NewDef.tag = MacroWithArgs.substr(initialOffset,p3-initialOffset);
    
    // check if the tag has arguments included
    
    size_t argPos = NewDef.tag.find("(");
    
    if(argPos != std::string::npos)
    {
        // arguments are seperate by commas
        size_t nextComma = argPos+1;
        size_t nextComma1 = argPos+1;
        while( ( nextComma1 = NewDef.tag.find(",",nextComma)) !=std::string::npos)
        {
            NewDef.args.push_back(NewDef.tag.substr(nextComma,nextComma1-nextComma));
            nextComma=nextComma1+1;
        }
        
        size_t endBracket = NewDef.tag.find(")");
        
        NewDef.args.push_back(NewDef.tag.substr(nextComma,endBracket-nextComma));
        
        
        // remove arguments from tag
        NewDef.tag = NewDef.tag.substr(0,argPos);
    }
    
    NewDef.definition = MacroWithArgs.substr(p3,MacroWithArgs.size()-p3);
    
    return NewDef;
}
bool checkForMacro(std::string &Line, size_t filePos)
{
    // examples
    // macroName(p1, p2, p3, p4)
    
    Macro_definition_t NewDef = findArgumentsInMacro(Line,0);
    bool returnValue = false;
    for(auto NextMacro: MacroList)
    {
        if(NextMacro.tag.size() == NewDef.tag.size())
        {
            if(strncasecmp(NextMacro.tag.c_str(), NewDef.tag.c_str(),NextMacro.tag.size()) == 0)
            {
                returnValue = true;
                // copy arguments into macro text
                // example '#define Add(x,y) MOVF x,0,1   ADDWF y,1,1'
                // search for each name argument, replace it with the argument with the same index from the macro call
                // i.e. Add(7,2)
                // becomes
                //  Movf 7,0,1
                //  Addwf 2,1,1
                
                int counter = 0;
                
                Line.clear();
                Line=NextMacro.definition;
                for(auto arg: NextMacro.args)
                {
                    size_t nextArg = 0 ;
                    size_t prevArg = 0;
                    while( (nextArg = Line.find(arg,prevArg)) != std::string::npos)
                    {
                        
                        // argument could be bounded by spaces, brackets or commas, otherwise this may be an alias
                        
                        // check the character before the suspected argument
                        if(((Line[nextArg-1] != ' ') &&
                            (Line[nextArg-1] != ',') &&
                            (Line[nextArg-1] != '('))||
                           
                           // and the character afterwards
                           ((Line[nextArg+1] != ' ') &&
                            (Line[nextArg+1] != ',') &&
                            (Line[nextArg+1] != ')')))
                        {
                            continue;
                        }

                        
                        std::string StringBeforeArgument = Line.substr(prevArg,nextArg-prevArg);

                        nextArg += arg.size();
                        
                        std::string StringAfterArgument = Line.substr(nextArg,Line.size()-nextArg);
                        
                        Line = StringBeforeArgument + NewDef.args[counter] + StringAfterArgument;
                        
                        prevArg=nextArg+1;
                    }
                    
                    counter++;
                }
                if(debugMacros)
                    std::cout << "Replacing Macro: '" << NewDef.tag << "' with " << Line<<"\n";
            }
        }
    }
    
    return returnValue;
}
static void addIncludeDirectory(uint8_t type,const char* boundingCharacter, std::string NextLine, size_t &fileNameStart)
{
    fileNameStart++;
    size_t fileNameEnd = NextLine.find(boundingCharacter,fileNameStart);
    
    include_directory_t newDir;
    std::string NewFileName = (NextLine.substr(fileNameStart,fileNameEnd-fileNameStart));
    
    if(type == INCLUDE_RELATIVE)
    {
        std::string Annoying = "/";
        std::string FileDir = Global_working_directory + Annoying + NewFileName;
    }
    newDir.type = type;
    newDir.name = NewFileName;
    includesList.push_back(newDir);
}
void checkPreprocesseorDirectives(std::string &NextLine, size_t fileLine)
{
    if(debugMacros)
    {
        std::cout << "Checking directive: "<< NextLine << "\n";
    }
    
    size_t p2 = NextLine.find("#define");
    
    if(p2 != std::string::npos)
    {
        p2 += strlen("#define ");
        
        
        Macro_definition_t NewDef = findArgumentsInMacro(NextLine,p2);

        NewDef.scope_start = fileLine;
        NewDef.scope_end = ~0;
        
        MacroList.push_back(NewDef);
    }
    else
    {
        //look for other directives
        p2 = NextLine.find("#include");
        if(p2 != std::string::npos)
        {
            // look for relative directory, bounded by " "
            
            size_t fileNameStart = NextLine.find("\"");
            
            if(fileNameStart!=std::string::npos)
            {
                addIncludeDirectory(INCLUDE_RELATIVE,"\"", NextLine, fileNameStart);
            }
            // look for absolute directory, bound by < >
            else
            {
                size_t fileNameStart = NextLine.find("<");
                
                if(fileNameStart!=std::string::npos)
                {
                    addIncludeDirectory(INCLUDE_ABSOLUTE,">", NextLine, fileNameStart);
                }
            }
        }
    }
    return ;
}
void printUserMacros()
{
    for(auto it: MacroList)
    {
        std::cout << it.tag << "' '" << it.definition << " args = ";
        
        for(auto args: it.args)
        {
            std::cout << args << ",";
        }
        std::cout << "\n";
    }
}

// returns a filename if the directive is to include another file
void createListOfUserMacros(std::vector<std::string> CODE_FULL_PROGRAM)
{
    size_t line_counter = 0;
    for(auto NextLine : CODE_FULL_PROGRAM)
    {
        // check for a potential preprocessor directive
        
        size_t p1 = NextLine.find("#");
        if(p1 != std::string::npos)
        {
            checkPreprocesseorDirectives(NextLine,line_counter);
        }
        line_counter++;
    }
    
    return ;
}
