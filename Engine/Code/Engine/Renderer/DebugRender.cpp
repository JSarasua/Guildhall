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
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Vec4.hpp"
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
	Rgba8 m_meshColor = Rgba8::WHITE;
	float m_duration = 0.f;
	eDebugRenderMode m_mode = DEBUG_RENDER_USE_DEPTH;
	eDebugRenderTo m_renderTo = DEBUG_RENDER_TO_WORLD;
	Texture const* m_texture = nullptr;
	GPUMesh* m_mesh = nullptr;
	Vec3 m_pivotDim;
	Vec4 m_screenLocation;
	Mat44 m_modelMatrix;
	std::vector<Vertex_PCU> m_vertices;
	std::vector<uint> m_indices;
	bool m_isText = false;
	bool m_isBillBoarded = false;
	bool m_isWireMesh = false;
};

void DebugRenderObject::UpdateColors()
{
	float duration = (float)m_timer.GetElapsedSeconds();
	float totalTime = duration + (float)m_timer.GetSecondsRemaining();
	float lerpValue = 1.f;

	if( totalTime != 0.f )
	{
		lerpValue = duration / totalTime;
	}

	Rgba8 color = Rgba8::LerpColorAsHSL( m_startColor, m_endColor, lerpValue );

	for( size_t vertIndex = 0; vertIndex < m_vertices.size(); vertIndex++ )
	{
		m_vertices[vertIndex].tint = color;
	}

	if( nullptr != m_mesh )
	{
		m_meshColor = color;
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

	Mat44 transformMatrix = m_modelMatrix;
	Mat44 screenTranslation;
	if( m_screenLocation != Vec4() )
	{
		AABB2 screenBounds = DebugGetScreenBounds();
		Vec2 relativePos;
		relativePos.x = screenBounds.maxs.x * m_screenLocation.x;
		relativePos.y = screenBounds.maxs.y * m_screenLocation.y;

		Vec2 relativeOffset = Vec2( m_screenLocation.z, m_screenLocation.w );

		Vec2 absolutePos = relativePos + relativeOffset;
		screenTranslation = Mat44::CreateTranslationXY( absolutePos );
	}

	if( m_isBillBoarded )
	{
		//Mat44 pivotTransform = Mat44::CreateTranslation3D( -m_pivotDim );

		transformMatrix.TransformBy( cameraView ); 	//Camera doesn't need to be reversed because camera is facing -Z but our append verts are already assuming drawing +Z
		//transformMatrix.TransformBy( pivotTransform );
	}

	transformMatrix.TransformBy( screenTranslation );
	Mat44 pivotTransform = Mat44::CreateTranslation3D( -m_pivotDim );
	transformMatrix.TransformBy( pivotTransform );

	for( size_t vertexIndex = 0; vertexIndex < m_vertices.size(); vertexIndex++ )
	{
		Vec3 vertex = m_vertices[vertexIndex].position;
		Rgba8 const& color = m_vertices[vertexIndex].tint;
		Vec2 const& uv = m_vertices[vertexIndex].uvTexCoords;

		Rgba8 tintedColor = Rgba8::LerpColorAsHSL( color, tint, tintStrength );

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

	void DrawTexturedObjects( RenderContext* context, Mat44 const& cameraModel, eDebugRenderTo renderTo, eDebugRenderMode renderMode );
	void DrawMeshes( RenderContext* context, Mat44 const& cameraModel, eDebugRenderTo renderTo, eDebugRenderMode renderMode );

	void AddObjectToList( DebugRenderObject* objectToAdd, std::vector<DebugRenderObject*>& listToAddto );

public:
	RenderContext* m_context;
	Texture const* m_fontText = nullptr;
	std::vector<DebugRenderObject*> m_renderObjects;
	std::vector<DebugRenderObject*> m_texturedRenderObjects;
	std::vector<DebugRenderObject*> m_meshObjects;
	bool m_isDebugRenderingEnabled = false;
	float m_screenHeight = 1080.f;
	float m_aspectRatio = 0.f;

	GPUMesh* m_cubeMesh;
	GPUMesh* m_sphereMesh;
	GPUMesh* m_basisMesh;
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

	for( size_t debugObjectIndex = 0; debugObjectIndex < m_texturedRenderObjects.size(); debugObjectIndex++ )
	{
		DebugRenderObject* debugObject = m_texturedRenderObjects[debugObjectIndex];
		if( nullptr != debugObject )
		{
			debugObject->UpdateColors();
		}
	}

	for( size_t debugObjectIndex = 0; debugObjectIndex < m_meshObjects.size(); debugObjectIndex++ )
	{
		DebugRenderObject* debugObject = m_meshObjects[debugObjectIndex];
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
				if( debugObject->m_renderTo == renderTo )
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
	}
	else
	{
		for( size_t debugObjectIndex = 0; debugObjectIndex < m_renderObjects.size(); debugObjectIndex++ )
		{
			DebugRenderObject* debugObject = m_renderObjects[debugObjectIndex];

			if( nullptr != debugObject && !debugObject->m_isText )
			{
				if( debugObject->m_renderTo == DEBUG_RENDER_TO_SCREEN && debugObject->m_renderTo == renderTo  )
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
				if( debugObject->m_renderTo == renderTo )
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
	}
	else
	{
		for( size_t debugObjectIndex = 0; debugObjectIndex < m_renderObjects.size(); debugObjectIndex++ )
		{
			DebugRenderObject* debugObject = m_renderObjects[debugObjectIndex];

			if( nullptr != debugObject && debugObject->m_isText )
			{
				if( debugObject->m_renderTo == DEBUG_RENDER_TO_SCREEN && debugObject->m_renderTo == renderTo   )
				{
					debugObject->AppendIndexedVerts( vertexList, indexList, cameraView );
				}
			}
		}
	}
}

void DebugRenderSystem::DrawTexturedObjects( RenderContext* context, Mat44 const& cameraModel, eDebugRenderTo renderTo, eDebugRenderMode renderMode )
{
	if( nullptr == context )
	{
		return;
	}

	for( size_t debugObjectIndex = 0; debugObjectIndex < m_texturedRenderObjects.size(); debugObjectIndex++ )
	{
		DebugRenderObject* debugObject = m_texturedRenderObjects[debugObjectIndex];
		if( nullptr != debugObject )
		{
			if( debugObject->m_renderTo == renderTo && debugObject->m_mode == renderMode )
			{
				std::vector<Vertex_PCU> vertexList;
				std::vector<uint> indexList;
				debugObject->AppendIndexedVerts( vertexList, indexList, cameraModel );

				Texture const* tex = debugObject->m_texture;
				context->BindTexture( tex );
				context->DrawIndexedVertexArray( vertexList, indexList );
			}
		}
	}
}

void DebugRenderSystem::DrawMeshes( RenderContext* context, Mat44 const& cameraModel, eDebugRenderTo renderTo, eDebugRenderMode renderMode )
{
	UNUSED( cameraModel );
	if( nullptr == context )
	{
		return;
	}

	context->BindTexture( nullptr );

	for( size_t debugObjectIndex = 0; debugObjectIndex < m_meshObjects.size(); debugObjectIndex++ )
	{
		DebugRenderObject* debugObject = m_meshObjects[debugObjectIndex];
		if( nullptr != debugObject )
		{
			if( debugObject->m_renderTo == renderTo && ( (debugObject->m_mode == renderMode) || (debugObject->m_mode == DEBUG_RENDER_XRAY && renderMode == DEBUG_RENDER_USE_DEPTH) ) )
			{
				if( renderMode == DEBUG_RENDER_XRAY )
				{
					if( debugObject->m_isWireMesh )
					{
						context->SetFillMode( eFillMode::FILL_WIREFRAME );
						context->SetCullMode( eCullMode::CULL_NONE );
					}
					GPUMesh* mesh = debugObject->m_mesh;
					Mat44 modelMatrix = debugObject->m_modelMatrix;
					Rgba8 tintColor = Rgba8::LerpColorAsHSL( debugObject->m_meshColor, Rgba8(0,0,0,0), 0.7f );
					context->SetModelMatrix( modelMatrix, tintColor );
					context->DrawMesh( mesh );
					context->SetFillMode( eFillMode::FILL_SOLID );
					context->SetCullMode( eCullMode::CULL_BACK );
				}
				else
				{
					if( debugObject->m_isWireMesh )
					{
						context->SetFillMode( eFillMode::FILL_WIREFRAME );
						context->SetCullMode( eCullMode::CULL_NONE );
					}
					GPUMesh* mesh = debugObject->m_mesh;
					Mat44 modelMatrix = debugObject->m_modelMatrix;
					context->SetModelMatrix( modelMatrix, debugObject->m_meshColor );
					context->DrawMesh( mesh );
					context->SetFillMode( eFillMode::FILL_SOLID );
					context->SetCullMode( eCullMode::CULL_BACK );
				}
			}
		}
	}
}

void DebugRenderSystem::AddObjectToList( DebugRenderObject* objectToAdd, std::vector<DebugRenderObject*>& listToAddto )
{
	for( size_t listIndex = 0; listIndex < listToAddto.size(); listIndex++ )
	{
		if( nullptr == listToAddto[listIndex] )
		{
			listToAddto[listIndex] = objectToAdd;
			return;
		}
	}

	listToAddto.push_back( objectToAdd );

	return;
}

static DebugRenderSystem* s_DebugRenderSystem = nullptr;

/************************************************************************/
/* Methods                                                              
/************************************************************************/
void DebugRenderSystemStartup( RenderContext* context )
{
	s_DebugRenderSystem = new DebugRenderSystem();
	s_DebugRenderSystem->m_context = context;

	g_theEventSystem->SubscribeToEvent("debug_render", CONSOLECOMMAND, SetDebugRenderEnabled );
	g_theEventSystem->SubscribeToEvent("debug_add_world_point", CONSOLECOMMAND, DebugAddWorldPoint );
	g_theEventSystem->SubscribeToEvent("debug_add_world_wire_sphere", CONSOLECOMMAND, DebugAddWorldWireSphere );
	g_theEventSystem->SubscribeToEvent("debug_add_world_wire_bounds", CONSOLECOMMAND, DebugAddWorldWireBounds );
	g_theEventSystem->SubscribeToEvent("debug_add_world_billboard_text", CONSOLECOMMAND, DebugAddWorldBillboardText );
	g_theEventSystem->SubscribeToEvent("debug_add_screen_point", CONSOLECOMMAND, DebugAddScreenPoint );
	g_theEventSystem->SubscribeToEvent("debug_add_screen_quad", CONSOLECOMMAND, DebugAddScreenQuad );
	g_theEventSystem->SubscribeToEvent("debug_add_screen_text", CONSOLECOMMAND, DebugAddScreenText );

	DebugRenderCreateMeshes();
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

	delete s_DebugRenderSystem->m_cubeMesh;
	s_DebugRenderSystem->m_cubeMesh = nullptr;

	delete s_DebugRenderSystem->m_sphereMesh;
	s_DebugRenderSystem->m_sphereMesh = nullptr;

	delete s_DebugRenderSystem->m_basisMesh;
	s_DebugRenderSystem->m_basisMesh = nullptr;
}

void DebugRenderCreateMeshes()
{
	//Cube
	s_DebugRenderSystem->m_cubeMesh = new GPUMesh( s_DebugRenderSystem->m_context );
	std::vector<Vertex_PCU> cubeVerts;
	std::vector<uint> cubeIndices;
	AppendIndexedVertsCube( cubeVerts, cubeIndices, 1.f );
	s_DebugRenderSystem->m_cubeMesh->UpdateVertices( cubeVerts );
	s_DebugRenderSystem->m_cubeMesh->UpdateIndices( cubeIndices );

	//Sphere
	s_DebugRenderSystem->m_sphereMesh = new GPUMesh( s_DebugRenderSystem->m_context );
	std::vector<Vertex_PCU> sphereVerts;
	std::vector<uint> sphereIndices;
	AppendIndexedVertsSphere( sphereVerts, sphereIndices, Vec3( 0.f, 0.f, 0.f ), 1.f, 64, 64, Rgba8::WHITE );
	s_DebugRenderSystem->m_sphereMesh->UpdateVertices( sphereVerts );
	s_DebugRenderSystem->m_sphereMesh->UpdateIndices( sphereIndices );


	//Bases
	Vec3 iBasis = Vec3( 1.f, 0.f, 0.f );
	Vec3 jBasis = Vec3( 0.f, 1.f, 0.f );
	Vec3 kBasis = Vec3( 0.f, 0.f, 1.f );
	Vec3 tBasis = Vec3();

	iBasis += tBasis;
	jBasis += tBasis;
	kBasis += tBasis;

	s_DebugRenderSystem->m_basisMesh = new GPUMesh( s_DebugRenderSystem->m_context );
	std::vector<Vertex_PCU> basisVerts;
	std::vector<uint> basisIndices;

	float ilineLength = GetDistance3D( tBasis, iBasis );
	float ilineThickness = ilineLength * 0.025f;
	float ilineRadius = ilineThickness * 0.5f;
	AppendIndexedVertsCylinder( basisVerts, basisIndices, tBasis, ilineRadius, iBasis, 0.f, 8U, Rgba8::RED );

	Vec3 istartOfFlair = tBasis - iBasis;
	istartOfFlair *= 0.2f;
	istartOfFlair += iBasis;
	float iflairRadius = ilineRadius * 3.f;
	AppendIndexedVertsCone( basisVerts, basisIndices, istartOfFlair, iflairRadius, iBasis, 8U, Rgba8::RED );


	float jlineLength = GetDistance3D( tBasis, jBasis );
	float jlineThickness = jlineLength * 0.025f;
	float jlineRadius = jlineThickness * 0.5f;
	AppendIndexedVertsCylinder( basisVerts, basisIndices, tBasis, jlineRadius, jBasis, 0.f, 8U, Rgba8::GREEN );

	Vec3 jstartOfFlair = tBasis - jBasis;
	jstartOfFlair *= 0.2f;
	jstartOfFlair += jBasis;
	float jflairRadius = jlineRadius * 3.f;
	AppendIndexedVertsCone( basisVerts, basisIndices, jstartOfFlair, jflairRadius, jBasis, 8U, Rgba8::GREEN );


	float klineLength = GetDistance3D( tBasis, kBasis );
	float klineThickness = klineLength * 0.025f;
	float klineRadius = klineThickness * 0.5f;
	AppendIndexedVertsCylinder( basisVerts, basisIndices, tBasis, klineRadius, kBasis, 0.f, 8U, Rgba8::BLUE );

	Vec3 kstartOfFlair = tBasis - kBasis;
	kstartOfFlair *= 0.2f;
	kstartOfFlair += kBasis;
	float kflairRadius = klineRadius * 3.f;
	AppendIndexedVertsCone( basisVerts, basisIndices, kstartOfFlair, kflairRadius, kBasis, 8U, Rgba8::BLUE );

	s_DebugRenderSystem->m_basisMesh->UpdateVertices( basisVerts );
	s_DebugRenderSystem->m_basisMesh->UpdateIndices( basisIndices );
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

	std::vector<Vertex_PCU> AlwaysVertices;
	std::vector<uint> AlwaysIndices;
	std::vector<Vertex_PCU> AlwaysTextVertices;
	std::vector<uint> AlwaysTextIndices;

	//UpdateColors
	s_DebugRenderSystem->UpdateColors();
	//AppendVerts
	Mat44 camTransformRotationMatrix = cam->GetModelRotationMatrix();
	s_DebugRenderSystem->AppendIndexedVerts( useDepthVertices, useDepthIndices, camTransformRotationMatrix, DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_USE_DEPTH );
	s_DebugRenderSystem->AppendIndexedTextVerts( useDepthTextVertices, useDepthTextIndices, camTransformRotationMatrix, DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_USE_DEPTH );
	s_DebugRenderSystem->AppendIndexedVerts( XRayVertices, XRayIndices, camTransformRotationMatrix, DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_XRAY );
	s_DebugRenderSystem->AppendIndexedTextVerts( XRayTextVertices, XRayTextIndices, camTransformRotationMatrix, DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_XRAY );
	s_DebugRenderSystem->AppendIndexedVerts( AlwaysVertices, AlwaysIndices, camTransformRotationMatrix, DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_ALWAYS );
	s_DebugRenderSystem->AppendIndexedTextVerts( AlwaysTextVertices, AlwaysTextIndices, camTransformRotationMatrix, DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_ALWAYS );
	cam->m_clearMode = NO_CLEAR;

	context->BeginCamera( *cam );
	context->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL ); //Change to add if writing to depth buffer for xray mode
	context->SetBlendMode( eBlendMode::ALPHA );
	context->BindTexture( nullptr );
	context->BindShader( (Shader*)nullptr );

	//Draw
	context->DrawIndexedVertexArray( useDepthVertices, useDepthIndices );

	context->SetDepth( eDepthCompareMode::COMPARE_GREATER_THAN, eDepthWriteMode::WRITE_NONE );
	context->DrawIndexedVertexArray( XRayVertices, XRayIndices );

	context->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_NONE );
	context->DrawIndexedVertexArray( AlwaysVertices, AlwaysIndices );

	//Draw Text
	context->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL );
	Texture const* tex = context->m_fonts[0]->GetTexture();
	context->BindTexture( tex );
	context->DrawIndexedVertexArray( useDepthTextVertices, useDepthTextIndices );

	context->SetDepth( eDepthCompareMode::COMPARE_GREATER_THAN, eDepthWriteMode::WRITE_NONE );
	context->DrawIndexedVertexArray( XRayTextVertices, XRayTextIndices );

	context->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_NONE );
	context->DrawIndexedVertexArray( AlwaysTextVertices, AlwaysTextIndices );

	context->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL );
	s_DebugRenderSystem->DrawMeshes( context, camTransformRotationMatrix, DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_USE_DEPTH );
	context->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_NONE );
	s_DebugRenderSystem->DrawMeshes( context, camTransformRotationMatrix, DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_ALWAYS );
	context->SetDepth( eDepthCompareMode::COMPARE_GREATER_THAN, eDepthWriteMode::WRITE_NONE );
	s_DebugRenderSystem->DrawMeshes( context, camTransformRotationMatrix, DEBUG_RENDER_TO_WORLD, DEBUG_RENDER_XRAY );

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
	

	if( s_DebugRenderSystem->m_aspectRatio == 0.f )
	{
		float aspectRatio = outputDimensions.x / outputDimensions.y;
		s_DebugRenderSystem->m_aspectRatio = aspectRatio;
	}

	float aspectRatio = s_DebugRenderSystem->m_aspectRatio;
	float screenHeight = s_DebugRenderSystem->m_screenHeight;

	Vec2 max( aspectRatio * screenHeight, screenHeight );
	cam.SetProjectionOrthographic(max, 0.f, 1000.f );
	cam.SetPosition( Vec3(0.5f * max) );
	cam.m_cameraType = SCREENCAMERA;

	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;
	std::vector<Vertex_PCU> textVertices;
	std::vector<uint> textIndices;

	Mat44 cameraScreenRotation = cam.GetCameraScreenRotationMatrix();

	s_DebugRenderSystem->AppendIndexedVerts( vertices, indices, cameraScreenRotation, DEBUG_RENDER_TO_SCREEN, DEBUG_RENDER_ALWAYS );
	s_DebugRenderSystem->AppendIndexedTextVerts( textVertices, textIndices, cameraScreenRotation, DEBUG_RENDER_TO_SCREEN, DEBUG_RENDER_ALWAYS );


	context->BeginCamera( cam );
	context->SetDepth( eDepthCompareMode::COMPARE_ALWAYS );
	context->SetBlendMode( eBlendMode::ALPHA );
	context->BindTexture( nullptr );
	context->BindShader( (Shader*)nullptr );

	//Draw
	context->DrawIndexedVertexArray( vertices, indices );

	//Draw Text
	Texture const* tex = context->m_fonts[0]->GetTexture();
	context->BindTexture( tex );
	context->DrawIndexedVertexArray( textVertices, textIndices );

	s_DebugRenderSystem->DrawTexturedObjects( context, cameraScreenRotation, DEBUG_RENDER_TO_SCREEN, DEBUG_RENDER_ALWAYS );

	
	s_DebugRenderSystem->DrawMeshes( context, cameraScreenRotation, DEBUG_RENDER_TO_SCREEN, DEBUG_RENDER_ALWAYS );
	
	context->EndCamera( cam );


}

