#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Core/Rgba8.hpp"

class Camera;
class Texture;

class Client
{
public:
	Client();
	~Client();

	void Startup();
	void Shutdown();

	void BeginFrame();
	void EndFrame();

	void Update( float deltaSeconds );
	void UpdateCamera();
	void Render();


	//RenderState
	void RenderLoading();
	void RenderAttract();
	void RenderDeath();
	void RenderVictory();
	void RenderPaused();
	void RenderPlaying();

	//New render methods
	void BeginRender();
	void EndRender();


	void RenderWorld();
	void RenderEntities();
	void RenderUI();

	void CheckInput();

public:
	Camera* m_camera = nullptr;
	Camera* m_UICamera = nullptr;

	Texture* m_backBuffer = nullptr;
	Texture* m_colorTarget = nullptr;


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
};





