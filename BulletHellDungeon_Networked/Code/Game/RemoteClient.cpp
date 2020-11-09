#include "Game/RemoteClient.hpp"
#include "Game/Game.hpp"
#include "Game/Server.hpp"
#include "Game/Actor.hpp"
#include "Game/App.hpp"
#include "Game/Entity.hpp"
#include "Game/AudioDefinition.hpp"
#include "Game/UDPGameConnection.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include <vector>

RemoteClient::RemoteClient()
{

}

RemoteClient::RemoteClient( UDPGameConnection* newUDPConnection ) :
	m_UDPConnection( newUDPConnection )
{

}

RemoteClient::~RemoteClient()
{

}

void RemoteClient::Startup()
{
	g_theInput->HideSystemCursor();

	m_deathTimer.SetSeconds( 2.5 );
	m_deathTimer.Stop();

	m_victoryTimer.SetSeconds( 2.5 );
	m_victoryTimer.Stop();
}

void RemoteClient::Shutdown()
{

}

void RemoteClient::BeginFrame()
{
	if( m_frameCounter == 1 )
	{
		g_theGame->LoadAssets();
		g_theGame->InitializeDefinitions();
	}


	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses();
		UpdateGameState();
	}
}

void RemoteClient::EndFrame()
{
}

void RemoteClient::Update( float deltaSeconds )
{
	UpdateGameState();

	UpdateCamera();

	switch( m_gameState )
	{
	case LOADING: UpdateLoading( deltaSeconds );
		break;
	case ATTRACT: UpdateAttract( deltaSeconds );
		break;
	case PLAYING: UpdatePlaying( deltaSeconds );
		break;
	case PAUSED: UpdatePaused( deltaSeconds );
		break;
	case DEATH: UpdateDeath( deltaSeconds );
		break;
	case VICTORY: UpdateVictory( deltaSeconds );
		break;
	default: ERROR_AND_DIE( "Invalid Game State" );
		break;
	}


	if( g_theGame->m_player )
	{
		if( !g_theGame->m_player->IsAlive() )
		{
			if( !m_deathTimer.IsRunning() )
			{
				m_deathTimer.Reset();
				AudioDefinition::StopAllSounds();
			}
		}
	}
}

void RemoteClient::UpdateCamera()
{
	m_camera = g_theGame->GetCameraPointer();
	m_UICamera = g_theGame->GetUICameraPointer();
}

void RemoteClient::Render()
{

}

void RemoteClient::UpdateDebugMouse()
{

}

void RemoteClient::UpdateGameState()
{
	m_gameState = g_theServer->GetCurrentGameState();
}

void RemoteClient::UpdateLoading( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if( m_frameCounter > 1 )
	{

	}
	else if( m_frameCounter == 1 )
	{

	}
	else if( m_frameCounter == 0 )
	{

	}

	if( m_frameCounter == 2 )
	{

	}

	m_frameCounter++;
}

void RemoteClient::UpdateAttract( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void RemoteClient::UpdateDeath( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void RemoteClient::UpdateVictory( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void RemoteClient::UpdatePaused( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void RemoteClient::UpdatePlaying( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	// 	g_theGame->Update( deltaSeconds );
	// 	g_theGame->UpdateCamera( deltaSeconds );
	UpdateDebugMouse();
}

void RemoteClient::RenderLoading()
{

}

void RemoteClient::RenderAttract()
{

}

void RemoteClient::RenderDeath()
{

}

void RemoteClient::RenderVictory()
{

}

void RemoteClient::RenderPaused()
{

}

void RemoteClient::RenderPlaying()
{

}

void RemoteClient::BeginRender()
{
	g_theRenderer->ClearScreen( Rgba8::BLACK );

	m_backBuffer = g_theRenderer->GetBackBuffer();
	m_colorTarget = g_theRenderer->AcquireRenderTargetMatching( m_backBuffer );
	m_camera->SetColorTarget( 0, m_colorTarget );
	m_UICamera->SetColorTarget( 0, m_colorTarget );
}

void RemoteClient::EndRender()
{

	g_theRenderer->CopyTexture( m_backBuffer, m_colorTarget );
	m_camera->SetColorTarget( nullptr );
	g_theRenderer->ReleaseRenderTarget( m_colorTarget );

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( m_camera );
	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
	DebugRenderEndFrame();
}

void RemoteClient::RenderMouse()
{
}

void RemoteClient::RenderGame()
{
	RenderWorld();
	RenderEntities();
}

void RemoteClient::RenderWorld()
{
	Texture const& tex = g_tileSpriteSheet->GetTexture();
	g_theRenderer->BindTexture( &tex );

	std::vector<Vertex_PCU> const& verts = g_theServer->GetTileVertsToRender();
	g_theRenderer->DrawVertexArray( verts );
}

void RemoteClient::RenderEntities()
{
	std::vector<Entity*> const& entitiesToRender= g_theServer->GetEntitiesToRender();

	for( size_t entityIndex = 0; entityIndex < entitiesToRender.size(); entityIndex++ )
	{
		Entity* entityToRender = entitiesToRender[entityIndex];

		if( entityToRender )
		{
			entityToRender->Render();
		}
	}
}

void RemoteClient::RenderUI()
{

}

void RemoteClient::RenderConsole()
{
}

void RemoteClient::CheckButtonPresses()
{
	if( m_gameState == PLAYING )
	{
		AddressedInputPacket inputPacket = m_UDPConnection->PopFirstReceivedPacket();
		if( inputPacket.isValid )
		{
			InputMessage const& input = inputPacket.packet.message;
			int changeWeapons = input.changeWeapons;
			bool isDodging = input.isDodging;
			bool isShooting = input.isShooting;
			Vec2 mousePos = input.mousePos;
			Vec2 moveVec = input.moveVec;


			EventArgs args;
			args.SetValue( "changeWeapons", changeWeapons );
			args.SetValue( "isDodging", isDodging );
			args.SetValue( "isShooting", isShooting );
			args.SetValue( "mousePos", mousePos );
			args.SetValue( "moveVec", moveVec );
			args.SetValue( "playerID", m_playerID );
			g_theEventSystem->FireEvent( "Input", NOCONSOLECOMMAND, &args );
		}
	}
}

void RemoteClient::SetUDPSocket( UDPGameConnection* newUDPConnection )
{
	if( m_UDPConnection )
	{
		delete m_UDPConnection;
		m_UDPConnection = nullptr;
	}

	m_UDPConnection = newUDPConnection;
}