void DebugRenderEndFrame()
{
	std::vector<DebugRenderObject*>& debugObjects = s_DebugRenderSystem->m_renderObjects;
	for( size_t debugIndex = 0; debugIndex < debugObjects.size(); debugIndex++ )
	{
		if( nullptr != debugObjects[debugIndex] )
		{
			Timer& objTimer = debugObjects[debugIndex]->m_timer;
			if( objTimer.HasElapsed() )
			{
				delete debugObjects[debugIndex];
				debugObjects[debugIndex] = nullptr;
			}
		}
	}

	std::vector<DebugRenderObject*>& texturedDebugObjects = s_DebugRenderSystem->m_texturedRenderObjects;
	for( size_t debugIndex = 0; debugIndex < texturedDebugObjects.size(); debugIndex++ )
	{
		if( nullptr != texturedDebugObjects[debugIndex] )
		{
			Timer& objTimer = texturedDebugObjects[debugIndex]->m_timer;
			if( objTimer.HasElapsed() )
			{
				delete texturedDebugObjects[debugIndex];
				texturedDebugObjects[debugIndex] = nullptr;
			}
		}
	}

	std::vector<DebugRenderObject*>& meshObjects = s_DebugRenderSystem->m_meshObjects;
	for( size_t debugIndex = 0; debugIndex < meshObjects.size(); debugIndex++ )
	{
		if( nullptr != meshObjects[debugIndex] )
		{
			Timer& objTimer = meshObjects[debugIndex]->m_timer;
			if( objTimer.HasElapsed() )
			{
				delete meshObjects[debugIndex];
				meshObjects[debugIndex] = nullptr;
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
	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_renderObjects );
	//s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddWorldPoint( Vec3 const& pos, float size, Rgba8 const& color, float duration /*= 0.f*/, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWorldPoint( pos, size, color, color, duration, mode );
}

void DebugAddWorldPoint( Vec3 const& pos, Rgba8 const& color, float duration /*= 0.f*/, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWorldPoint( pos, 0.1f, color, color, duration, mode );
}

void DebugAddWorldLine( LineSegment3 const& line, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode )
{
	
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_mode = mode;
	debugObject->m_renderTo = DEBUG_RENDER_TO_WORLD;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	//debugObject->m_modelMatrix = Mat44::CreateTranslation3D( pos );
	debugObject->m_isBillBoarded = false;
	debugObject->m_isText = false;

	float lineLength = line.GetLength();
	float lineThickness = lineLength * 0.025f;
	float lineRadius = lineThickness * 0.5f;
	AppendIndexedVertsCylinder( debugObject->m_vertices, debugObject->m_indices, line.startPosition, lineRadius, line.endPosition, lineRadius, 8U, startColor );

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_renderObjects );
	//s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddWorldLine( LineSegment3 const& line, Rgba8 const& color, float duration /*= 0.f*/, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWorldLine( line, color, color, duration, mode );
}

void DebugAddWorldArrow( LineSegment3 const& arrow, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_mode = mode;
	debugObject->m_renderTo = DEBUG_RENDER_TO_WORLD;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	//debugObject->m_modelMatrix = Mat44::CreateTranslation3D( pos );
	debugObject->m_isBillBoarded = false;
	debugObject->m_isText = false;

	float lineLength = arrow.GetLength();
	float lineThickness = lineLength * 0.025f;
	float lineRadius = lineThickness * 0.5f;
	AppendIndexedVertsCylinder( debugObject->m_vertices, debugObject->m_indices, arrow.startPosition, lineRadius, arrow.endPosition, 0.f, 8U, startColor );

	Vec3 startOfFlair = arrow.startPosition - arrow.endPosition;
	startOfFlair *= 0.2f;
	startOfFlair += arrow.endPosition;
	float flairRadius = lineRadius * 3.f;
	AppendIndexedVertsCone( debugObject->m_vertices, debugObject->m_indices, startOfFlair, flairRadius, arrow.endPosition, 8U, startColor );

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_renderObjects );
	//s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddWorldArrow( LineSegment3 const& arrow, Rgba8 const& color, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWorldArrow( arrow, color, color, duration, mode );
}

