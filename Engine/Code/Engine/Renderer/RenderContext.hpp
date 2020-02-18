#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"

#include <vector>
#include <string>



struct AABB2;
class Sampler;
class BitmapFont;
class Window;
class SwapChain;
class Shader;
class RenderBuffer;
class VertexBuffer;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11BlendState;
struct IDXGIDebug;


struct FrameData
{
	float systemTime;
	float systemDeltaTime;

	float padding[2];
};

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
	ADDITIVE,
	SOLID
};

enum class eBufferSlot
{
	UBO_FRAME_SLOT = 0,
	UBO_CAMERA_SLOT = 1
};

#undef DrawText
class RenderContext
{
public:

	void StartUp(Window* window);
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void UpdateFrameTime( float deltaSeconds );

	void ClearScreen( const Rgba8& clearColor );
	void BeginCamera( Camera& camera );
	void BeginCamera( const Camera& camera, Viewport viewPort );
	void EndCamera( const Camera& camera );

	void Draw( int numVertexes, int vertexOffset = 0 );

	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray( const std::vector<Vertex_PCU>& vertexes);

	void AppendVerts( std::vector<Vertex_PCU>& masterVertexList, std::vector<Vertex_PCU>& vertsToAppend);
	void AppendVertsFromAABB2( std::vector<Vertex_PCU>& masterVertexList, const AABB2& aabb, const Rgba8& tint, const Vec2& uvMins, const Vec2& uvMaxs );

	void BindShader( Shader* shader );
	void BindShader( char const* filename );
	void BindVertexBuffer( VertexBuffer* vbo );

	void BindUniformBuffer( unsigned int slot, RenderBuffer* ubo ); // ubo - uniform buffer object

	Texture* CreateTextureFromColor( Rgba8 color );
	//Texture* CreateTextureFromImage(...);
	Texture*	CreateOrGetTextureFromFile(const char* filePath);
	Shader*		GetOrCreateShader( char const* filename );
	void BindTexture( const Texture* constTex  );
	void BindSampler( Sampler const* constSampler );
	void SetBlendMode( BlendMode blendMode );
	void CreateBlendModes();
	bool IsDrawing() const;

	void DrawLine( const Vec2& startPoint, const Vec2& endPoint, const Rgba8& color, float thickness );
	void DrawRing( const Vec2& center, float radius, const Rgba8& color, float thickness );
	void DrawAABB2( const AABB2& aabb, const Rgba8& color, float thickness );
	void DrawAABB2Filled( const AABB2& aabb, const Rgba8& color);
	void DrawAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax);
	void DrawRotatedAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax, float orientationDegrees);

	BitmapFont* CreateOrGetBitmapFont( const char* bitmapFontFilePathNoExtension );
	void DrawTextAtPosition( const char* textstring, const Vec2& textMins, float fontHeight, const Rgba8& tint = Rgba8::WHITE );
	void DrawAlignedTextAtPosition( const char* textstring, const AABB2& box, float fontHeight, const Vec2& alignment, const Rgba8& tint = Rgba8::WHITE );

	void CreateDebugModule();
	void DestroyDebugModule();
	void ReportLiveObjects();

protected:

	Texture* CreateTextureFromFile(const char* filePath);
	BitmapFont* CreateBitMapFontFromFile( const char* filePath );
	void CreateBlendStates();

private:
	std::vector<Texture*> m_Textures;
	std::vector<BitmapFont*> m_fonts;

	bool m_isDrawing = false;

public:
	ID3D11Device* m_device;			//Our GPU
	ID3D11DeviceContext* m_context; //How we issue commands (immediate context)
	SwapChain*			m_swapchain = nullptr;

	Shader* m_currentShader = nullptr;
	Shader* m_defaultShader = nullptr;
	std::vector<Shader*> m_shaders;
	VertexBuffer* m_immediateVBO;
	ID3D11Buffer* m_lastVBOHandle = nullptr;

	Sampler* m_sampPoint = nullptr;
	Texture* m_texWhite = nullptr;

	RenderBuffer* m_frameUBO = nullptr;

	ID3D11BlendState* m_alphaBlendStateHandle = nullptr;
	ID3D11BlendState* m_additiveBlendStateHandle = nullptr;
	ID3D11BlendState* m_solidBlendStateHandle = nullptr;

	void* m_debugModule        = nullptr;
	IDXGIDebug* m_debug           = nullptr;
};



