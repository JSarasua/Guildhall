#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"

#include <vector>
#include <string>



struct AABB2;
class BitmapFont;
class Window;
class SwapChain;
class Shader;
class RenderBuffer;
class VertexBuffer;

struct ID3D11Device;
struct ID3D11DeviceContext;

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

	void StartUp(Window* window);
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void ClearScreen( const Rgba8& clearColor );
	void BeginCamera( const Camera& camera );
	void BeginCamera( const Camera& camera, Viewport viewPort );
	void EndCamera( const Camera& camera );

	void Draw( int numVertexes, int vertexOffset = 0 );

	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray( const std::vector<Vertex_PCU>& vertexes);

	void AppendVerts( std::vector<Vertex_PCU>& masterVertexList, std::vector<Vertex_PCU>& vertsToAppend);
	void AppendVertsFromAABB2( std::vector<Vertex_PCU>& masterVertexList, const AABB2& aabb, const Rgba8& tint, const Vec2& uvMins, const Vec2& uvMaxs );

	void BindShader( Shader* shader );
	void BindVertexInput( VertexBuffer* vbo );

	Texture* CreateOrGetTextureFromFile(const char* filePath);
	void BindTexture( const Texture* texture ) const;
	void SetBlendMode( BlendMode blendMode );


	void DrawLine( const Vec2& startPoint, const Vec2& endPoint, const Rgba8& color, float thickness );
	void DrawRing( const Vec2& center, float radius, const Rgba8& color, float thickness );
	void DrawAABB2( const AABB2& aabb, const Rgba8& color, float thickness );
	void DrawAABB2Filled( const AABB2& aabb, const Rgba8& color);
	void DrawAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax);
	void DrawRotatedAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax, float orientationDegrees);

	BitmapFont* CreateOrGetBitmapFont( const char* bitmapFontFilePathNoExtension );
	void DrawTextAtPosition( const char* textstring, const Vec2& textMins, float fontHeight, const Rgba8& tint = Rgba8::WHITE );
	void DrawAlignedTextAtPosition( const char* textstring, const AABB2& box, float fontHeight, const Vec2& alignment, const Rgba8& tint = Rgba8::WHITE );

protected:

	Texture* CreateTextureFromFile(const char* filePath);
	BitmapFont* CreateBitMapFontFromFile( const char* filePath );

private:
	std::vector<Texture*> m_Textures;
	std::vector<BitmapFont*> m_fonts;

public:
	ID3D11Device* m_device;			//Our GPU
	ID3D11DeviceContext* m_context; //How we issue commands (immediate context)
	SwapChain*			m_swapchain = nullptr;

	Shader* m_currentShader = nullptr;
	Shader* m_defaultShader = nullptr;
	VertexBuffer* m_immediateVBO;
};

