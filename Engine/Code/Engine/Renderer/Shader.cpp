#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include <stdio.h>

#include <d3dcompiler.h>

void* FileReadToNewBuffer( std::string const& filename, size_t* outSize = nullptr )
{
	FILE* fp = nullptr;
	fopen_s(&fp, filename.c_str(), "r");
	if( fp == nullptr )
	{
		return nullptr;
	}

	fseek( fp, 0, SEEK_END );
	long fileSize = ftell(fp);

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



	fclose(fp);
	return buffer;
}

bool Shader::CreateFromFile( std::string const& filename )
{
	size_t fileSize = 0;
	void * src = FileReadToNewBuffer( filename, &fileSize );
	if( src == nullptr )
	{
		return false;
	}

	delete[] src;

	return true;
}

bool ShaderStage::Compile( RenderContext* context, std::string const& filename, void const* source, size_t const sourceByteLen, SHADERTYPE stage )
{
	UNUSED(context);
	UNUSED(filename);
	UNUSED(source);
	UNUSED(sourceByteLen);
	UNUSED(stage);

	return false;
}
