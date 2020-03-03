#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include <string>


Texture::Texture()
{
	DX_SAFE_RELEASE( m_handle );
}

Texture::Texture( RenderContext* context, ID3D11Texture2D* handle )
	: m_owner( context ),
	m_handle( handle )
{
	D3D11_TEXTURE2D_DESC desc; //description
	handle->GetDesc(&desc);
	m_texelSizeCoords.x = desc.Width;
	m_texelSizeCoords.y = desc.Height;
}

Texture::Texture( RenderContext* ctx, ID3D11Texture2D* handle, const char* filePath )
	: Texture(ctx, handle)
{
	if( nullptr != filePath )
	{
		m_filePath = filePath;
	}
}

int Texture::GetTextureID() const
{
	return m_textureId;
}

const IntVec2& Texture::GetTexelSize() const
{
	return m_texelSizeCoords;
}

bool Texture::IsFileInTexture( std::string filePath )
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

	delete m_shaderResourceView;
	m_shaderResourceView = nullptr;

	delete m_depthStencilView;
	m_depthStencilView = nullptr;

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

TextureView* Texture::GetOrCreateShaderResourceView()
{
	if( nullptr != m_shaderResourceView )
	{
		return m_shaderResourceView;
	}

	ID3D11Device* dev = m_owner->m_device;
	ID3D11ShaderResourceView* srv = nullptr;

	dev->CreateShaderResourceView( m_handle, nullptr, &srv );

	if( nullptr != srv )
	{
		m_shaderResourceView = new TextureView();
		m_shaderResourceView->m_srv = srv;
	}

	return m_shaderResourceView;
}

TextureView* Texture::GetOrCreateDepthStencilView()
{
	if( nullptr != m_depthStencilView )
	{
		return m_depthStencilView;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	ID3D11DepthStencilView* dsv = nullptr;
	m_owner->m_device->CreateDepthStencilView( m_handle, &descDSV, &dsv );

	if( nullptr != dsv )
	{
		m_depthStencilView = new TextureView();
		m_depthStencilView->m_dsv = dsv;
	}

	return m_depthStencilView;
}

