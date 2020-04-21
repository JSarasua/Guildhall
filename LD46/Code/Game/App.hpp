#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
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

	bool IsNoClipping();

	void BeginFrame();
	void Update();
	void Render();
	void EndFrame();

	void RestartGame();

	bool GetDebugGameMode();
	bool GetDebugCameraMode();

	static bool QuitRequested(const EventArgs* args);

private:
	void CheckButtonPresses();
	void CheckController();

public:
	Game* m_game = nullptr;

private:
 	bool m_isQuitting = false;

	bool m_debugGameMode = false;
	bool m_debugCameraMode = false;
	bool m_noClip = false;
	bool m_isSpedUp = false;
	Camera* m_devConsoleCamera = nullptr;

};