#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>

static RenderContext*	s_DebugRenderContext = nullptr;
static Camera*			s_DebugCamera = nullptr;

struct DebugRenderObject
{
public:
	void UpdateColors();
	void AppendVerts( std::vector<Vertex_PCU>& vertexList, Mat44 const& cameraView );
	void AppendIndexedVerts( std::vector<Vertex_PCU>& vertexList, std::vector<uint>& indexList, Mat44 const& cameraView, Rgba8 const& tint = Rgba8::WHITE, float tintStrength = 0.f );

public:
	Timer m_timer;

	Rgba8 m_startColor = Rgba8::WHITE;
	Rgba8 m_endColor = Rgba8::WHITE;
	float m_duration = 0.f;
	eDebugRenderMode m_mode = DEBUG_RENDER_USE_DEPTH;
	eDebugRenderTo m_renderTo = DEBUG_RENDER_TO_WORLD;
	Texture const* m_texture = nullptr;
	/*GPUMesh* m_mesh = nullptr;*/
	Vec3 m_pivotDim;
	Mat44 m_modelMatrix;
	std::vector<Vertex_PCU> m_vertices;
	std::vector<uint> m_indices;
	bool m_isText = false;
	bool m_isBillBoarded = false;
};

void DebugRenderObject::UpdateColors()
{
	float startR	= (float)m_startColor.r;
	float startG	= (float)m_startColor.g;
	float startB	= (float)m_startColor.b;
	float startA	= (float)m_startColor.a;
	float endR		= (float)m_endColor.r;
	float endG		= (float)m_endColor.g;
	float endB		= (float)m_endColor.b;
	float endA		= (float)m_endColor.a;

	float duration = (float)m_timer.GetElapsedSeconds();
	float totalTime = duration + (float)m_timer.GetSecondsRemaining();
	float lerpValue = 1.f;

	if( totalTime != 0.f )
	{
		lerpValue = duration / totalTime;
	}

	unsigned char lerpR = (unsigned char)Interpolate( startR, endR, lerpValue );
	unsigned char lerpG = (unsigned char)Interpolate( startG, endG, lerpValue );
	unsigned char lerpB = (unsigned char)Interpolate( startB, endB, lerpValue );
	unsigned char lerpA = (unsigned char)Interpolate( startA, endA, lerpValue );

	Rgba8 color = Rgba8( lerpR, lerpG, lerpB, lerpA );

	for( size_t vertIndex = 0; vertIndex < m_vertices.size(); vertIndex++ )
	{
		m_vertices[vertIndex].tint = color;
	}
}

void DebugRenderObject::AppendVerts( std::vector<Vertex_PCU>& vertexList, Mat44 const& cameraView )
{
	//Mat44 transformMatrix = m_transform.ToMatrix();
	Mat44 transformMatrix = m_modelMatrix;
	if( m_isBillBoarded )
	{
		Mat44 pivotTransform = Mat44::CreateTranslation3D( -m_pivotDim );

		transformMatrix.TransformBy( cameraView ); 	//Camera doesn't need to be reversed because camera is facing -Z but our append verts are already assuming drawing +Z
		transformMatrix.TransformBy( pivotTransform );
	}

	for( size_t vertexIndex = 0; vertexIndex < m_vertices.size(); vertexIndex++ )
	{
		Vec3 vertex = m_vertices[vertexIndex].position;
		Rgba8 const& tint = m_vertices[vertexIndex].tint;
		Vec2 const& uv = m_vertices[vertexIndex].uvTexCoords;

		vertex = transformMatrix.TransformPosition3D( vertex );

		Vertex_PCU transformedVert = Vertex_PCU( vertex, tint, uv );

		vertexList.push_back( transformedVert );
	}
}

