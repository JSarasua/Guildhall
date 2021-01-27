#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"

byte* FileReadToNewBuffer( std::string const& filename, size_t* outSize = nullptr );
int AppendBufferToFile( std::string const& filename, size_t inSize, byte* bufferToWrite );
void DeleteFile( std::string const& filename );

void AppendDataToBuffer( byte const* dataToWrite, size_t byteCount, std::vector<byte>& buffer, size_t& startIndex );

float	ParseFloat( byte*& buffer );
int		ParseInt( byte*& buffer );
bool	ParseBool( byte*& buffer );