#pragma once
#include <string>

void* FileReadToNewBuffer( std::string const& filename, size_t* outSize = nullptr );