void DebugRenderObject::AppendIndexedVerts( std::vector<Vertex_PCU>& vertexList, std::vector<uint>& indexList, Mat44 const& cameraView, Rgba8 const& tint, float tintStrength )
{
	uint currentVertexListEnd = (uint)vertexList.size();

	//Mat44 transformMatrix = m_transform.ToMatrix();
	Mat44 transformMatrix = m_modelMatrix;

	if( m_isBillBoarded )
	{
		//Mat44 pivotTransform = Mat44::CreateTranslation3D( -m_pivotDim );

		transformMatrix.TransformBy( cameraView ); 	//Camera doesn't need to be reversed because camera is facing -Z but our append verts are already assuming drawing +Z
		//transformMatrix.TransformBy( pivotTransform );
	}
	Mat44 pivotTransform = Mat44::CreateTranslation3D( -m_pivotDim );
	transformMatrix.TransformBy( pivotTransform );

	for( size_t vertexIndex = 0; vertexIndex < m_vertices.size(); vertexIndex++ )
	{
		Vec3 vertex = m_vertices[vertexIndex].position;
		Rgba8 const& color = m_vertices[vertexIndex].tint;
		Vec2 const& uv = m_vertices[vertexIndex].uvTexCoords;

		Rgba8 tintedColor = Rgba8::LerpColorTo( color, tint, tintStrength );

		vertex = transformMatrix.TransformPosition3D( vertex );

		Vertex_PCU transformedVert = Vertex_PCU( vertex, tintedColor, uv );

		vertexList.push_back( transformedVert );
	}

	for( size_t indexIndex = 0; indexIndex < m_indices.size(); indexIndex++ )
	{
		indexList.push_back( currentVertexListEnd + (uint)m_indices[indexIndex] );
	}
}

class DebugRenderSystem
{
public:
	void UpdateColors();
	void AppendVerts( std::vector<Vertex_PCU>& vertexList, Mat44 const& cameraView, eDebugRenderTo renderTo );
	void AppendTextVerts( std::vector<Vertex_PCU>& vertexList, Mat44 const& cameraView, eDebugRenderTo renderTo );
	void AppendIndexedVerts( std::vector<Vertex_PCU>& vertexList, std::vector<uint>& indexList, Mat44 const& cameraView, eDebugRenderTo renderTo, eDebugRenderMode mode );
	void AppendIndexedTextVerts( std::vector<Vertex_PCU>& vertexList, std::vector<uint>& indexList, Mat44 const& cameraView, eDebugRenderTo renderTo, eDebugRenderMode mode );

public:
	RenderContext* m_context;
	Texture const* m_fontText = nullptr;
	std::vector<DebugRenderObject*> m_renderObjects;
	bool m_isDebugRenderingEnabled = false;
	float m_screenHeight = 1080.f;
};

void DebugRenderSystem::UpdateColors()
{
	for( size_t debugObjectIndex = 0; debugObjectIndex < m_renderObjects.size(); debugObjectIndex++ )
	{
		DebugRenderObject* debugObject = m_renderObjects[debugObjectIndex];
		if( nullptr != debugObject )
		{
			debugObject->UpdateColors();
		}
	}
}

void DebugRenderSystem::AppendVerts( std::vector<Vertex_PCU>& vertexList, Mat44 const& cameraView, eDebugRenderTo renderTo )
{
	for( size_t debugObjectIndex = 0; debugObjectIndex < m_renderObjects.size(); debugObjectIndex++ )
	{
		DebugRenderObject* debugObject = m_renderObjects[debugObjectIndex];

		if( nullptr != debugObject && !debugObject->m_isText )
		{
			if( debugObject->m_renderTo == renderTo )
			{
				debugObject->AppendVerts( vertexList, cameraView );
			}
		}
	}
}

void DebugRenderSystem::AppendTextVerts( std::vector<Vertex_PCU>& vertexList, Mat44 const& cameraView, eDebugRenderTo renderTo )
{
	for( size_t debugObjectIndex = 0; debugObjectIndex < m_renderObjects.size(); debugObjectIndex++ )
	{
		DebugRenderObject* debugObject = m_renderObjects[debugObjectIndex];
		if( nullptr != debugObject && debugObject->m_isText )
		{
			if( debugObject->m_renderTo == renderTo )
			{
				debugObject->AppendVerts( vertexList, cameraView );
			}
		}
	}
}

void DebugRenderSystem::AppendIndexedVerts( std::vector<Vertex_PCU>& vertexList, std::vector<uint>& indexList, Mat44 const& cameraView, eDebugRenderTo renderTo, eDebugRenderMode mode )
{
	if( renderTo == DEBUG_RENDER_TO_WORLD )
	{

		for( size_t debugObjectIndex = 0; debugObjectIndex < m_renderObjects.size(); debugObjectIndex++ )
		{
			DebugRenderObject* debugObject = m_renderObjects[debugObjectIndex];

			if( nullptr != debugObject && !debugObject->m_isText )
			{
				if( debugObject->m_mode == mode || (debugObject->m_mode == DEBUG_RENDER_XRAY && mode == DEBUG_RENDER_USE_DEPTH) )
				{
					if( mode == DEBUG_RENDER_XRAY )
					{
						debugObject->AppendIndexedVerts( vertexList, indexList, cameraView, Rgba8::BLACK, 0.5f );
					}
					else
					{
						debugObject->AppendIndexedVerts( vertexList, indexList, cameraView );
					}
				}
			}
		}
	}
	else
	{
		for( size_t debugObjectIndex = 0; debugObjectIndex < m_renderObjects.size(); debugObjectIndex++ )
		{
			DebugRenderObject* debugObject = m_renderObjects[debugObjectIndex];

			if( nullptr != debugObject && !debugObject->m_isText )
			{
				if( debugObject->m_renderTo == DEBUG_RENDER_TO_SCREEN )
				{
					debugObject->AppendIndexedVerts( vertexList, indexList, cameraView );
				}
			}
		}
	}
}

