#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
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
	CreateRasterState();
}

Shader::~Shader()
{
	DX_SAFE_RELEASE(m_rasterState);
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

	delete[] source;

	return m_vertexStage.IsValid() && m_fragmentStage.IsValid();
}

void Shader::CreateRasterState()
{
	D3D11_RASTERIZER_DESC desc; //description

	desc.FillMode = D3D11_FILL_SOLID; //full triangle
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = TRUE; //the only reason we're doing this;
	desc.DepthBias = 0U;
	desc.DepthBiasClamp = 0.f;
	desc.SlopeScaledDepthBias = 0.f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	ID3D11Device* device = m_owner->m_device;
	device->CreateRasterizerState( &desc, &m_rasterState );
}

ID3D11InputLayout* Shader::GetOrCreateInputLayout()
{
	if( nullptr != m_inputLayout )
	{
		return m_inputLayout;
	}

	D3D11_INPUT_ELEMENT_DESC vertexDescription[3];

	// position
	vertexDescription[0].SemanticName			= "POSITION";
	vertexDescription[0].SemanticIndex			= 0;	//Array Element if we had color[3] or something
	vertexDescription[0].Format					= DXGI_FORMAT_R32G32B32_FLOAT;	// 3 32-bit floats
	vertexDescription[0].InputSlot				= 0;
	vertexDescription[0].AlignedByteOffset		= offsetof( Vertex_PCU, position );
	vertexDescription[0].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;		//I'm drawing a tree, PER_INSTANCE_DATA is I'm drawing a million trees
	vertexDescription[0].InstanceDataStepRate	= 0;

	// position
	vertexDescription[1].SemanticName			= "COLOR";
	vertexDescription[1].SemanticIndex			= 0;	//Array Element if we had color[3] or something
	vertexDescription[1].Format					= DXGI_FORMAT_R8G8B8A8_UNORM;	// 4 1 byte channel. unsigned normal value
	vertexDescription[1].InputSlot				= 0;
	vertexDescription[1].AlignedByteOffset		= offsetof( Vertex_PCU, tint );
	vertexDescription[1].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;		//I'm drawing a tree, PER_INSTANCE_DATA is I'm drawing a million trees
	vertexDescription[1].InstanceDataStepRate	= 0;

	// position
	vertexDescription[2].SemanticName			= "TEXCOORD";
	vertexDescription[2].SemanticIndex			= 0;	//Array Element if we had color[3] or something
	vertexDescription[2].Format					= DXGI_FORMAT_R32G32_FLOAT;	// 3 32-bit floats
	vertexDescription[2].InputSlot				= 0;
	vertexDescription[2].AlignedByteOffset		= offsetof( Vertex_PCU, uvTexCoords );
	vertexDescription[2].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;		//I'm drawing a tree, PER_INSTANCE_DATA is I'm drawing a million trees
	vertexDescription[2].InstanceDataStepRate	= 0;

	ID3D11Device* device = m_owner->m_device;
	device->CreateInputLayout( 
		vertexDescription, 
		3,
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
