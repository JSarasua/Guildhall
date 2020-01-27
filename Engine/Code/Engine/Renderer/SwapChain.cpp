#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

SwapChain::SwapChain( RenderContext* owner, IDXGISwapChain* handle ) 
	: m_owner(owner),
	m_handle(handle)
{
}

SwapChain::~SwapChain()
{
	delete m_backbuffer;
	m_backbuffer = nullptr;

	m_owner = nullptr;
	DX_SAFE_RELEASE(m_handle);
}

void SwapChain::Present( int vsync )
{
	m_handle->Present( vsync, 0 );
}

Texture* SwapChain::GetBackBuffer()
{
	if( nullptr != m_backbuffer )
	{
		return m_backbuffer;
	}

	// first, we request the D3D11 handle of the textures owned by the swapbuffer
	ID3D11Texture2D* texHandle = nullptr;
	m_handle->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)&texHandle );

	ASSERT_OR_DIE(texHandle, "TexHandle not set");

	m_backbuffer = new Texture( m_owner, texHandle );

	return m_backbuffer;
}
