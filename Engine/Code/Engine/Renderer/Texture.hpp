#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <string>

class RenderContext;
struct ID3D11Texture2D;
class TextureView;

class Texture
{
public:
	Texture();
	Texture( RenderContext* ctx, ID3D11Texture2D* handle, const char* filePath);
	Texture(RenderContext* ctx, ID3D11Texture2D* handle );
	
	~Texture();

	TextureView* GetRenderTargetView();
	TextureView* GetOrCreateShaderResourceView();


	int GetTextureID() const;
	const IntVec2& GetTexelSize() const;
	bool IsFileInTexture( std::string filePath );

private:
	RenderContext* m_owner		= nullptr;
	ID3D11Texture2D* m_handle	= nullptr;

	int m_textureId = 0;
	std::string m_filePath;
	IntVec2 m_texelSizeCoords;

	TextureView* m_renderTargetView = nullptr;
	TextureView* m_shaderResourceView = nullptr;
};