void DebugAddWorldQuad( Vec3 const& p0, Vec3 const& p1, Vec3 const& p2, Vec3 const& p3, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_mode = mode;
	debugObject->m_renderTo = DEBUG_RENDER_TO_WORLD;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	//debugObject->m_modelMatrix = Mat44::CreateTranslation3D( pos );
	debugObject->m_isBillBoarded = false;
	debugObject->m_isText = false;
	
	
	Vertex_PCU::AppendVerts4Points( debugObject->m_vertices, debugObject->m_indices, p0, p1, p2, p3, startColor );

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_renderObjects );
	//s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddWorldWireBounds( Transform const& transform, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_mode = mode;
	debugObject->m_renderTo = DEBUG_RENDER_TO_WORLD;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_modelMatrix = transform.ToMatrix();
	debugObject->m_isBillBoarded = false;
	debugObject->m_isText = false;
	debugObject->m_isWireMesh = true;
	debugObject->m_mesh	= s_DebugRenderSystem->m_cubeMesh;

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_meshObjects );
	//s_DebugRenderSystem->m_meshObjects.push_back( debugObject );
}

void DebugAddWorldWireBounds( Transform const& transform, Rgba8 const& color, float duration /*= 0.f*/, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWorldWireBounds( transform, color, color, duration, mode );
}

