#include "Engine/UI/UIManager.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include <vector>


UIManager::UIManager( Vec2 screenDimensions, RenderContext* contextToUse ) : 
	m_context( contextToUse )
{
	m_camera = Camera();
	m_camera.SetColorTarget( nullptr ); // we use this
	//m_camera.CreateMatchingDepthStencilTarget( m_context );
	m_camera.SetOutputSize( screenDimensions );
	m_camera.SetProjectionOrthographic( m_camera.m_outputSize, 0.f, 100.f );
	m_screenBounds = AABB2( m_camera.GetOrthoBottomLeft(), m_camera.GetOrthoTopRight() );

	m_UIMesh = new GPUMesh( m_context );
	std::vector<Vertex_PCU> uiVerts;
	std::vector<uint> uiIndices;
	AABB2 uiBounds = AABB2( Vec2( -0.5f, -0.5f ), Vec2( 0.5f, 0.5f ) );
	AABB2 defaultUVs = AABB2( Vec2(), Vec2( 1.f, 1.f ) );
	Vertex_PCU::AppendIndexedVertsAABB2D( uiVerts, uiIndices, uiBounds, Rgba8::WHITE, defaultUVs );
	m_UIMesh->UpdateVertices( uiVerts );
	m_UIMesh->UpdateIndices( uiIndices );

	Rgba8 clearColor = Rgba8::BLACK;
	m_camera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT, clearColor, 0.f, 0 );
}

UIManager::~UIManager()
{
	if( m_rootWidget )
	{
		delete m_rootWidget;
		m_rootWidget = nullptr;
	}

	if( m_UIMesh )
	{
		delete m_UIMesh;
		m_UIMesh = nullptr;
	}
}

void UIManager::Startup()
{
	m_rootWidget = new Widget( m_screenBounds );
	m_rootWidget->SetCanSelect( false );
	m_rootWidget->SetCanHover( false );

}

void UIManager::Render()
{
	Texture* backbuffer = m_context->GetBackBuffer();
	Texture* colorTarget = m_context->AcquireRenderTargetMatching( backbuffer );
	m_camera.SetColorTarget( 0, colorTarget );

	m_context->BeginCamera( m_camera );
	m_context->SetModelMatrix( Mat44() );
	m_context->SetBlendMode( eBlendMode::ALPHA );
	m_context->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	m_context->BindTexture( nullptr );
	m_context->BindShader( (Shader*)nullptr );

	if( m_rootWidget )
	{
		m_rootWidget->Render();
	}

	m_context->EndCamera( m_camera );

	m_context->CopyTexture( backbuffer, colorTarget );
	m_camera.SetColorTarget( nullptr );
	m_context->ReleaseRenderTarget( colorTarget );
}

void UIManager::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	Vec2 mousePos = g_theInput->GetMouseNormalizedPos();
	mousePos = m_screenBounds.GetPointAtUV( mousePos );

	m_rootWidget->UpdateHovered( mousePos );
	m_rootWidget->CheckInput( deltaSeconds );
	m_rootWidget->UpdateDrag();
}

