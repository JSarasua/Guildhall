#pragma once
#include "Engine/Math/IntVec2.hpp"

class RenderContext;
struct ID3D11Texture2D;
class TextureView;

class Texture
{
public:
	Texture();
	Texture( int textureId, const IntVec2& texelSizeCoords, const char* filePath);
	Texture(RenderContext* ctx, ID3D11Texture2D* handle );
	
	~Texture();

	TextureView* GetRenderTargetView();

	int GetTextureID() const;
	const IntVec2& GetTexelSize() const;
	bool IsFileInTexture( const char* filePath );

private:
	RenderContext* m_owner		= nullptr;
	ID3D11Texture2D* m_handle	= nullptr;

	int m_textureId = 0;
	const char* m_filePath = nullptr;
	IntVec2 m_texelSizeCoords;

	TextureView* m_renderTargetView = nullptr;
};