void DebugRenderSystem::AppendIndexedTextVerts( std::vector<Vertex_PCU>& vertexList, std::vector<uint>& indexList, Mat44 const& cameraView, eDebugRenderTo renderTo, eDebugRenderMode mode )
{
	if( renderTo == DEBUG_RENDER_TO_WORLD )
	{

		for( size_t debugObjectIndex = 0; debugObjectIndex < m_renderObjects.size(); debugObjectIndex++ )
		{
			DebugRenderObject* debugObject = m_renderObjects[debugObjectIndex];

			if( nullptr != debugObject && debugObject->m_isText )
			{
				if( debugObject->m_mode == mode || (debugObject->m_mode == DEBUG_RENDER_XRAY && mode == DEBUG_RENDER_USE_DEPTH) )
				{
					if( mode == DEBUG_RENDER_XRAY )
					{
						debugObject->AppendIndexedVerts( vertexList, indexList, cameraView, Rgba8::BLACK, 0.5f );
					}
					else
					{
						debugObject->AppendIndexedVerts( vertexList, indexList, cameraView );
					}

				}
			}
		}
	}
	else
	{
		for( size_t debugObjectIndex = 0; debugObjectIndex < m_renderObjects.size(); debugObjectIndex++ )
		{
			DebugRenderObject* debugObject = m_renderObjects[debugObjectIndex];

			if( nullptr != debugObject && debugObject->m_isText )
			{
				if( debugObject->m_renderTo == DEBUG_RENDER_TO_SCREEN )
				{
					debugObject->AppendIndexedVerts( vertexList, indexList, cameraView );
				}
			}
		}
	}
}

static DebugRenderSystem* s_DebugRenderSystem = nullptr;

/************************************************************************/
/* Methods                                                              */
/************************************************************************/
void DebugRenderSystemStartup( RenderContext* context )
{
	s_DebugRenderSystem = new DebugRenderSystem();
	s_DebugRenderSystem->m_context = context;
	//s_DebugRenderSystem->m_fontText = context->m_fonts[0]->GetTexture();
}

void DebugRenderSystemShutdown()
{
	std::vector<DebugRenderObject*>& debugObjects = s_DebugRenderSystem->m_renderObjects;
	for( size_t renderObjectIndex = 0; renderObjectIndex < debugObjects.size(); renderObjectIndex++ )
	{
		delete debugObjects[renderObjectIndex];
		debugObjects[renderObjectIndex] = nullptr;
	}
	debugObjects.clear();
}

void EnableDebugRendering()
{
	s_DebugRenderSystem->m_isDebugRenderingEnabled = true;
}

void DisableDebugRendering()
{
	s_DebugRenderSystem->m_isDebugRenderingEnabled = false;
}

void ClearDebugRendering()
{
	s_DebugRenderSystem->m_renderObjects.clear();
}

void DebugRenderBeginFrame()
{
	//Update colors
}

