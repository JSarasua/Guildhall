#include "BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
//#include "Engine/Math/vec2.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"



BitmapFont::BitmapFont( const char* fontName, const Texture* fontTexture ) : 
	m_fontName(fontName),
	m_glyphSpriteSheet(*fontTexture,IntVec2(16,16))
{

}

const Texture* BitmapFont::GetTexture() const
{
	return &m_glyphSpriteSheet.GetTexture();
}

const std::string& BitmapFont::GetFontName() const
{
	return m_fontName;
}

void BitmapFont::AddVertsForText2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins, float cellHeight, 
									const std::string& text, const Rgba8& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f */ )
{
	//Loop through each character, create an aabb2 for each.
	for( int charIndex = 0; charIndex < text.size(); charIndex++ )
	{
		char character = text[charIndex];
		float cellWidth = cellAspect * cellHeight;
		Vec2 textMaxs(cellWidth, cellHeight);
		textMaxs += textMins;

		AABB2 charAABB(textMins, textMaxs);
		charAABB.Translate(Vec2(static_cast<float>(charIndex) * cellWidth, 0.f));

		Vec2 uvMin( 0.f, 0.f );
		Vec2 uvMax( 1.f, 1.f );
		m_glyphSpriteSheet.GetSpriteUVs(uvMin, uvMax, character);


		vertexArray.push_back(Vertex_PCU(Vec3(charAABB.mins),tint, uvMin));
		vertexArray.push_back(Vertex_PCU(Vec3(charAABB.maxs.x, charAABB.mins.y, 0.f),tint, Vec2(uvMax.x,uvMin.y)));
		vertexArray.push_back(Vertex_PCU(Vec3(charAABB.maxs),tint, uvMax));

		vertexArray.push_back(Vertex_PCU(Vec3(charAABB.mins),tint, uvMin));
		vertexArray.push_back(Vertex_PCU(Vec3(charAABB.maxs),tint, uvMax));
		vertexArray.push_back(Vertex_PCU(Vec3(charAABB.mins.x, charAABB.maxs.y, 0.f),tint, Vec2(uvMin.x, uvMax.y)));

	}
}

void BitmapFont::AddVertsForTextInBox2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, float cellHeight, const std::string& text, 
	const Rgba8& tint /*= Rgba8::WHITE*/, float cellAspect/*=1.f*/, const Vec2& alignment/*=ALIGN_CENTERED */ )
{
	float halfTextWidth = cellAspect * cellHeight * (text.length()*0.5f);
	float halfTextHeight = cellHeight * 0.5f;

	Vec2 halfTextDimensions(halfTextWidth,halfTextHeight);

	AABB2 innerBox = box;
	innerBox.mins += halfTextDimensions;
	innerBox.maxs -= halfTextDimensions;
	
	float centerOfTextX = Interpolate(innerBox.mins.x,innerBox.maxs.x, alignment.x);
	float centerOfTextY = Interpolate(innerBox.mins.y, innerBox.maxs.y, alignment.y);

	float textMinsX = centerOfTextX - halfTextWidth;
	float textMinsY = centerOfTextY - halfTextHeight;

	Vec2 textMins(textMinsX,textMinsY);

	//Loop through each character, create an aabb2 for each.
	for( int charIndex = 0; charIndex < text.size(); charIndex++ )
	{
		char character = text[charIndex];
		float cellWidth = cellAspect * cellHeight;
		Vec2 textMaxs( cellWidth, cellHeight );
		textMaxs += textMins;

		AABB2 charAABB( textMins, textMaxs );
		charAABB.Translate( Vec2( static_cast<float>(charIndex)* cellWidth, 0.f ) );

		Vec2 uvMin( 0.f, 0.f );
		Vec2 uvMax( 1.f, 1.f );
		m_glyphSpriteSheet.GetSpriteUVs( uvMin, uvMax, character );


		vertexArray.push_back( Vertex_PCU( Vec3( charAABB.mins ), tint, uvMin ) );
		vertexArray.push_back( Vertex_PCU( Vec3( charAABB.maxs.x, charAABB.mins.y, 0.f ), tint, Vec2( uvMax.x, uvMin.y ) ) );
		vertexArray.push_back( Vertex_PCU( Vec3( charAABB.maxs ), tint, uvMax ) );

		vertexArray.push_back( Vertex_PCU( Vec3( charAABB.mins ), tint, uvMin ) );
		vertexArray.push_back( Vertex_PCU( Vec3( charAABB.maxs ), tint, uvMax ) );
		vertexArray.push_back( Vertex_PCU( Vec3( charAABB.mins.x, charAABB.maxs.y, 0.f ), tint, Vec2( uvMin.x, uvMax.y ) ) );

	}
}

Vec2 BitmapFont::GetDimensionsForText2D( float cellHeight, const std::string& text, float cellAspect/*=1.f */ )
{
	float cellWidth = cellAspect * cellHeight;
	float stringWidth = cellWidth * text.length();
	Vec2 dimensions(stringWidth, cellHeight);

	return dimensions;
}

float BitmapFont::GetGlyphAspect( int glyphUnicode ) const
{
	UNUSED(glyphUnicode);
	//Will change later
	return 1.f;
}