void DebugAddWorldWireSphere( Vec3 const& pos, float radius, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_mode = mode;
	debugObject->m_renderTo = DEBUG_RENDER_TO_WORLD;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_isBillBoarded = false;
	debugObject->m_isText = false;
	debugObject->m_isWireMesh = true;
	debugObject->m_mesh	= s_DebugRenderSystem->m_sphereMesh;

	Transform transform;
	transform.SetPosition( pos );
	transform.SetUniformScale( radius );

	debugObject->m_modelMatrix = transform.ToMatrix();

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_meshObjects );
	//s_DebugRenderSystem->m_meshObjects.push_back( debugObject );
}

void DebugAddWorldWireSphere( Vec3 const& pos, float radius, Rgba8 const& color, float duration /*= 0.f*/, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWorldWireSphere( pos, radius, color, color, duration, mode );
}

void DebugAddWireMeshToWorld( Mat44 const& modelMatrix, GPUMesh* mesh, Rgba8 startTint, Rgba8 endTint, float duration, eDebugRenderMode mode )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startTint;
	debugObject->m_endColor = endTint;
	debugObject->m_duration = duration;
	debugObject->m_mode = mode;
	debugObject->m_renderTo = DEBUG_RENDER_TO_WORLD;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_isBillBoarded = false;
	debugObject->m_isText = false;
	debugObject->m_isWireMesh = true;
	debugObject->m_mesh	= mesh;

	debugObject->m_modelMatrix = modelMatrix;

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_meshObjects );
	//s_DebugRenderSystem->m_meshObjects.push_back( debugObject );
}

