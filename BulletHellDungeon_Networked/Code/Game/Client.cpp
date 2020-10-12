#include "Game/Client.hpp"
#include "Game/Game.hpp"
#include "Game/Server.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>



Client::Client()
{

}

Client::~Client()
{

}

void Client::Startup()
{

}

void Client::Shutdown()
{

}

void Client::BeginFrame()
{

}

void Client::EndFrame()
{

}

void Client::Update( float deltaSeconds )
{
	UpdateCamera();

}

void Client::UpdateCamera()
{
	m_camera = g_theGame->GetCameraPointer();
	m_UICamera = g_theGame->GetUICameraPointer();
}

void Client::Render()
{
	//g_theGame->Render();
	
	//BeginRender();

	eGameState currentGamestate = g_theServer->GetCurrentGameState();
	switch( currentGamestate )
	{
	case LOADING: RenderLoading();
		break;
	case ATTRACT: RenderAttract();
		break;
	case DEATH: RenderDeath();
		break;
	case VICTORY: RenderVictory();
		break;
	case PAUSED: RenderPaused();
		break;
	case PLAYING: RenderPlaying();
		break;
	default: ERROR_AND_DIE( "Invalid Game State" );
		break;
	}

	//EndRender();

}

void Client::RenderLoading()
{
	//g_theGame->RenderLoading();
	BeginRender();
	Texture* loadingScreenTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/LoadingScreen.png" );

	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	g_theRenderer->BindTexture( loadingScreenTex );
	g_theRenderer->BindShader( (Shader*)nullptr );
	AABB2 cameraBounds = AABB2( m_camera->GetOrthoBottomLeft(), m_camera->GetOrthoTopRight() );
	g_theRenderer->DrawAABB2Filled( cameraBounds, Rgba8::WHITE );
	EndRender();
}

void Client::RenderAttract()
{
	g_theGame->RenderAttract();
}

void Client::RenderDeath()
{
	g_theGame->RenderDeath();
}

void Client::RenderVictory()
{
	g_theGame->RenderVictory();
}

void Client::RenderPaused()
{
	g_theGame->RenderPaused();
}

void Client::RenderPlaying()
{
	g_theGame->RenderPlaying();
}

void Client::BeginRender()
{
	g_theRenderer->ClearScreen( Rgba8::BLACK );

	 m_backBuffer = g_theRenderer->GetBackBuffer();
	m_colorTarget = g_theRenderer->AcquireRenderTargetMatching( m_backBuffer );
	m_camera->SetColorTarget( 0, m_colorTarget );

	g_theRenderer->BeginCamera( *m_camera );
}

void Client::EndRender()
{
	g_theRenderer->EndCamera( *m_camera );

	g_theRenderer->CopyTexture( m_backBuffer, m_colorTarget );
	m_camera->SetColorTarget( nullptr );
	g_theRenderer->ReleaseRenderTarget( m_colorTarget );
}

void Client::RenderWorld()
{
	Texture const& tex = g_tileSpriteSheet->GetTexture();
	g_theRenderer->BindTexture( &tex );

	std::vector<Vertex_PCU> const& verts = g_theServer->GetTileVertsToRender();
	g_theRenderer->DrawVertexArray( verts );
}

