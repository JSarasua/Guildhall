#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>
#include <vector>

class Image
{
public:
	Image( const char* imageFilePath );
	const std::string&	GetImageFilePath() const;
	IntVec2				GetDimensions() const;
	Rgba8				GetTexelColor( int texelX, int texelY ) const;
	Rgba8				GetTexelColor( const IntVec2& texelCoords ) const;

	void				SetTexelColor( const Rgba8& color, const IntVec2& texelCoords );


private:
	std::string			m_imageFilePath;
	IntVec2				m_dimensions = IntVec2(0,0);
	std::vector<Rgba8>	m_rgbaTexels;
};
