#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"


Material::Material( RenderContext* context, char const* materialFilePath )
{
	m_ubo = new RenderBuffer( context, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
}

Material::~Material()
{
	delete m_ubo;
	m_ubo = nullptr;
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