void DebugRenderWorldToCamera( Camera* cam )
{
	if( !s_DebugRenderSystem->m_isDebugRenderingEnabled )
	{
		return;
	}

	RenderContext* context = s_DebugRenderSystem->m_context;
	std::vector<Vertex_PCU> useDepthVertices;
	std::vector<uint> useDepthIndices;
	std::vector<Vertex_PCU> useDepthTextVertices;
	std::vector<uint> useDepthTextIndices;

	std::vector<Vertex_PCU> XRayVertices;
	std::vector<uint> XRayIndices;
	std::vector<Vertex_PCU> XRayTextVertices;
	std::vector<uint> XRayTextIndices;

	//UpdateColors
	s_DebugRenderSystem->UpdateColors();
	//AppendVerts
	s_DebugRenderSystem->AppendIndexedVerts( useDepthVertices, useDepthIndices, cam->GetViewRotationMatrix(), DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_USE_DEPTH );
	s_DebugRenderSystem->AppendIndexedTextVerts( useDepthTextVertices, useDepthTextIndices, cam->GetViewRotationMatrix(), DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_USE_DEPTH );
	s_DebugRenderSystem->AppendIndexedVerts( XRayVertices, XRayIndices, cam->GetViewRotationMatrix(), DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_XRAY );
	s_DebugRenderSystem->AppendIndexedTextVerts( XRayTextVertices, XRayTextIndices, cam->GetViewRotationMatrix(), DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_XRAY );
	cam->m_clearMode = NO_CLEAR;

	context->BeginCamera( *cam );
	context->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL ); //Change to add if writing to depth buffer for xray mode
	context->SetBlendMode( BlendMode::ALPHA );
	context->BindTexture( nullptr );
	context->BindShader( (Shader*)nullptr );

	//Draw
	context->DrawIndexedVertexArray( useDepthVertices, useDepthIndices );

	context->SetDepth( eDepthCompareMode::COMPARE_GREATER_THAN, eDepthWriteMode::WRITE_NONE );
	context->DrawIndexedVertexArray( XRayVertices, XRayIndices );

	//Draw Text
	context->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL );
	Texture const* tex = context->m_fonts[0]->GetTexture();
	context->BindTexture( tex );
	context->DrawIndexedVertexArray( useDepthTextVertices, useDepthTextIndices );

	context->SetDepth( eDepthCompareMode::COMPARE_GREATER_THAN, eDepthWriteMode::WRITE_NONE );
	context->DrawIndexedVertexArray( XRayTextVertices, XRayTextIndices );

	context->EndCamera( *cam );
}

void DebugRenderScreenTo( Texture* output )
{
	if( !s_DebugRenderSystem->m_isDebugRenderingEnabled )
	{
		return;
	}
	s_DebugRenderSystem->UpdateColors();

	RenderContext* context = output->GetRenderContext();

	Camera cam;
	cam.SetColorTarget( output );
	cam.SetClearMode( NO_CLEAR, Rgba8::BLACK );

	Vec2 outputDimensions = Vec2(output->GetTexelSize());
	float aspectRatio = outputDimensions.x / outputDimensions.y;
	float screenHeight = s_DebugRenderSystem->m_screenHeight;

	Vec2 max( aspectRatio * screenHeight, screenHeight );
	cam.SetProjectionOrthographic(max, 0.f, 1.f );
	cam.SetPosition( Vec3(0.5f * max) );

	std::vector<Vertex_PCU> vertices;
	std::vector<Vertex_PCU> textVertices;

	s_DebugRenderSystem->AppendVerts( vertices, cam.GetViewRotationMatrix(), DEBUG_RENDER_TO_SCREEN );
	s_DebugRenderSystem->AppendTextVerts( textVertices, cam.GetViewRotationMatrix(), DEBUG_RENDER_TO_SCREEN );


	context->BeginCamera( cam );
	context->SetDepth( eDepthCompareMode::COMPARE_ALWAYS );
	context->SetBlendMode( BlendMode::ALPHA );
	context->BindTexture( nullptr );
	context->BindShader( (Shader*)nullptr );

	//Draw
	context->DrawVertexArray( vertices );

	//Draw Text
	Texture const* tex = context->m_fonts[0]->GetTexture();
	context->BindTexture( tex );
	context->DrawVertexArray( textVertices );

	context->EndCamera( cam );


}

void DebugRenderEndFrame()
{
	std::vector<DebugRenderObject*>& debugObjects = s_DebugRenderSystem->m_renderObjects;
	for( size_t debugIndex = 0; debugIndex < debugObjects.size(); debugIndex++ )
	{
		Timer& objTimer = debugObjects[debugIndex]->m_timer;
		if( nullptr != &objTimer )
		{
			if( objTimer.HasElapsed() )
			{
				delete debugObjects[debugIndex];
				debugObjects[debugIndex] = nullptr;
			}
		}

	}
}

