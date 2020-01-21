#pragma once
#include <vector>
#include <string>
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Texture;
struct Vertex_PCU;
struct Vec2;
struct AABB2;


class BitmapFont
{
	friend class RenderContext; //Only RenderContext creates new BitmapFont objects

private:
	BitmapFont( const char* fontName, const Texture* fontTexture );


public:
	const Texture* GetTexture() const;
	const std::string& GetFontName() const;

	void AddVertsForText2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins, float cellHeight, 
							const std::string& text, const Rgba8& tint = Rgba8::WHITE, float cellAspect = 1.f );

	void AddVertsForTextInBox2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, float cellHeight, 
		const std::string& text, const Rgba8& tint = Rgba8::WHITE, float cellAspect=1.f,
		const Vec2& alignment=ALIGN_CENTERED );

	Vec2 GetDimensionsForText2D( float cellHeight, const std::string& text, float cellAspect=1.f );

protected:
	float GetGlyphAspect( int glyphUnicode ) const; //For now returns 1.f

protected:
	std::string m_fontName;
	SpriteSheet m_glyphSpriteSheet;
};