void DebugAddWorldBasis( Mat44 const& basis, Rgba8 const& startTint, Rgba8 const& endTint, float duration, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_duration = duration;
	debugObject->m_mode = mode;
	debugObject->m_renderTo = DEBUG_RENDER_TO_WORLD;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_isBillBoarded = false;
	debugObject->m_isText = false;
	debugObject->m_mesh	= s_DebugRenderSystem->m_basisMesh;
	debugObject->m_modelMatrix = basis;
	debugObject->m_startColor = startTint;
	debugObject->m_endColor = endTint;
	debugObject->m_isWireMesh = false;

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_meshObjects );
	//s_DebugRenderSystem->m_meshObjects.push_back( debugObject );
}

void DebugAddWorldBasis( Mat44 const& basis, float duration /*= 0.f*/, eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWorldBasis( basis, Rgba8::WHITE, Rgba8::WHITE, duration, mode );
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

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_renderObjects );
	//s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
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

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_renderObjects );
	//s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
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
	debugObject->m_mode = DEBUG_RENDER_ALWAYS;
	debugObject->m_isBillBoarded = false;

	Vertex_PCU::AppendVertsAABB2D( debugObject->m_vertices, aabb, startColor );
	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_indices.push_back((uint)vertIndex);
	}

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_renderObjects );
	//s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddScreenPoint( Vec2 const& pos, float size, Rgba8 const& color, float duration /*= 0.f */ )
{
	DebugAddScreenPoint( pos, size, color, color, duration );
}

