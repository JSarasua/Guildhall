#pragma once
//#include "Game/Game.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Core/Rgba8.hpp"
//#include "Engine/Network/UDPSocket.hpp"

class Game;
class Camera;
class Texture;

enum eGameState : int;


class Client
{
public:
	Client();
	virtual ~Client();

	virtual void Startup() = 0;
	virtual void Shutdown() = 0;

	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void Update( float deltaSeconds ) = 0;
	virtual void UpdateCamera() = 0;
	virtual void Render() = 0;

	virtual void UpdateDebugMouse() = 0;

	//Update State
	virtual void UpdateGameState() = 0;
	virtual void UpdateLoading( float deltaSeconds ) = 0;
	virtual void UpdateAttract( float deltaSeconds ) = 0;
	virtual void UpdateDeath( float deltaSeconds ) = 0;
	virtual void UpdateVictory( float deltaSeconds ) = 0;
	virtual void UpdatePaused( float deltaSeconds ) = 0;
	virtual void UpdatePlaying( float deltaSeconds ) = 0;

	//RenderState
	virtual void RenderLoading() = 0;
	virtual void RenderAttract() = 0;
	virtual void RenderDeath() = 0;
	virtual void RenderVictory() = 0;
	virtual void RenderPaused() = 0;
	virtual void RenderPlaying() = 0;

	//New render methods
	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;

	virtual void RenderMouse() = 0;
	virtual void RenderGame() = 0;
	virtual void RenderWorld() = 0;
	virtual void RenderEntities() = 0;
	virtual void RenderUI() = 0;
	virtual void RenderConsole() = 0;

	virtual void CheckButtonPresses() = 0;

	void SetPlayerID( int playerID ) { m_playerID = playerID; }

public:
	int m_playerID = 0;
// 	Camera* m_camera = nullptr;
// 	Camera* m_UICamera = nullptr;
// 
// 	Texture* m_backBuffer = nullptr;
// 	Texture* m_colorTarget = nullptr;
// 
// 	int m_frameCounter = 0;
// 
// 	enum eGameState m_gameState = (eGameState)0;
// 
// 	//Mouse
// 	Vec2 m_mousePositionOnMainCamera;
// 	Vec2 m_mousePositionOnUICamera;
// 
// 	//UI
// 	AABB2 m_pausedMenu;
// 	AABB2 m_pausedResumeButton;
// 	AABB2 m_pausedRestartButton;
// 	AABB2 m_pausedQuitButton;
// 	Rgba8 m_pausedResumeButtonTint= Rgba8::WHITE;
// 	Rgba8 m_pausedRestartButtonTint = Rgba8::WHITE;
// 	Rgba8 m_pausedQuitButtonTint = Rgba8::WHITE;
// 	bool m_isMouseOverPausedResume = false;
// 	bool m_isMouseOverPausedRestart = false;
// 	bool m_isMouseOverPausedQuit = false;
// 
// 	bool m_isMouseOverMainMenuPlay = false;
// 	bool m_isMouseOverMainMenuQuit = false;
// 
// 	AABB2 m_deadMenu;
// 	AABB2 m_deadYOUDIED;
// 	AABB2 m_deadContinueButton;
// 	Rgba8 m_deadYOUDIEDTint = Rgba8::RED;
// 
// 	Timer m_deathTimer;
// 	bool m_isMouseOverDeadContinue = false;
// 
// 	Timer m_victoryTimer;
// 	AABB2 m_victoryMenu;
// 	AABB2 m_victoryContinueButton;
// 	bool m_isMouseOverVictoryContinue = false;
};





