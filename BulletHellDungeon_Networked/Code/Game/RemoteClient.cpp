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
	g_theEventSystem->SubscribeMethodToEvent( "CreateEntity", NOCONSOLECOMMAND, this, &RemoteClient::HandleCreateEntity );
	g_theEventSystem->SubscribeMethodToEvent( "UpdateEntity", NOCONSOLECOMMAND, this, &RemoteClient::HandleUpdateEntity );
	g_theEventSystem->SubscribeMethodToEvent( "DeleteEntity", NOCONSOLECOMMAND, this, &RemoteClient::HandleDeleteEntity );
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
{
	UNUSED( deltaSeconds );

	for( auto& packetIter : m_unAckedPackets )
	{
		UDPPacket& packet = packetIter.second;
		std::string packetStr = packet.ToString();
		m_UDPConnection->SendUDPMessage( packetStr );
	}
}

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
	AddressedUDPPacket udpPacket = m_UDPConnection->PopFirstReceivedPacket();
	bool isValid = udpPacket.isValid;
	while( isValid )
	{
		AddressedUDPPacket udpPacketTemp = m_UDPConnection->PopFirstReceivedPacket();

		if( udpPacketTemp.isValid )
		{
			if( udpPacketTemp.packet.header.m_id == VERIFIEDPACKET )
			{
				uint16_t sequenceNoToErase = udpPacketTemp.packet.header.m_sequenceNo;
				m_unAckedPackets.erase( sequenceNoToErase );
			}
			else
			{
				udpPacket = udpPacketTemp;

				UDPPacket returnPacket;
				returnPacket.header = udpPacketTemp.packet.header;
				returnPacket.header.m_size = 0;
				returnPacket.header.m_id = VERIFIEDPACKET;
				returnPacket.SetMessage( "", 0 );

				std::string returnMessage = returnPacket.ToString();
				m_UDPConnection->SendUDPMessage( returnMessage );
			}
		}
		isValid = udpPacketTemp.isValid;
	}
	if( udpPacket.isValid )
	{
		int packetID = udpPacket.packet.header.m_id;

		if( packetID == UPDATEPLAYER )
		{
			InputMessage input = *(InputMessage*)udpPacket.packet.message;

			//InputMessage const& input = udpPacket.packet.message;
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

bool RemoteClient::HandleCreateEntity( EventArgs const& args )
{
	CreateEntityMessage message;

	message.initialPosition = args.GetValue( "position", Vec2() );
	message.initialVelocity = args.GetValue( "velocity", Vec2() );
	message.initialOrientationDegrees = args.GetValue( "orientation", 0.f );
	message.speedMultiplier = args.GetValue( "speedMultiplier", 1.f );
	message.entityType = args.GetValue( "entityType", -1 );
	message.defIndex = args.GetValue( "defIndex", 0 );
	message.entityID = args.GetValue( "entityID", -1 );

	int messageSize = sizeof( message );
	UDPPacket packet;
	packet.header.m_id = ADDENTITY;
	packet.header.m_size = (uint16_t)messageSize;
	packet.header.m_sequenceNo = m_sequenceNo;
	m_sequenceNo++;
	
	char const* messageStr = (char const*)&message;
	packet.SetMessage( messageStr, messageSize );

	std::string createEntityMessage = packet.ToString();

	m_UDPConnection->SendUDPMessage( createEntityMessage );
	return false;
}

bool RemoteClient::HandleUpdateEntity( EventArgs const& args )
{
	UpdateEntityMessage message;
	message.position = args.GetValue("position", Vec2() );
	message.velocity = args.GetValue("velocity", Vec2() );
	message.orientationDegrees = args.GetValue( "orientationDegrees", 0.f );
	message.weaponOrientationDegrees = args.GetValue( "weaponOrientationDegrees", 0.f );
	message.angularVelocity = args.GetValue( "angularVelocity", 0.f );
	message.health = args.GetValue( "health", 0 );
	message.isDead = args.GetValue( "isDead", false );
	message.entityID = args.GetValue( "entityID", -1 );

	int messageSize = sizeof( message );
	UDPPacket packet;
	packet.header.m_id = UPDATEENTITY;
	packet.header.m_size = (uint16_t)messageSize;
	packet.header.m_sequenceNo = m_sequenceNo;
	m_sequenceNo++;

	char const* messageStr = (char const*)&message;
	packet.SetMessage( messageStr, messageSize );

	std::string createEntityMessage = packet.ToString();

	m_UDPConnection->SendUDPMessage( createEntityMessage );

	return false;
}

bool RemoteClient::HandleDeleteEntity( EventArgs const& args )
{
	DeleteEntityMessage message;
	message.entityID = args.GetValue( "entityID", -1 );

	int messageSize = sizeof( message );
	UDPPacket packet;
	packet.header.m_id = DELETEENTITY;
	packet.header.m_size = (uint16_t)messageSize;
	packet.header.m_sequenceNo = m_sequenceNo;
	m_sequenceNo++;

	char const* messageStr = (char const*)&message;
	packet.SetMessage( messageStr, messageSize );

	std::string createEntityMessage = packet.ToString();

	m_UDPConnection->SendUDPMessage( createEntityMessage );

	return false;
}

