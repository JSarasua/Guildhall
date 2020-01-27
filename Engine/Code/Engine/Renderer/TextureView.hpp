#pragma once

struct ID3D11RenderTargetView;

class TextureView
{
public:
	TextureView()	{}
	~TextureView();

	ID3D11RenderTargetView* GetRTVHandle() const { return m_rtv; }

public:
	ID3D11RenderTargetView* m_rtv = nullptr;
};