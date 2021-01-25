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

int AppendBufferToFile( std::string const& filename, size_t inSize, byte* bufferToWrite )
{
	FILE* fp = nullptr;
	fopen_s( &fp, filename.c_str(), "ab" );
	if( fp == nullptr )
	{
		return -1;
	}

	fwrite( bufferToWrite, sizeof(byte), inSize, fp );
	fclose( fp );

	return 0;
}

void AppendDataToBuffer( byte* dataToWrite, size_t byteCount, std::vector<byte>& buffer )
{
	size_t capacity = buffer.capacity();
	size_t bufferSize = buffer.size();
	size_t requiredCapacity = bufferSize + byteCount;

	if( capacity < requiredCapacity )
	{
		buffer.resize( requiredCapacity );
	}

	memcpy( &buffer[bufferSize], dataToWrite, byteCount );
}

float ParseFloat( byte*& buffer )
{
	float* floatPtr = (float*)buffer;
	buffer += sizeof(float);
	return *floatPtr;
}

int ParseInt( byte*& buffer )
{
	int* intPtr = (int*)buffer;

	buffer += sizeof(int);

	return *intPtr;
}

bool ParseBool( byte*& buffer )
{
	bool* boolPtr = (bool*)buffer;
	buffer += sizeof(bool);
	return *boolPtr;
}
