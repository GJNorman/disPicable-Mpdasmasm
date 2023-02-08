//
//  BasicFileEditing.hpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#ifndef BasicFileEditing_hpp
#define BasicFileEditing_hpp

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

std::string remove_tabs(std::string next_line);
char* delete_all_the_stupid_carriage_returns(const char *input_file);
void Copy_FIRMWARE_FILE_to_Buffer(const char *Input_File_dir,std::vector<unsigned char> &OutputFileContents);
char *copy_file_to_buffer(const char *inputfiledir);
bool check_if_line_is_empty(std::string line_from_file);

bool  check_data_bytes(char &data);
void check_for_escape_characters(uint8_t &counter, char *&data_bytes);
void removeEscapeCharacter(char character, char* dst, size_t dst_size);
/*
 
 for finding substrings
 */
char *copy_out_substring(size_t p1, size_t p2, const char* parent);
size_t findTermInCharString(size_t defaultValue,const char *parent_String,const char *searchTerm,size_t search_start_offset,size_t search_end_offset);
char* return_substring(const char *parent_String, const char *searchTerm1,const char *searchTerm2, size_t search_start_offset, size_t search_end_offset, size_t search_end_offset2);

size_t CheckForLeadingSpaces(const char *data, size_t start_pos);
void removeCharacters(const char *CharacterToRemove, const char *ReplacementCharacter, char *buffer);
size_t RemoveLeadingSpaces(std::string &data, size_t start_pos);
size_t FindNextSpaceCharacter(const char* data, size_t start_pos);
size_t FindNextSpaceCharacter(std::string &data, size_t start_pos);
size_t findWhereCurrentWordBegan(const char* data,size_t currentPos);
size_t FindEndOfCurrentWord(const char *data, size_t start_pos);


#endif /* BasicFileEditing_hpp */
