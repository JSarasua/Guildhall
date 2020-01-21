#include "Engine/Renderer/Texture.hpp"
#include <string>


Texture::Texture()
{

}

Texture::Texture( int textureId, const IntVec2& texelSizeCoords, const char* filePath ) :
	m_textureId(textureId),
	m_texelSizeCoords(texelSizeCoords),
	m_filePath(filePath)
{}

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

}

