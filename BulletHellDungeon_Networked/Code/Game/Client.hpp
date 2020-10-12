#pragma once

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
};





