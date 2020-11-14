#pragma once
#include "Engine/Math/vec2.hpp"
#include <string>
#include <vector>

struct Rgba8;
struct IntVec2;

class App;
class Game;
class Window;
class RenderContext;
class AudioSystem;
/*class InputSystem;*/
class SpriteSheet;
class Server;
class Client;

constexpr int PLAYER_HEALTH = 100;
constexpr int ENEMY_HEALTH = 30;
constexpr int BOSS_HEALTH = 1000;
constexpr float CLIENT_ASPECT = 16.f/9.f;

//Given Constants
constexpr int INT_MAP_SIZE_X = 70;
constexpr int INT_MAP_SIZE_Y = 30;
constexpr int SAFEZONE = 3;
constexpr float GAME_CAMERA_X = 13.5f;
constexpr float GAME_CAMERA_Y = GAME_CAMERA_X/2.f;

constexpr float UI_CAMERA_Y = 90.f;
constexpr float UI_CAMERA_X = UI_CAMERA_Y * CLIENT_ASPECT;
constexpr float TESTTEXTLINEHEIGHT = 1.f;
extern const char* IMAGETESTPATH;// = "/Data/Images/Test_StbiAndDirectX.png";

extern const IntVec2 STEP_NORTH;
extern const IntVec2 STEP_WEST;
extern const IntVec2 STEP_SOUTH;
extern const IntVec2 STEP_EAST;

extern const IntVec2 STEP_NORTHWEST;
extern const IntVec2 STEP_SOUTHWEST;
extern const IntVec2 STEP_SOUTHEAST;
extern const IntVec2 STEP_NORTHEAST;

extern App* g_theApp;
extern Game* g_theGame;
extern Server* g_theServer;
extern Client* g_theClient;
extern Window* g_theWindow;
extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudio;
/*extern InputSystem* g_theInput;*/
extern SpriteSheet* g_tileSpriteSheet;
extern SpriteSheet* g_actorSpriteSheet;
extern SpriteSheet* g_portraitSpriteSheet;
extern SpriteSheet* g_weaponSpriteSheet;
extern SpriteSheet* g_bulletsSpriteSheet;

//constexpr uint16_t SERVERLISTENING = 1;
//constexpr uint16_t TEXTMESSAGE = 2;
//constexpr uint16_t CLIENTDISCONNECT = 3;
constexpr uint16_t ADDPLAYER = 4;
constexpr uint16_t ADDENTITY = 5;
constexpr uint16_t UPDATEPLAYER = 6;

struct Header
{
	uint16_t m_id = 0;
	uint16_t m_size = 0;
};

struct TCPMessage
{
	uint16_t m_id = 0;
	uint16_t m_port = 0;
	uint16_t m_playerID = 0;

	static TCPMessage ToMessage( char const* messageStr )
	{
		TCPMessage message = *(TCPMessage*)messageStr;
		return message;
	}
};

struct InputMessage
{
	Vec2 mousePos = Vec2();
	Vec2 moveVec = Vec2();
	int changeWeapons = 0;
	bool isShooting = false;
	bool isDodging = false;
};

struct InputPacket
{
	Header header;
	InputMessage message;

	static InputPacket ToPacket( char const* packetStr )
	{
		Header* newHeader = (Header*)packetStr;
		InputMessage* newMessage = (InputMessage*)(packetStr + 4);
		
		InputPacket newPacket;
		newPacket.header = *newHeader;
		newPacket.message = *newMessage;
		return newPacket;
	}

public:
	std::string ToString()
	{
		std::string inputPacketStr;
		inputPacketStr.append( (char*)&header, 4 );
		inputPacketStr.append( (char*)&message, 22 );

		return inputPacketStr;
	}
};

struct AddressedInputPacket
{
	bool isValid = false;
	std::string IPAddress;
	InputPacket packet;
};


struct UpdateEntityMessage
{
	Vec2 position;
	Vec2 velocity;
	float orientationDegrees;
	float weaponOrientationDegrees;
	float angularVelocity;
	int health;
	bool isDead;

};

struct AddPlayerMessage
{
	Vec2 position;
	int playerID = -1;
};

struct AddPlayerPacket
{
	Header header = Header{ADDPLAYER,0};
	AddPlayerMessage message;


	static AddPlayerPacket ToPacket( char const* packetStr )
	{
		Header* newHeader = (Header*)packetStr;
		AddPlayerMessage* newMessage = (AddPlayerMessage*)(packetStr + 4);

		AddPlayerPacket newPacket;
		newPacket.header = *newHeader;
		newPacket.message = *newMessage;
		return newPacket;
	}

	std::string ToString()
	{
		std::string packetStr;
		packetStr.append( (char*)&header, 4 );
		packetStr.append( (char*)&message, 22 );

		return packetStr;
	}
};
