#pragma once
#include "Engine/Math/vec2.hpp"
#include <string>
#include <vector>
#include "Engine/Math/MathUtils.hpp"

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
constexpr uint32_t UDPIDENTIFIER = 5678;
constexpr uint32_t TCPIDENTIFIER = 1234;

constexpr uint16_t ADDPLAYER = 4;
constexpr uint16_t ADDENTITY = 5;
constexpr uint16_t UPDATEPLAYER = 6;
constexpr uint16_t UPDATEENTITY = 7;
constexpr uint16_t DELETEENTITY = 8;
constexpr uint16_t VERIFIEDPACKET = 9;
constexpr uint16_t NEXTMAP = 10;

constexpr int MAXUDPMESSAGESIZE = 200;

constexpr int ID_PLAYER = 10;
constexpr int ID_ACTOR = 11;
constexpr int ID_GOOD_BULLET = 12;
constexpr int ID_EVIL_BULLET = 13;
constexpr int ID_LOOT = 14;

constexpr int FRAMESPERPACKET = 3;

struct ByteMessage;

struct Header
{
	uint16_t m_id = 0;
	uint16_t m_size = 0;
	uint32_t m_sequenceNo = 0;
};

struct TCPMessage
{
	uint16_t m_id = 0;
	uint16_t m_port = 0;
	uint16_t m_playerID = 0;
	uint32_t m_gameID = TCPIDENTIFIER;

	static TCPMessage ToMessage( char const* messageStr )
	{
		TCPMessage message = *(TCPMessage*)messageStr;
		return message;
	}
};



struct UDPPacket
{

	Header header;
	uint32_t m_gameID = UDPIDENTIFIER;
	char message[MAXUDPMESSAGESIZE]{};

	void SetMessage( char const* messageStr, int size )
	{
		memset(message, 0, MAXUDPMESSAGESIZE );
		memcpy( message, messageStr, size );
	}

	static UDPPacket ToPacket( char const* packetStr, int size )
	{
		Header* newHeader = (Header*)packetStr;
		uint32_t* newID = (uint32_t*)(packetStr + sizeof(header));
		char const* newMessage = packetStr + sizeof(header) + sizeof(uint32_t);

		UDPPacket newPacket;
		newPacket.header = *newHeader;
		newPacket.m_gameID = *newID;

		memset( &newPacket.message[0], 0, MAXUDPMESSAGESIZE );
		
		int messageSize = ClampInt( size - (sizeof(header) + sizeof(uint32_t)), 0, MAXUDPMESSAGESIZE );
		memcpy( &newPacket.message[0], newMessage, messageSize );
		return newPacket;
	}

	std::string ToString()
	{
		int size = header.m_size;
		size = ClampInt( size, 0, MAXUDPMESSAGESIZE );

		std::string packetStr;
		packetStr.append( (char*)&header, sizeof(header) );
		packetStr.append( (char*)&m_gameID, sizeof(m_gameID) );
		packetStr.append( (char*)&message, size );
		return packetStr;
	}

	ByteMessage ToByteMessage();
// 	{
// 		int size = header.m_size;
// 		size = ClampInt( size, 0, MAXUDPMESSAGESIZE );
// 		size += sizeof(header) + sizeof(m_gameID);
// 
// 		ByteMessage byteMessage;
// 		byteMessage.size = size;
// 		memcpy( byteMessage.message, this, size );
// 
// 		return byteMessage;
// 	}
};

struct ByteMessage
{
	char message[sizeof(UDPPacket)]{};
	int size = 0;
};



struct AddressedUDPPacket
{
	bool isValid = false;
	std::string IPAddress;
	UDPPacket packet;
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
	uint32_t m_gameID = UDPIDENTIFIER;
	InputMessage message;

	static InputPacket ToPacket( char const* packetStr )
	{
		Header* newHeader = (Header*)packetStr;
		uint32_t* newID = (uint32_t*)(packetStr + sizeof(header));
		InputMessage* newMessage = (InputMessage*)(packetStr + sizeof(header) + sizeof(m_gameID));
		
		InputPacket newPacket;
		newPacket.header = *newHeader;
		newPacket.m_gameID = *newID;
		newPacket.message = *newMessage;
		return newPacket;
	}

public:
	std::string ToString()
	{
		std::string inputPacketStr;
		inputPacketStr.append( (char*)&header, sizeof(Header) );
		inputPacketStr.append( (char*)&m_gameID, sizeof(uint32_t) );
		inputPacketStr.append( (char*)&message, 22 );

		return inputPacketStr;
	}

	ByteMessage ToByteMessage()
	{
		int size = sizeof(InputPacket);
		size = ClampInt( size, 0, MAXUDPMESSAGESIZE );
		//size += sizeof( header ) + sizeof( m_gameID );

		ByteMessage byteMessage;
		byteMessage.size = size;
		memcpy( byteMessage.message, this, size );

		return byteMessage;
	}
};

struct AddressedInputPacket
{
	bool isValid = false;
	std::string IPAddress;
	InputPacket packet;
};

struct CreateEntityMessage
{
	Vec2 initialPosition;
	Vec2 initialVelocity;
	float initialOrientationDegrees;
	float speedMultiplier;
	int entityType;
	int defIndex = -1;
	int entityID = -1;
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
	int entityID = -1;
};

struct DeleteEntityMessage
{
	int entityID = -1;
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
		AddPlayerMessage* newMessage = (AddPlayerMessage*)(packetStr + sizeof(Header));

		AddPlayerPacket newPacket;
		newPacket.header = *newHeader;
		newPacket.message = *newMessage;
		return newPacket;
	}

	std::string ToString()
	{
		std::string packetStr;
		packetStr.append( (char*)&header, sizeof(Header) );
		packetStr.append( (char*)&message, 22 );

		return packetStr;
	}
};
