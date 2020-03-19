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

static RenderContext*	s_DebugRenderContext = nullptr;
static Camera*			s_DebugCamera = nullptr;

struct DebugRenderObject
{
public:
	void UpdateColors();
	void AppendVerts( std::vector<Vertex_PCU>& vertexList, Mat44 const& cameraView );

public:
	Timer m_timer;

	Rgba8 m_startColor = Rgba8::WHITE;
	Rgba8 m_endColor = Rgba8::WHITE;
	float m_duration = 0.f;
	eDebugRenderMode m_mode = DEBUG_RENDER_USE_DEPTH;
	Texture const* m_texture = nullptr;
	/*GPUMesh* m_mesh = nullptr;*/
	Transform m_transform;
	std::vector<Vertex_PCU> m_vertices;
	std::vector<int> m_indices;
	bool m_isText = false;
	bool m_isBillBoarded = false;
	//bool m_isReadyToBeCulled = false;
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
	//Check for billboarding
	Mat44 reversedCamera = cameraView;
	//reversedCamera.ScaleUniform3D(-1.f);
	MatrixInvertOrthoNormal( reversedCamera );
	Mat44 transformMatrix = m_transform.ToMatrix();
	//transformMatrix.TransformBy( reversedCamera );
	//reversedCamera.TransformBy( transformMatrix );
	for( size_t vertexIndex = 0; vertexIndex < m_vertices.size(); vertexIndex++ )
	{
		Vec3 vertex = m_vertices[vertexIndex].position;
		Rgba8 const& tint = m_vertices[vertexIndex].tint;
		Vec2 const& uv = m_vertices[vertexIndex].uvTexCoords;

		//transformMatrix.TransformBy( cameraView );
		vertex = reversedCamera.TransformVector3D(vertex);
		vertex = transformMatrix.TransformPosition3D( vertex );
		//Vec4 vertexT = transformMatrix.TransformHomogeneousPoint3D( Vec4(vertex.x, vertex.y, vertex.z, 1.f) );
		//vertex = Vec3( vertexT.x, vertexT.y, vertexT.z );
		Vertex_PCU transformedVert = Vertex_PCU( vertex, tint, uv );

		vertexList.push_back( transformedVert );
	}
	//vertexList.insert( vertexList.end(), m_vertices.begin(), m_vertices.end() );
}

class DebugRenderSystem
{
public:
	void UpdateColors();
	void AppendVerts( std::vector<Vertex_PCU>& vertexList, Mat44 const& cameraView );
	void AppendTextVerts( std::vector<Vertex_PCU>& vertexList, Mat44 const& cameraView );

public:
	RenderContext* m_context;
	Texture const* m_fontText = nullptr;
	//Camera* m_camera;
	//Other stuff
	std::vector<DebugRenderObject*> m_renderObjects;
	bool m_isDebugRenderingEnabled = false;
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

void DebugRenderSystem::AppendVerts( std::vector<Vertex_PCU>& vertexList, Mat44 const& cameraView )
{
	for( size_t debugObjectIndex = 0; debugObjectIndex < m_renderObjects.size(); debugObjectIndex++ )
	{
		DebugRenderObject* debugObject = m_renderObjects[debugObjectIndex];
		if( nullptr != debugObject && !debugObject->m_isText)
		{
			debugObject->AppendVerts( vertexList, cameraView );
		}
	}
}

void DebugRenderSystem::AppendTextVerts( std::vector<Vertex_PCU>& vertexList, Mat44 const& cameraView )
{
	for( size_t debugObjectIndex = 0; debugObjectIndex < m_renderObjects.size(); debugObjectIndex++ )
	{
		DebugRenderObject* debugObject = m_renderObjects[debugObjectIndex];
		if( nullptr != debugObject && debugObject->m_isText )
		{
			debugObject->AppendVerts( vertexList, cameraView );
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
	//std::vector<DebugRenderObject*>& debugObjects = s_DebugRenderSystem->m_renderObjects;
	std::vector<Vertex_PCU> vertices;
	std::vector<Vertex_PCU> textVertices;

	//UpdateColors
	s_DebugRenderSystem->UpdateColors();
	//AppendVerts
	s_DebugRenderSystem->AppendVerts( vertices, cam->GetViewMatrix() );
	s_DebugRenderSystem->AppendTextVerts( textVertices, cam->GetViewMatrix() );
	cam->m_clearMode = NO_CLEAR;

	context->BeginCamera( *cam );
	context->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL );
	context->SetBlendMode( BlendMode::ALPHA );
	context->BindTexture( nullptr );
	context->BindShader( (Shader*)nullptr );

	//Draw
	context->DrawVertexArray( vertices );

	//Draw Text
	Texture const* tex = context->m_fonts[0]->GetTexture();
	context->BindTexture( tex );
	context->DrawVertexArray( textVertices );

	context->EndCamera( *cam );
}

void DebugRenderScreenTo( Texture* output )
{
	UNUSED( output );
	if( !s_DebugRenderSystem->m_isDebugRenderingEnabled )
	{
		return;
	}
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
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_transform.SetPosition( pos );
	Vertex_PCU::AppendVertsAABB2D(debugObject->m_vertices, aabb, startColor);
	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_vertices[vertIndex].position;
	}
	s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

// void DebugAddWorldText( Mat44 const& basis, Vec2 pivot, Rgba8 const& starColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode, char const* text )
// {
// // 	Vec3 mins = Vec3( -0.5f, -0.5f, 0.f );
// // 	Vec3 maxs = Vec3( 0.5f, 0.5f, 0.f );
// // 	basis.TransformPosition3D( mins );
// // 	basis.TransformPosition3D( maxs );
// // 
// 
// }

void DebugAddWorldBillboardText( Vec3 const& origin, Vec2 const& pivot, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode, char const* text )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_mode = mode;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_isText = true;
	debugObject->m_isBillBoarded = true;


	std::string strText(text);
	RenderContext* context = s_DebugRenderSystem->m_context;
	Vec2 textDimensions = context->m_fonts[0]->GetDimensionsForText2D(0.1f, strText );
	Vec2 pivotDim = pivot * textDimensions;
	context->m_fonts[0]->AddVertsForText2D( debugObject->m_vertices, Vec2( 0.f,0.f ), 0.1f, std::string(text) );
	debugObject->m_texture = context->m_fonts[0]->GetTexture();


	Vec3 pivotOrigin = Vec3( origin.x - pivotDim.x, origin.y - pivotDim.y, origin.z );
	debugObject->m_transform.SetPosition( pivotOrigin );


	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_vertices[vertIndex].position;
	}
	s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