void DebugAddScreenPoint( Vec2 const& pos, Rgba8 const& color )
{
	DebugAddScreenPoint( pos, 10.f, color, color, 0.f );
}

void DebugAddScreenLine( Vec2 const& p0, Vec2 const& p1, Rgba8 const& startColor, Rgba8 const& endColor, float duration )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_modelMatrix = Mat44(); //Identity
	debugObject->m_renderTo = DEBUG_RENDER_TO_SCREEN;
	debugObject->m_mode = DEBUG_RENDER_ALWAYS;
	debugObject->m_isBillBoarded = false;

	LineSegment2 line = LineSegment2( p0, p1 );
	float lineLength = line.GetLength();
	float lineThickness = lineLength * .01f;
	Vertex_PCU::AppendVertsLine2D( debugObject->m_vertices, line, lineThickness, startColor );

	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_indices.push_back( (uint)vertIndex );
	}

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_renderObjects );
	//s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddScreenLine( Vec2 const& p0, Vec2 const& p1, Rgba8 const& color, float duration /*= 0.f */ )
{
	DebugAddScreenLine( p0, p1, color, color, duration );
}

void DebugAddScreenArrow( Vec2 const& p0, Vec2 const& p1, Rgba8 const& startColor, Rgba8 const& endColor, float duration )
{

	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_modelMatrix = Mat44(); //Identity
	debugObject->m_renderTo = DEBUG_RENDER_TO_SCREEN;
	debugObject->m_mode = DEBUG_RENDER_ALWAYS;
	debugObject->m_isBillBoarded = false;

	LineSegment2 line( p0, p1 );
	Vec2 normalizedDisplacement = (line.endPosition - line.startPosition).GetNormalized();
	float lineLength = line.GetLength();
	float lineThickness = lineLength * .025f;
	float radius = lineThickness * 5.f;
	Vec2 fwd = normalizedDisplacement * radius;
	Vec2 leftVec = fwd.GetRotated90Degrees();
	Vec2 endLeft = line.endPosition + leftVec;
	Vec2 endRight= line.endPosition + -leftVec;
	Vec2 endHead = line.endPosition + fwd;

	Vertex_PCU::AppendVertsLine2D( debugObject->m_vertices, line, lineThickness, startColor );
	debugObject->m_vertices.push_back( Vertex_PCU( Vec3(endRight), startColor, Vec2() ) );
	debugObject->m_vertices.push_back( Vertex_PCU( Vec3(endHead), startColor, Vec2() ) );
	debugObject->m_vertices.push_back( Vertex_PCU( Vec3(endLeft), startColor, Vec2() ) );

	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_indices.push_back( (uint)vertIndex );
	}

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_renderObjects );
	//s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddScreenArrow( Vec2 const& p0, Vec2 const& p1, Rgba8 const& color, float duration /*= 0.f */ )
{
	DebugAddScreenArrow( p0, p1, color, color, duration );
}