void DebugAddWorldPoint( Vec3 const& pos, float size, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	float halfSize = 0.5f * size;
	AABB2 aabb = AABB2( -halfSize, -halfSize, halfSize, halfSize );
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_mode = mode;
	debugObject->m_renderTo = DEBUG_RENDER_TO_WORLD;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_modelMatrix = Mat44::CreateTranslation3D( pos );
	debugObject->m_isBillBoarded = true;
	debugObject->m_isText = false;
	Vertex_PCU::AppendVertsAABB2D(debugObject->m_vertices, aabb, startColor);
	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_indices.push_back((uint)vertIndex);
	}
	s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddWorldText( Mat44 const& basis, Vec2 pivot, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode, char const* text )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_mode = mode;
	debugObject->m_renderTo = DEBUG_RENDER_TO_WORLD;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_isText = true;
	debugObject->m_isBillBoarded = false;

	std::string strText( text );
	RenderContext* context = s_DebugRenderSystem->m_context;
	Vec2 textDimensions = context->m_fonts[0]->GetDimensionsForText2D( 0.1f, strText );
	Vec2 pivotDim = pivot * textDimensions;
	context->m_fonts[0]->AddVertsForText2D( debugObject->m_vertices, Vec2( 0.f, 0.f ), 0.1f, std::string( text ) );
	debugObject->m_texture = context->m_fonts[0]->GetTexture();


	//Vec3 pivotOrigin = Vec3( origin.x, origin.y, origin.z );
	debugObject->m_modelMatrix = basis;
	debugObject->m_pivotDim = Vec3( pivotDim.x, pivotDim.y, 0.f );


	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_indices.push_back( (uint)vertIndex );
	}
	for( int vertIndex = (int)debugObject->m_vertices.size() - 1; vertIndex >= 0; vertIndex-- )
	{
		debugObject->m_indices.push_back( (uint)vertIndex );
	}
	s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddWorldTextf( Mat44 const& basis, Vec2 pivot, Rgba8 const& color, float duration, eDebugRenderMode mode, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string text = Stringv( format, args );

	DebugAddWorldText( basis, pivot, color, color, duration, mode, text.c_str() );
}

void DebugAddWorldTextf( Mat44 const& basis, Vec2 pivot, Rgba8 const& color, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string text = Stringv( format, args );

	DebugAddWorldText( basis, pivot, color, color, 0.f, DEBUG_RENDER_USE_DEPTH, text.c_str() );
}

void DebugAddWorldBillboardText( Vec3 const& origin, Vec2 const& pivot, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode, char const* text )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_mode = mode;
	debugObject->m_renderTo = DEBUG_RENDER_TO_WORLD;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_isText = true;
	debugObject->m_isBillBoarded = true;


	std::string strText(text);
	RenderContext* context = s_DebugRenderSystem->m_context;
	Vec2 textDimensions = context->m_fonts[0]->GetDimensionsForText2D(0.1f, strText );
	Vec2 pivotDim = pivot * textDimensions;
	context->m_fonts[0]->AddVertsForText2D( debugObject->m_vertices, Vec2(0.f, 0.f), 0.1f, std::string(text) );
	debugObject->m_texture = context->m_fonts[0]->GetTexture();


	Vec3 pivotOrigin = Vec3( origin.x, origin.y, origin.z );
	debugObject->m_modelMatrix = Mat44::CreateTranslation3D( pivotOrigin );
	debugObject->m_pivotDim = Vec3( pivotDim.x, pivotDim.y, 0.f );


	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_indices.push_back( (uint)vertIndex );
	}

	s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddWorldBillboardTextf( Vec3 const& origin, Vec2 const& pivot, Rgba8 const& color, float duration, eDebugRenderMode mode, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string text = Stringv( format, args );

	DebugAddWorldBillboardText( origin, pivot, color, color, duration, mode, text.c_str() );
}

void DebugAddWorldBillboardTextf( Vec3 const& origin, Vec2 const& pivot, Rgba8 const& color, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string text = Stringv( format, args );

	DebugAddWorldBillboardText( origin, pivot, color, color, 0.f, DEBUG_RENDER_USE_DEPTH, text.c_str() );
}

void DebugAddScreenPoint( Vec2 const& pos, float size, Rgba8 const& startColor, Rgba8 const& endColor, float duration )
{
	float halfSize = 0.5f * size;
	AABB2 aabb = AABB2( -halfSize, -halfSize, halfSize, halfSize );
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_modelMatrix = Mat44::CreateTranslation3D( pos );
	debugObject->m_renderTo = DEBUG_RENDER_TO_SCREEN;
	debugObject->m_isBillBoarded = false;

	Vertex_PCU::AppendVertsAABB2D( debugObject->m_vertices, aabb, startColor );
	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_indices.push_back((uint)vertIndex);
	}
	s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

