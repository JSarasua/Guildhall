#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

TextureView::~TextureView()
{
	DX_SAFE_RELEASE(m_rtv);
}

