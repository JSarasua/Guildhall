#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game.hpp"

class App
{
public:
	App();
	~App();
	void Startup();
	void Shutdown();
	void RunFrame();

	bool IsQuitting() const { return m_isQuitting; } //return m_isQuitting
	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );
	bool HandleQuitRequested();

	bool IsUpArrowPressed();
	bool IsLeftArrowPressed();
	bool IsRightArrowPressed();
	bool IsSlowed();

	bool IsNoClipping();

	void BeginFrame();
	void Update(float deltaSeconds);
	void Render();
	void EndFrame();

	void RestartGame();

	bool GetDebugGameMode();
	bool GetDebugCameraMode();


private:
	void CheckButtonPresses();
	void CheckController();

private:
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_isSlowed = false;
	bool m_isUpArrowPressed = false;
	bool m_isLeftArrowPressed = false;
	bool m_isRightArrowPressed = false;
	bool m_isLeftActuallyPressed = false;
	bool m_isRightActuallyPressed = false;
	bool m_isSpaceStillHeld = false;
	bool m_isOstillHeld = false;
	Game* m_game;
	float m_previousTime = 0.f;
	float m_currentTime = 0.f;
	float m_deltaTime = 0.f;

	bool m_debugGameMode = false;
	bool m_debugCameraMode = false;
	bool m_noClip = false;
	bool m_isSpedUp = false;

};