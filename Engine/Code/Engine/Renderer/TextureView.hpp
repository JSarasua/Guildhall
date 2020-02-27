#pragma once

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Resource;

class TextureView
{
public:
	TextureView();
	~TextureView();

	ID3D11RenderTargetView* GetAsRTV() const { return m_rtv; };
	ID3D11ShaderResourceView* GetAsSRV() const { return m_srv; };

public:
	union
	{
		ID3D11Resource* m_handle;
		ID3D11RenderTargetView* m_rtv;
		ID3D11ShaderResourceView* m_srv;
	};
};