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

}

void RemoteClient::Shutdown()
{}

void RemoteClient::BeginFrame()
{
	CheckButtonPresses();
}

void RemoteClient::EndFrame()
{}

void RemoteClient::Update( float deltaSeconds )
{}

void RemoteClient::UpdateCamera()
{}

void RemoteClient::Render()
{}

void RemoteClient::UpdateDebugMouse()
{}

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
}

void RemoteClient::RenderLoading()
{}

void RemoteClient::RenderAttract()
{}

void RemoteClient::RenderDeath()
{}

void RemoteClient::RenderVictory()
{}

void RemoteClient::RenderPaused()
{}

void RemoteClient::RenderPlaying()
{}

void RemoteClient::BeginRender()
{}

void RemoteClient::EndRender()
{}

void RemoteClient::RenderMouse()
{}

void RemoteClient::RenderGame()
{}

void RemoteClient::RenderWorld()
{}

void RemoteClient::RenderEntities()
{}

void RemoteClient::RenderUI()
{}

void RemoteClient::RenderConsole()
{}

void RemoteClient::CheckButtonPresses()
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
		args.SetValue( "actorID", m_playerID );
		g_theEventSystem->FireEvent( "UpdateInput", NOCONSOLECOMMAND, &args );
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

