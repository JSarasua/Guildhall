#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"

byte* FileReadToNewBuffer( std::string const& filename, size_t* outSize = nullptr );
int AppendBufferToFile( std::string const& filename, size_t inSize, byte* bufferToWrite );

void AppendDataToBuffer( byte* dataToWrite, size_t byteCount, std::vector<byte>& buffer );

float	ParseFloat( void*& buffer );
int		ParseInt( byte*& buffer );