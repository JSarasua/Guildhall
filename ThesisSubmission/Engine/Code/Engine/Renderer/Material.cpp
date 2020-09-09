#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"


Material::Material( RenderContext* context, char const* materialFilePath )
{
	m_context = context;
	m_ubo = new RenderBuffer( context, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	std::string filePath = std::string( materialFilePath );

	if( nullptr != materialFilePath && "" != filePath )
	{
		XmlDocument document;
		XmlElement const& element = GetRootElement( document, materialFilePath );

		LoadDataFromXML( element );
	}
}

Material::Material( RenderContext* context, XmlElement const& element )
{
	m_context = context;
	m_ubo = new RenderBuffer( context, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	LoadDataFromXML( element );
}

Material::~Material()
{
	delete m_ubo;
	m_ubo = nullptr;


	for( size_t samplerIndex = 0; samplerIndex < m_samplersPerSlot.size(); samplerIndex++ )
	{
		delete m_samplersPerSlot[samplerIndex];
		m_samplersPerSlot[samplerIndex] = nullptr;
	}
}

void Material::LoadDataFromXML( XmlElement const& element )
{
	XmlElement const* texturesElement = element.FirstChildElement( "Textures" );
	XmlElement const* samplersElement = element.FirstChildElement( "Samplers" );
	XmlElement const* shaderStateElement = element.FirstChildElement( "ShaderState" );
	m_shaderState = new ShaderState(m_context, *shaderStateElement );

	m_tint = ParseXMLAttribute( element, "tint", Rgba8::WHITE );
	m_specularFactor = ParseXMLAttribute( element, "specularFactor", 1.f );
	m_specularPower = ParseXMLAttribute( element, "specularPower", 1.f );
	std::string diffuseTexturePath = ParseXMLAttribute( element, "diffuseTexturePath", "" );
	std::string normalTexturePath = ParseXMLAttribute( element, "normalTexturePath", "" );

	if( "" != diffuseTexturePath )
	{
		m_diffuseTexture = m_context->CreateOrGetTextureFromFile( diffuseTexturePath.c_str() );
	}

	if( "" != normalTexturePath )
	{
		m_normalTexture = m_context->CreateOrGetTextureFromFile( normalTexturePath.c_str() );
	}

	//load textures
	LoadTexturesFromXML( *texturesElement );

	//load samplers
	LoadSamplersFromXML( *samplersElement );

}

void Material::LoadTexturesFromXML( XmlElement const& element )
{
	XmlElement const* child = element.FirstChildElement();

	while( child != nullptr )
	{
		std::string texturePath = ParseXMLAttribute( *child, "textureFilePath", "" );

		if( "" != texturePath )
		{
			Texture* texture = m_context->CreateOrGetTextureFromFile( texturePath.c_str() );
			m_texturePerSlot.push_back( texture );
		}

		child = child->NextSiblingElement();
	}
}

void Material::LoadSamplersFromXML( XmlElement const& element )
{
	XmlElement const* child = element.FirstChildElement();

	while( child != nullptr )
	{
		std::string samplerType = ParseXMLAttribute( *child, "SamplerType", "SAMPLER_POINT" );
		std::string samplerEdgeMode = ParseXMLAttribute( *child, "SamplerEdgeMode", "SAMPLER_WRAP" );

		LoadSamplerFromString( samplerType, samplerEdgeMode );

		child = child->NextSiblingElement();
	}
}

void Material::LoadSamplerFromString( std::string samplerTypeStr, std::string samplerEdgeModeStr )
{
	eSamplerType samplerType = SAMPLER_POINT;
	eSamplerEdgeMode samplerEdgeMode = SAMPLER_WRAP;

	if( samplerTypeStr == "SAMPLER_POINT" )
	{
		samplerType = SAMPLER_POINT;
	}
	else if( samplerTypeStr == "SAMPLER_BILINEAR" )
	{
		samplerType = SAMPLER_BILINEAR;
	}
	else
	{
		ERROR_AND_DIE( "Invalid Sampler Type. Must be SAMPLER_POINT or SAMPLER_BILINEAR");
	}

	if( samplerEdgeModeStr == "SAMPLER_WRAP" )
	{
		samplerEdgeMode = SAMPLER_WRAP;
	}
	else if( samplerEdgeModeStr == "SAMPLER_MIRROR" )
	{
		samplerEdgeMode = SAMPLER_MIRROR;
	}
	else if( samplerEdgeModeStr == "SAMPLER_CLAMP" )
	{
		samplerEdgeMode = SAMPLER_CLAMP;
	}
	else if( samplerEdgeModeStr == "SAMPLER_BORDER" )
	{
		samplerEdgeMode = SAMPLER_BORDER;
	}
	else
	{
		ERROR_AND_DIE( "Invalid Sampler Edge Mode. Must be SAMPLER_WRAP, SAMPLER_MIRROR, SAMPLER_CLAMP, or SAMPLER_BORDER" );
	}

	Sampler* newSampler = new Sampler( m_context, samplerType, samplerEdgeMode );

	m_samplersPerSlot.push_back( newSampler );
}

void Material::SetShaderByName( Shader* shader )
{
	m_shaderState->m_shader = shader;
}

void Material::SetShaderStateByName( ShaderState* shaderState )
{
	m_shaderState = shaderState;
}

void Material::SetDiffuseTexture( Texture* diffuseTexture )
{
	m_diffuseTexture = diffuseTexture;
}

void Material::SetNormalTexture( Texture* normalTexture )
{
	m_normalTexture = normalTexture;
}

void Material::UpdateUBOIfDirty()
{
	if( m_uboISDirty )
	{
		m_ubo->Update( &m_uboCPUData[0], m_uboCPUData.size(), m_uboCPUData.size() );
	}
}