void DebugAddScreenAABB2( AABB2 const& bounds, Rgba8 const& startColor, Rgba8 const& endColor, float duration )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_modelMatrix = Mat44(); //Identity
	debugObject->m_renderTo = DEBUG_RENDER_TO_SCREEN;
	debugObject->m_mode = DEBUG_RENDER_ALWAYS;
	debugObject->m_isBillBoarded = false;
	Vertex_PCU::AppendVertsAABB2D( debugObject->m_vertices, bounds, startColor );

	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_indices.push_back( (uint)vertIndex );
	}

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_renderObjects );
	//s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddScreenAABB2( AABB2 const& bounds, Rgba8 const& color, float duration /*= 0.f */ )
{
	DebugAddScreenAABB2( bounds, color, color, duration );
}

void DebugAddScreenTexturedQuad( AABB2 const& bounds, Texture* tex, AABB2 const& uvs, Rgba8 const& startTint, Rgba8 const& endTint, float duration /*= 0.f */ )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startTint;
	debugObject->m_endColor = endTint;
	debugObject->m_duration = duration;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_modelMatrix = Mat44(); //Identity
	debugObject->m_renderTo = DEBUG_RENDER_TO_SCREEN;
	debugObject->m_mode = DEBUG_RENDER_ALWAYS;
	debugObject->m_isBillBoarded = false;
	debugObject->m_texture = tex;
	Vertex_PCU::AppendVertsAABB2D( debugObject->m_vertices, bounds, startTint, uvs );

	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_indices.push_back( (uint)vertIndex );
	}

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_texturedRenderObjects );
	//s_DebugRenderSystem->m_texturedRenderObjects.push_back( debugObject );
}

void DebugAddScreenTexturedQuad( AABB2 const& bounds, Texture* tex, AABB2 const& uvs, Rgba8 const& tint, float duration /*= 0.f */ )
{
	DebugAddScreenTexturedQuad( bounds, tex, uvs, tint, tint, duration );
}

void DebugAddScreenTexturedQuad( AABB2 const& bounds, Texture* tex )
{
	DebugAddScreenTexturedQuad( bounds, tex, AABB2(), Rgba8::WHITE, Rgba8::WHITE, 0.f );
}

void DebugAddScreenText( Vec4 const& pos, Vec2 const& pivot, float textSize, Rgba8 const& startColor, Rgba8 const& endColor, float duration, char const* text )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_startColor = startColor;
	debugObject->m_endColor = endColor;
	debugObject->m_duration = duration;
	debugObject->m_mode = DEBUG_RENDER_ALWAYS;
	debugObject->m_renderTo = DEBUG_RENDER_TO_SCREEN;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_isText = true;
	debugObject->m_isBillBoarded = false;
	debugObject->m_screenLocation = pos;


	std::string strText( text );
	RenderContext* context = s_DebugRenderSystem->m_context;
	Vec2 textDimensions = context->m_fonts[0]->GetDimensionsForText2D( textSize, strText );
	Vec2 pivotDim = pivot * textDimensions;
	context->m_fonts[0]->AddVertsForText2D( debugObject->m_vertices, Vec2( 0.f, 0.f ), textSize, std::string( text ) );
	debugObject->m_texture = context->m_fonts[0]->GetTexture();
	debugObject->m_pivotDim = Vec3( pivotDim.x, pivotDim.y, 0.f );


	for( size_t vertIndex = 0; vertIndex < debugObject->m_vertices.size(); vertIndex++ )
	{
		debugObject->m_indices.push_back( (uint)vertIndex );
	}

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_renderObjects );
	//s_DebugRenderSystem->m_renderObjects.push_back( debugObject );
}

void DebugAddScreenTextf( Vec4 const& pos, Vec2 const& pivot, float textSize, Rgba8 const& startColor, Rgba8 const& endColor, float duration, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string text = Stringv( format, args );

	DebugAddScreenText( pos, pivot, textSize, startColor, endColor, duration, text.c_str() );
}
// 
void DebugAddScreenTextf( Vec4 const& pos, Vec2 const& pivot, float textSize, Rgba8 const& color, float duration, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string text = Stringv( format, args );

	DebugAddScreenText( pos, pivot, textSize, color, color, duration, text.c_str() );
}
// 
void DebugAddScreenTextf( Vec4 const& pos, Vec2 const& pivot, float textSize, Rgba8 const& color, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string text = Stringv( format, args );

	DebugAddScreenText( pos, pivot, textSize, color, color, 0.f, text.c_str() );
}
// 
void DebugAddScreenTextf( Vec4 const& pos, Vec2 const& pivot, Rgba8 const& color, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string text = Stringv( format, args );

	DebugAddScreenText( pos, pivot, 10.f, color, color, 0.f, text.c_str() );
}

