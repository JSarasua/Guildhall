#include "Engine/Core/FileUtils.hpp"

byte* FileReadToNewBuffer( std::string const& filename, size_t* outSize )
{
	FILE* fp = nullptr;
	fopen_s( &fp, filename.c_str(), "r" );
	if( fp == nullptr )
	{
		return nullptr;
	}

	fseek( fp, 0, SEEK_END );
	long fileSize = ftell( fp );

	unsigned char* buffer = new unsigned char[fileSize + 1];
	if( nullptr != buffer )
	{
		fseek( fp, 0, SEEK_SET );
		size_t bytesRead = fread( buffer, 1, (size_t)fileSize, fp );
		buffer[bytesRead] = NULL;

		if( outSize != nullptr )
		{
			*outSize = (size_t)bytesRead;
		}
	}



	fclose( fp );
	return buffer;
}

int ParseInt( byte*& buffer )
{
	int* intPtr = (int*)buffer;

	buffer += sizeof(int);

	return *intPtr;
}
