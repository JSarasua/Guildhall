#pragma once
#include "Game/Server.hpp"
#include <vector>

class Entity;
class SpriteDefinition;
class Game;
class UDPGameConnection;

struct Vertex_PCU;
struct InputPacket;

enum eGameState : int;

class RemoteServer : public Server
{
public:
	RemoteServer( Game* game );
	virtual ~RemoteServer();

	virtual void Startup() override;
	virtual void Shutdown() override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	virtual void RestartGame() override;
	virtual void PauseGame() override;
	virtual void UnpauseGame() override;

	virtual void Update( float deltaSeconds ) override;
	virtual void UpdateGameState( eGameState newGamestate ) override;

	virtual int GetPlayerHealth() override;
	virtual int GetBossHealth() override;
	virtual SpriteDefinition const* GetPlayerWeaponSprite() const override;

	virtual std::vector<Vertex_PCU> const& GetTileVertsToRender() override;
	virtual std::vector<Entity*> const& GetEntitiesToRender() override;
	virtual eGameState GetCurrentGameState() override;

	void SendUnACKedMessages();
	void ACKMessageServerSent( uint32_t sequenceNo );
	void ACKReceivedMessage( uint32_t sequenceNo );

	bool HandleReceiveTCPMessage( EventArgs const& args );
	bool HandleInput( EventArgs const& args );


public:
	UDPGameConnection* m_UDPGameConnection = nullptr;
	int m_port = -1;
	uint32_t m_currentSequenceNo = 0;
	std::map< uint32_t, InputPacket > m_UnACKedMessages;
};