void DebugAddScreenBasis( Mat44 const& basis, Rgba8 const& startTint, Rgba8 const& endTint, float duration )
{
	DebugRenderObject* debugObject = new DebugRenderObject;
	debugObject->m_duration = duration;
	debugObject->m_mode = DEBUG_RENDER_ALWAYS;
	debugObject->m_renderTo = DEBUG_RENDER_TO_SCREEN;
	debugObject->m_timer.SetSeconds( s_DebugRenderSystem->m_context->m_gameClock, (double)duration );
	debugObject->m_isBillBoarded = false;
	debugObject->m_isText = false;
	debugObject->m_mesh	= s_DebugRenderSystem->m_basisMesh;
	debugObject->m_startColor = startTint;
	debugObject->m_endColor = endTint;
	debugObject->m_isWireMesh = false;

	
	AABB2 screenBounds = DebugGetScreenBounds();
	float height = screenBounds.maxs.y - screenBounds.mins.y;
	float screenBasisScale = height * 0.1f;
	Vec3 screenOffset( height * 0.2f, height * 0.2f, 0.f );
	Vec3 pos = screenBounds.maxs;
	pos.z = 100.f;
	pos -= screenOffset;
	
	debugObject->m_modelMatrix = basis;
	debugObject->m_modelMatrix.SetTranslation3D( Vec3( 0.f, 0.f, 0.f ) );
	debugObject->m_modelMatrix.ScaleUniform3D( screenBasisScale );
	debugObject->m_modelMatrix.SetTranslation3D( pos );

	s_DebugRenderSystem->AddObjectToList( debugObject, s_DebugRenderSystem->m_meshObjects );
	//s_DebugRenderSystem->m_meshObjects.push_back( debugObject );
}

void DebugRenderSetScreenHeight( float height )
{
	s_DebugRenderSystem->m_screenHeight = height;
}

AABB2 DebugGetScreenBounds()
{
	float height = s_DebugRenderSystem->m_screenHeight;
	float width = height * s_DebugRenderSystem->m_aspectRatio;
	return AABB2( 0.f, 0.f, width, height );
}

bool SetDebugRenderEnabled( const EventArgs* args )
{
	if( nullptr != args )
	{
		bool argValue = args->GetValue( std::string( "enabled" ), true );

		s_DebugRenderSystem->m_isDebugRenderingEnabled = argValue;
	}

	return true;
}

bool DebugAddWorldPoint( const EventArgs* args )
{
	if( nullptr != args )
	{
 		Vec3 argPosition = args->GetValue( std::string( "position" ), Vec3() );
 		float argDuration = args->GetValue( std::string( "duration" ), 0.f );
 		DebugAddWorldPoint( argPosition, Rgba8::WHITE, argDuration );
	}

	return true;
}

bool DebugAddWorldWireSphere( const EventArgs* args )
{
	if( nullptr != args )
	{
		Vec3 argPosition = args->GetValue( std::string( "position" ), Vec3() );
		float argRadius = args->GetValue( std::string( "radius" ), 1.f );
		float argDuration = args->GetValue( std::string( "duration" ), 0.f );
		DebugAddWorldWireSphere( argPosition, argRadius, Rgba8::WHITE, Rgba8::WHITE, argDuration );
	}

	return true;
}

bool DebugAddWorldWireBounds( const EventArgs* args )
{
	if( nullptr != args )
	{
		Vec3 argMin = args->GetValue( std::string( "min" ), Vec3() );
		Vec3 argMax = args->GetValue( std::string( "max" ), Vec3() );
		Vec3 diff = argMax - argMin;
		Transform transform;
		transform.SetPosition( diff + argMin );
		transform.SetNonUniformScale( diff );
		float argDuration = args->GetValue( std::string( "duration" ), 0.f );
		DebugAddWorldWireBounds( transform, Rgba8::WHITE, Rgba8::WHITE, argDuration );
	}
	return true;
}

bool DebugAddWorldBillboardText( const EventArgs* args )
{
	if( nullptr != args )
	{
		Vec3 argPosition = args->GetValue( std::string( "position" ), Vec3() );
		Vec2 argPivot = args->GetValue( std::string( "radius" ), Vec2() );
		std::string argString = args->GetValue( std::string( "text" ), std::string("") );
		DebugAddWorldBillboardText( argPosition, argPivot, Rgba8::WHITE, Rgba8::WHITE, 1.f, DEBUG_RENDER_USE_DEPTH, argString.c_str() );
	}

	return true;
}

bool DebugAddScreenPoint( const EventArgs* args )
{
	if( nullptr != args )
	{
		Vec2 argPosition = args->GetValue( std::string( "position" ), Vec2() );
		float argDuration = args->GetValue( std::string( "duration" ), 0.f );
		DebugAddScreenPoint( argPosition, 10.f, Rgba8::WHITE, argDuration );
	}

	return true;
}

bool DebugAddScreenQuad( const EventArgs* args )
{
	if( nullptr != args )
	{
		Vec2 argMin = args->GetValue( std::string( "min" ), Vec2() );
		Vec2 argMax = args->GetValue( std::string( "max" ), Vec2(1.f, 1.f) );
		float argDuration = args->GetValue( std::string( "duration" ), 0.f );

		AABB2 aabb( argMin, argMax );
		DebugAddScreenAABB2( aabb, Rgba8::WHITE, argDuration );
	}

	return true;
}

bool DebugAddScreenText( const EventArgs* args )
{
	if( nullptr != args )
	{
		Vec2 argPosition = args->GetValue( std::string( "position" ), Vec2() );
		Vec2 argPivot = args->GetValue( std::string( "radius" ), Vec2() );
		std::string argString = args->GetValue( std::string( "text" ), std::string( "" ) );
		Vec4 pos = Vec4( 0.f, 0.f, argPosition.x, argPosition.y );
		DebugAddScreenTextf( pos, argPivot, Rgba8::WHITE, argString.c_str() );
	}
	return true;
}

