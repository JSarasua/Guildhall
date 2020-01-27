#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include <string>


Texture::Texture()
{
	DX_SAFE_RELEASE( m_handle );
}

Texture::Texture( int textureId, const IntVec2& texelSizeCoords, const char* filePath ) :
	m_textureId(textureId),
	m_texelSizeCoords(texelSizeCoords),
	m_filePath(filePath)
{}

Texture::Texture( RenderContext* context, ID3D11Texture2D* handle )
	: m_owner( context ),
	m_handle( handle )
{

}

int Texture::GetTextureID() const
{
	return m_textureId;
}

const IntVec2& Texture::GetTexelSize() const
{
	return m_texelSizeCoords;
}

bool Texture::IsFileInTexture( const char* filePath )
{
	std::string fileToCompare = std::string(filePath);
	std::string textureFile = std::string(m_filePath);
	if( textureFile == fileToCompare )
	{
		return true;
	}
	return false;
}

Texture::~Texture()
{
	delete m_renderTargetView;
	m_renderTargetView = nullptr;
	m_owner = nullptr;

	DX_SAFE_RELEASE( m_handle );
}

TextureView* Texture::GetRenderTargetView()
{
	if( nullptr != m_renderTargetView )
	{
		return m_renderTargetView;
	}

	ID3D11Device* device = m_owner->m_device;
	ID3D11RenderTargetView* rtv = nullptr;
	device->CreateRenderTargetView( m_handle, nullptr, &rtv);

	if( nullptr != rtv )
	{
		m_renderTargetView = new TextureView();
		m_renderTargetView->m_rtv = rtv;
	}

	return m_renderTargetView;
}

