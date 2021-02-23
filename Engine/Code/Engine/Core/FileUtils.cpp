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

bool LoadBinaryFileToExistingBuffer( std::string const& fileName, std::vector<byte>& buffer )
{
	FILE* fp = nullptr;
	fopen_s( &fp, fileName.c_str(), "rb" );
	if( fp == nullptr )
	{
		return false;
	}

	fseek( fp, 0, SEEK_END );
	long fileSize = ftell( fp );

	buffer.resize( fileSize );

	fseek( fp, 0, SEEK_SET );
	/*size_t bytesRead = */fread( &buffer[0], 1, (size_t)fileSize, fp );
	//buffer[bytesRead] = NULL;

	fclose( fp );

	return true;
}

bool SaveBinaryFileFromBuffer( std::string const& filePath, std::vector<byte> const& buffer )
{
	FILE* fp = nullptr;
	fopen_s( &fp, filePath.c_str(), "wb" );
	if( fp == nullptr )
	{
		return false;
	}

	fwrite( &buffer[0], sizeof( byte ), buffer.size(), fp );
	fclose( fp );

	return true;
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

void DeleteFile( std::string const& filename )
{
	remove( filename.c_str() );
}

void AppendDataToBuffer( byte const* dataToWrite, size_t byteCount, std::vector<byte>& buffer, size_t& startIndex )
{
	//if buffer is null, then just return size
	//Would run through twice
	int bufferSize = (int)buffer.size();
	int capacity = bufferSize - (int)startIndex;

	if( capacity < 0 )
	{
		ERROR_AND_DIE( "startIndex is after current last element" );
	}

	if( capacity < byteCount )
	{
		buffer.resize( bufferSize + byteCount );
	}

	//Could just call resize without checking
	//buffer.data()[startIndex]

	memcpy( &buffer[startIndex], dataToWrite, byteCount );

	startIndex += byteCount;
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
