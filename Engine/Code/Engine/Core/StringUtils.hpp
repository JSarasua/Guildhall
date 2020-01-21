#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

typedef std::vector<std::string> Strings;

//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

const std::vector<std::string> SplitStringOnDelimeter( const char* text, const char delimeter);

bool SetBoolFromText(const char* text);




