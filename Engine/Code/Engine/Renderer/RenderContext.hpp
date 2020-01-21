#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include <vector>
#include <string>

struct AABB2;
class BitmapFont;

enum class Viewport
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	FullScreen
};

enum class BlendMode
{
	ALPHA,
	ADDITIVE
};

#undef DrawText
class RenderContext
{
public:

	void StartUp();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void ClearScreen( const Rgba8& clearColor );
	void BeginCamera( const Camera& camera );
	void BeginCamera( const Camera& camera, Viewport viewPort );
	void EndCamera( const Camera& camera );
	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes ) const;
	void DrawVertexArray( const std::vector<Vertex_PCU>& vertexes) const;

	void AppendVerts( std::vector<Vertex_PCU>& masterVertexList, std::vector<Vertex_PCU>& vertsToAppend);
	void AppendVertsFromAABB2( std::vector<Vertex_PCU>& masterVertexList, const AABB2& aabb, const Rgba8& tint, const Vec2& uvMins, const Vec2& uvMaxs );

	Texture* CreateOrGetTextureFromFile(const char* filePath);
	void BindTexture( const Texture* texture ) const;
	void SetBlendMode( BlendMode blendMode );


	void DrawLine( const Vec2& startPoint, const Vec2& endPoint, const Rgba8& color, float thickness ) const;
	void DrawRing( const Vec2& center, float radius, const Rgba8& color, float thickness ) const;
	void DrawAABB2( const AABB2& aabb, const Rgba8& color, float thickness ) const;
	void DrawAABB2Filled( const AABB2& aabb, const Rgba8& color) const;
	void DrawAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax) const;
	void DrawRotatedAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax, float orientationDegrees) const;

	BitmapFont* CreateOrGetBitmapFont( const char* bitmapFontFilePathNoExtension );
	void DrawTextAtPosition( const char* textstring, const Vec2& textMins, float fontHeight, const Rgba8& tint = Rgba8::WHITE ) const;
	void DrawAlignedTextAtPosition( const char* textstring, const AABB2& box, float fontHeight, const Vec2& alignment, const Rgba8& tint = Rgba8::WHITE ) const;

protected:

	Texture* CreateTextureFromFile(const char* filePath);
	BitmapFont* CreateBitMapFontFromFile( const char* filePath );

private:
	std::vector<Texture*> m_Textures;
	std::vector<BitmapFont*> m_fonts;

};