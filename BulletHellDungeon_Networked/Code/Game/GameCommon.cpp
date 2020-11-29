#pragma once
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
//#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Platform/Window.hpp"

App* g_theApp = nullptr;
Game* g_theGame = nullptr;
Server* g_theServer = nullptr;
Client* g_theClient = nullptr;
RenderContext* g_theRenderer = nullptr;
/*InputSystem* g_theInput = nullptr;*/
Window* g_theWindow = nullptr;
AudioSystem* g_theAudio = nullptr;

SpriteSheet* g_tileSpriteSheet = nullptr;
SpriteSheet* g_actorSpriteSheet = nullptr;
SpriteSheet* g_portraitSpriteSheet = nullptr;
SpriteSheet* g_weaponSpriteSheet = nullptr;
SpriteSheet* g_bulletsSpriteSheet = nullptr;

const char* IMAGETESTPATH = "Data/Images/Test_StbiAndDirectX.png";

const IntVec2 STEP_NORTH = IntVec2( 1, 0 );
const IntVec2 STEP_WEST = IntVec2( 0, -1 );
const IntVec2 STEP_SOUTH = IntVec2( -1, 0 );
const IntVec2 STEP_EAST = IntVec2( 0, 1 );

const IntVec2 STEP_NORTHWEST = IntVec2( 1, -1 );
const IntVec2 STEP_SOUTHWEST = IntVec2( -1, -1 );
const IntVec2 STEP_SOUTHEAST = IntVec2( -1, 1 );
const IntVec2 STEP_NORTHEAST = IntVec2( 1, 1 );


ByteMessage UDPPacket::ToByteMessage()
{
	int size = header.m_size;
	size = ClampInt( size, 0, MAXUDPMESSAGESIZE );
	size += sizeof( header ) + sizeof( m_gameID );

	ByteMessage byteMessage;
	byteMessage.size = size;
	memcpy( byteMessage.message, this, size );

	return byteMessage;
}



