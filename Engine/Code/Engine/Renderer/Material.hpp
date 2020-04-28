#pragma once
#include "Engine/Core/Rgba8.hpp"
#include <vector>

class RenderBuffer;
class RenderContext;
class Sampler;
class Shader;
class ShaderState;
class Texture;


class Material
{
public:

	Material( RenderContext* context, char const* materialFilePath );
	~Material();

	void SetShaderByName( Shader* shader );
	void SetShaderStateByName( ShaderState* shaderState );
	void SetDiffuseTexture( Texture* diffuseTexture );
	void SetNormalTexture( Texture* normalTexture );

	void UpdateUBOIfDirty();

	void SetData( void const* data, size_t dataSize )
	{
		m_uboCPUData.resize( dataSize );
		memcpy( &m_uboCPUData[0], data, dataSize );
		m_uboISDirty = true;
	}

	template <typename UBO_STRUCT_TYPE>
	void SetData( UBO_STRUCT_TYPE const& data )
	{
		SetData( &data, sizeof( UBO_STRUCT_TYPE ) );
	}

	template <typename UBO_STRUCT_TYPE>
	UBO_STRUCT_TYPE* GetDataAs()
	{
		m_uboISDirty = true;
		if( m_uboCPUData.size() == sizeof( UBO_STRUCT_TYPE ) )
		{
			return (UBO_STRUCT_TYPE*) &m_uboCPUData[0];
		}
		else
		{
			m_uboCPUData.resize( sizeof( UBO_STRUCT_TYPE ) );
			UBO_STRUCT_TYPE* returnPtr = (UBO_STRUCT_TYPE*) &m_uboCPUData[0];
			new (returnPtr) UBO_STRUCT_TYPE();

			return returnPtr;
		}
	}

public:
	ShaderState* m_shaderState = nullptr;

	Rgba8 m_tint = Rgba8::WHITE;
	float m_specularFactor = 1.f;
	float m_specularPower = 1.f;
	
	Texture* m_diffuseTexture = nullptr;
	Texture* m_normalTexture = nullptr;

	std::vector<Texture*> m_texturePerSlot;
	std::vector<Sampler*> m_samplersPerSlot;

	bool m_uboISDirty = false;
	std::vector<unsigned char> m_uboCPUData;
	RenderBuffer* m_ubo = nullptr;
};