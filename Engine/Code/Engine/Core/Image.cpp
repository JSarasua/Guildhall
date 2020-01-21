#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "ThirdParty/stb_image.h"

Image::Image( const char* imageFilePath ) :
	m_imageFilePath(imageFilePath)
{
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)


									// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );

	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ) );
	GUARANTEE_OR_DIE( numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );



	m_dimensions = IntVec2(imageTexelSizeX,imageTexelSizeY);
	//loop to create Rgba8 texel vector
	if( numComponents == 3 )
	{
		for( int y = 0; y < m_dimensions.y; y++ )
		{
			for( int x = 0; x < m_dimensions.x*numComponents; x+=3 )
			{
				int currentIndex = y * m_dimensions.x*numComponents + x;
				unsigned char r = imageData[currentIndex];
				unsigned char g = imageData[currentIndex+1];
				unsigned char b = imageData[currentIndex+2];
				Rgba8 rgbaTexel(r,g,b);
				m_rgbaTexels.push_back(rgbaTexel);
			}
		}
	}
	else if( numComponents == 4 )
	{
		for( int y = 0; y < m_dimensions.y; y++ )
		{
			for( int x = 0; x < m_dimensions.x*numComponents; x+=4 )
			{
				int currentIndex = y * m_dimensions.x*numComponents + x;
				unsigned char r = imageData[currentIndex];
				unsigned char g = imageData[currentIndex+1];
				unsigned char b = imageData[currentIndex+2];
				unsigned char a = imageData[currentIndex+3];
				Rgba8 rgbaTexel( r, g, b, a );
				m_rgbaTexels.push_back( rgbaTexel );
			}
		}
	}


}

const std::string& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}

IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}

Rgba8 Image::GetTexelColor( int texelX, int texelY ) const
{
	int texelIndex = texelY * m_dimensions.x + texelX;
	return m_rgbaTexels[texelIndex];
}

Rgba8 Image::GetTexelColor( const IntVec2& texelCoords ) const
{
	int texelIndex = texelCoords.y * m_dimensions.x + texelCoords.x;
	return m_rgbaTexels[texelIndex];
}

void Image::SetTexelColor( const Rgba8& color, const IntVec2& texelCoords )
{
	int texelIndex = texelCoords.y * m_dimensions.x + texelCoords.x;
	m_rgbaTexels[texelIndex] = color;
}