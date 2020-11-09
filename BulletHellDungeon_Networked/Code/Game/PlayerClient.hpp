#pragma once
#include "Game/Client.hpp"

class UDPGameConnection;

class PlayerClient : public Client
{
public:
	PlayerClient();
	PlayerClient( UDPGameConnection* clientUDPSocket );
	virtual ~PlayerClient();


	virtual void Startup() override;
	virtual void Shutdown() override;

	virtual void BeginFrame() override;
	virtual void EndFrame() override;

	virtual void Update( float deltaSeconds ) override;
	virtual void UpdateCamera() override;
	virtual void Render() override;

	virtual void UpdateDebugMouse() override;

	//Update State
	virtual void UpdateGameState() override;
	virtual void UpdateLoading( float deltaSeconds ) override;
	virtual void UpdateAttract( float deltaSeconds ) override;
	virtual void UpdateDeath( float deltaSeconds ) override;
	virtual void UpdateVictory( float deltaSeconds ) override;
	virtual void UpdatePaused( float deltaSeconds ) override;
	virtual void UpdatePlaying( float deltaSeconds ) override;

	//RenderState
	virtual void RenderLoading() override;
	virtual void RenderAttract() override;
	virtual void RenderDeath() override;
	virtual void RenderVictory() override;
	virtual void RenderPaused() override;
	virtual void RenderPlaying() override;

	//New render methods
	virtual void BeginRender() override;
	virtual void EndRender() override;

	virtual void RenderMouse() override;
	virtual void RenderGame() override;
	virtual void RenderWorld() override;
	virtual void RenderEntities() override;
	virtual void RenderUI() override;
	virtual void RenderConsole() override;

	virtual void CheckButtonPresses() override;

public:
public:
	Camera* m_camera = nullptr;
	Camera* m_UICamera = nullptr;

	Texture* m_backBuffer = nullptr;
	Texture* m_colorTarget = nullptr;

	int m_frameCounter = 0;

	enum eGameState m_gameState = (eGameState)0;

	//Mouse
	Vec2 m_mousePositionOnMainCamera;
	Vec2 m_mousePositionOnUICamera;

	//UI
	AABB2 m_pausedMenu;
	AABB2 m_pausedResumeButton;
	AABB2 m_pausedRestartButton;
	AABB2 m_pausedQuitButton;
	Rgba8 m_pausedResumeButtonTint= Rgba8::WHITE;
	Rgba8 m_pausedRestartButtonTint = Rgba8::WHITE;
	Rgba8 m_pausedQuitButtonTint = Rgba8::WHITE;
	bool m_isMouseOverPausedResume = false;
	bool m_isMouseOverPausedRestart = false;
	bool m_isMouseOverPausedQuit = false;

	bool m_isMouseOverMainMenuPlay = false;
	bool m_isMouseOverMainMenuQuit = false;

	AABB2 m_deadMenu;
	AABB2 m_deadYOUDIED;
	AABB2 m_deadContinueButton;
	Rgba8 m_deadYOUDIEDTint = Rgba8::RED;

	Timer m_deathTimer;
	bool m_isMouseOverDeadContinue = false;

	Timer m_victoryTimer;
	AABB2 m_victoryMenu;
	AABB2 m_victoryContinueButton;
	bool m_isMouseOverVictoryContinue = false;

private:
	UDPGameConnection* m_UDPConnection = nullptr;
	int m_playerID = -1;
};