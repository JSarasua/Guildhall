#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Texture.hpp"
#include <vector>
#include <string>


class Sampler;
class BitmapFont;
class Polygon2D;
class Window;
class SwapChain;
class Shader;
class RenderBuffer;
class VertexBuffer;
class IndexBuffer;
class GPUMesh;
class Clock;

struct AABB2;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11BlendState;
struct IDXGIDebug;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;

struct FrameData
{
	float systemTime;
	float systemDeltaTime;

	float padding[2];
};

struct ModelData
{
	Mat44 model;
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

enum class eDepthCompareMode
{
	COMPARE_LESS_THAN_OR_EQUAL,
	COMPARE_GREATER_THAN,
	COMPARE_ALWAYS
};

enum class eDepthWriteMode
{
	WRITE_ALL,
	WRITE_NONE
};


enum class eCullMode
{
	CULL_BACK,
	CULL_FRONT,
	CULL_NONE
};

enum class eFillMode
{
	FILL_WIREFRAME,
	FILL_SOLID
};

enum class eFrontFaceWindOrder
{
	COUNTERCLOCKWISE,
	CLOCKWISE
};

#undef DrawText
class RenderContext
{
public:

	void StartUp(Window* window);
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void Setup( Clock* gameClock );
	void UpdateFrameTime();

	void ClearScreen( const Rgba8& clearColor );
	void ClearDepth( Texture* depthStencilTarget, float depth = 1.f, float stencil = 0.f );
	void BeginCamera( Camera& camera );
	void EndCamera( const Camera& camera );

	void Draw( int numVertexes, int vertexOffset = 0 );
	void DrawIndexed( int numIndices, int indexOffset = 0, int vertexOffset = 0 );
	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray( const std::vector<Vertex_PCU>& vertexes);
	void DrawIndexedVertexArray( std::vector<Vertex_PCU> const& vertexes, std::vector<uint> const& indices );
	void DrawMesh( GPUMesh* mesh );

	void AppendVerts( std::vector<Vertex_PCU>& masterVertexList, std::vector<Vertex_PCU>& vertsToAppend);
	void AppendVertsFromAABB2( std::vector<Vertex_PCU>& masterVertexList, const AABB2& aabb, const Rgba8& tint, const Vec2& uvMins, const Vec2& uvMaxs );

	void BindShader( Shader* shader );
	void BindShader( char const* filename );
	void BindVertexBuffer( VertexBuffer* vbo );
	void BindIndexBuffer( IndexBuffer* ibo );
	void BindUniformBuffer( unsigned int slot, RenderBuffer* ubo ); // ubo - uniform buffer object

	void SetDepth( eDepthCompareMode compareMode, eDepthWriteMode writeMode = eDepthWriteMode::WRITE_ALL );
	void SetModelMatrix( Mat44 const& model );

	Texture* GetBackBuffer();

	Texture* CreateDepthStencilTarget();
	Texture* CreateTextureFromColor( Rgba8 color, IntVec2 texelSize = IntVec2( 1, 1 ) );
	//Texture* CreateTextureFromImage(...);
	Texture*	CreateOrGetTextureFromFile(const char* filePath);
	Shader*		GetOrCreateShader( char const* filename );
	BitmapFont* CreateOrGetBitmapFont( const char* bitmapFontFilePathNoExtension );
	void BindTexture( const Texture* constTex  );
	void BindSampler( Sampler const* constSampler );
	void SetBlendMode( BlendMode blendMode );
	void CreateBlendModes();
	bool IsDrawing() const;

	//Debug Draw methods that will be replaced
	void DrawLine( const Vec2& startPoint, const Vec2& endPoint, const Rgba8& color, float thickness );
	void DrawRing( const Vec2& center, float radius, const Rgba8& color, float thickness );
	void DrawDisc( Vec2 const& center, float radius, Rgba8 const& fillColor, Rgba8 const& borderColor, float thickness );
	void DrawAABB2( const AABB2& aabb, const Rgba8& color, float thickness );
	void DrawAABB2Filled( const AABB2& aabb, const Rgba8& color, float z = 0.f );
	void DrawAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax);
	void DrawRotatedAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax, float orientationDegrees);
	void DrawPolygon2D( Polygon2D const& polygon, Rgba8 const& fillColor, Rgba8 const& borderColor, float thickness );

	//Draw text
	void DrawTextAtPosition( const char* textstring, const Vec2& textMins, float fontHeight, const Rgba8& tint = Rgba8::WHITE );
	void DrawAlignedTextAtPosition( const char* textstring, const AABB2& box, float fontHeight, const Vec2& alignment, const Rgba8& tint = Rgba8::WHITE );

	void CreateDefaultRasterState();
	void SetCullMode( eCullMode cullMode ); //back, front, mode, none
	void SetFillMode( eFillMode fillMode ); //fill, solid, wire mode
	void SetFrontFaceWindOrder( eFrontFaceWindOrder windOrder ); //counterclockwise, clockwise

	void CreateDebugModule();
	void DestroyDebugModule();
	void ReportLiveObjects();

protected:
	Texture* CreateTextureFromFile(const char* filePath);
	BitmapFont* CreateBitMapFontFromFile( const char* filePath );
	void CreateBlendStates();

private:
	std::vector<Texture*> m_Textures;

	bool m_isDrawing = false;

public:
	std::vector<BitmapFont*> m_fonts;
	Clock* m_gameClock = nullptr;

	ID3D11Device* m_device;			//Our GPU
	ID3D11DeviceContext* m_context; //How we issue commands (immediate context)
	SwapChain*			m_swapchain = nullptr;

	Shader* m_currentShader = nullptr;
	Shader* m_defaultShader = nullptr;
	std::vector<Shader*> m_shaders;
	VertexBuffer* m_immediateVBO = nullptr;
	IndexBuffer* m_immediateIBO = nullptr;
	ID3D11Buffer* m_lastVBOHandle = nullptr;

	Sampler* m_sampPoint = nullptr;
	Texture* m_texWhite = nullptr;

	RenderBuffer* m_frameUBO = nullptr;
	RenderBuffer* m_modelUBO = nullptr;

	ID3D11BlendState* m_alphaBlendStateHandle = nullptr;
	ID3D11BlendState* m_additiveBlendStateHandle = nullptr;
	ID3D11BlendState* m_solidBlendStateHandle = nullptr;

	ID3D11DepthStencilState* m_depthStencilState = nullptr;

	void* m_debugModule        = nullptr;
	IDXGIDebug* m_debug           = nullptr;

	ID3D11RasterizerState* m_rasterState = nullptr;
};



