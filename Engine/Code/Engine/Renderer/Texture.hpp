#pragma once
#include "Engine/Math/IntVec2.hpp"

class Texture
{
public:
	Texture();
	~Texture();

	Texture( int textureId, const IntVec2& texelSizeCoords, const char* filePath);

	int GetTextureID() const;
	const IntVec2& GetTexelSize() const;
	bool IsFileInTexture( const char* filePath );
private:
	int m_textureId = 0;
	const char* m_filePath = nullptr;
	IntVec2 m_texelSizeCoords;

};