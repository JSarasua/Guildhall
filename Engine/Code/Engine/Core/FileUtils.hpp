#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"

byte* FileReadToNewBuffer( std::string const& filename, size_t* outSize = nullptr );

float	ParseFloat( void*& buffer );
int		ParseInt( byte*& buffer );