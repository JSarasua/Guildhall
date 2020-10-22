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

	bool IsQuitting() const { return m_isQuitting; }
	bool HandleQuitRequested();

	bool IsSlowed();

	bool IsNoClipping();

	void BeginFrame();
	void Update(float deltaSeconds);
	void Render();
	void EndFrame();

	void RestartGame();

	bool GetDebugGameMode();
	bool GetDebugCameraMode();

	static bool QuitRequested(const EventArgs* args);

	bool StartMultiplayerServer( EventArgs const& args );
	bool ConnectToMultiplayerServer( EventArgs const& args );

	void TogglePause();
	void PauseGame();
	void UnPauseGame();

private:
	void CheckButtonPresses();
	void CheckController();

private:
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_isSlowed = false;
	float m_previousTime = 0.f;
	float m_currentTime = 0.f;
	float m_deltaTime = 0.f;

	bool m_debugGameMode = false;
	bool m_debugCameraMode = false;
	bool m_noClip = false;
	bool m_isSpedUp = false;

};