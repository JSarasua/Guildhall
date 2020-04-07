#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BufferAttribute.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
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

Shader::Shader( RenderContext* context )
	: m_owner(context)
{
}

Shader::~Shader()
{
	DX_SAFE_RELEASE(m_inputLayout);
}

bool Shader::CreateFromFile( std::string const& filename )
{
	size_t fileSize = 0;
	void * source = FileReadToNewBuffer( filename, &fileSize );
	if( source == nullptr )
	{
		return false;
	}

	m_vertexStage.Compile( m_owner, filename, source, fileSize, SHADER_TYPE_VERTEX );
	m_fragmentStage.Compile( m_owner, filename, source, fileSize, SHADER_TYPE_FRAGMENT );

	m_filename = filename;
	
	delete[] source;

	return m_vertexStage.IsValid() && m_fragmentStage.IsValid();
}

ID3D11InputLayout* Shader::GetOrCreateInputLayout(  BufferAttribute const*  layout )
{
	if( layout == m_previousLayout )
	{
		return m_inputLayout;
	}
	else if( nullptr != m_inputLayout )
	{
		return m_inputLayout;
	}
	m_previousLayout = layout;

	std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDescription;
	size_t vertexDescIndex = 0;
	while( true )
	{
		if( layout[vertexDescIndex].m_name.empty() )
		{
			break;
		}
		char const* name = layout[vertexDescIndex].m_name.c_str();
		uint offset = layout[vertexDescIndex].m_offset;
		

		DXGI_FORMAT format;
		switch( layout[vertexDescIndex].m_type )
		{
		case BUFFER_FORMAT_VEC2:
			format = DXGI_FORMAT_R32G32_FLOAT;
			break;
		case BUFFER_FORMAT_VEC3:
			format = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case BUFFER_FORMAT_R8G8B8A8_UNORM:
			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		default:
			ERROR_AND_DIE("Need a valid format");
			break;
		}

		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName				= name;
		desc.SemanticIndex				= 0;								//Array Element if we had color[3] or something
		desc.Format						= format;
		desc.InputSlot					= 0;
		desc.AlignedByteOffset			= offset;
		desc.InputSlotClass				= D3D11_INPUT_PER_VERTEX_DATA;		//I'm drawing a tree, PER_INSTANCE_DATA is I'm drawing a million trees
		desc.InstanceDataStepRate		= 0;

		vertexDescription.push_back(desc);

		vertexDescIndex++;
	}

	ID3D11Device* device = m_owner->m_device;
	device->CreateInputLayout( 
		&vertexDescription[0], 
		(uint)vertexDescription.size(),
		m_vertexStage.GetByteCode(),
		m_vertexStage.GetByteCodeLength(),
		&m_inputLayout );

	return m_inputLayout;
}

static char const* GetDefaultEntryPointForStage( SHADERTYPE type )
{
	switch( type )
	{
	case SHADER_TYPE_VERTEX: return "VertexFunction";
	case SHADER_TYPE_FRAGMENT: return "FragmentFunction";
	default: ERROR_AND_DIE("Invalid Shader Type");
	}

}

static char const* GetShaderModelForStage( SHADERTYPE type )
{
	switch( type )
	{
	case SHADER_TYPE_VERTEX: return "vs_5_0";
	case SHADER_TYPE_FRAGMENT: return "ps_5_0";
	default: ERROR_AND_DIE("Unknown Shader Stage");
	}
}

ShaderStage::~ShaderStage()
{
	DX_SAFE_RELEASE(m_byteCode);
	DX_SAFE_RELEASE(m_handle);

}

bool ShaderStage::Compile( RenderContext* context, std::string const& filename, void const* source, size_t const sourceByteLen, SHADERTYPE stage )
{
	//HLSL - High Level Shading Language
	//Compile: HLSL -> ByteCode
	//Link ByteCode -> Device Assembly (what we need to get to) - This is device specific

	char const* entrypoint = GetDefaultEntryPointForStage( stage );
	char const* shaderModel = GetShaderModelForStage( stage );

	DWORD compileFlags = 0U;
	#if defined(DEBUG_SHADERS)
	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
	#else 
		// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
	#endif

	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors = nullptr;


	HRESULT hr = ::D3DCompile( source,
		sourceByteLen,
		filename.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint,
		shaderModel,
		compileFlags,
		0,
		&byteCode,
		&errors );

	if( FAILED( hr ) )
	{
		if( nullptr != errors )
		{
			char *errorString = (char*) errors->GetBufferPointer();
			DebuggerPrintf( "Failed to compile [%s]. Compiler gave the following output;\n%s",
				filename.c_str(),
				errorString );

			DEBUGBREAK();
		}
	}
	else
	{
		ID3D11Device* device = context->m_device;
		void const* byteCodePtr = byteCode->GetBufferPointer();
		size_t byteCodeSize = byteCode->GetBufferSize();
		switch( stage )
		{

			case SHADER_TYPE_VERTEX: 
			{
				hr = device->CreateVertexShader( byteCodePtr, byteCodeSize, nullptr, &m_vs );
				GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to link shader stage" );
			} break;

			case SHADER_TYPE_FRAGMENT:
			{
				hr = device->CreatePixelShader( byteCodePtr, byteCodeSize, nullptr, &m_fs );
				GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to link shader stage" );
			} break;
			default: ERROR_AND_DIE( "Unimplemented stage."); break;
		}
	}

	DX_SAFE_RELEASE(errors);

	if( stage == SHADER_TYPE_VERTEX )
	{
		m_byteCode = byteCode;
	}
	else
	{
		DX_SAFE_RELEASE(byteCode);
		m_byteCode = nullptr;
	}

	m_type = stage;

	return IsValid();
}

void const* ShaderStage::GetByteCode()
{
	return m_byteCode->GetBufferPointer();
}

size_t ShaderStage::GetByteCodeLength() const
{
	return m_byteCode->GetBufferSize();
}
