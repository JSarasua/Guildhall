#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Time/Timer.hpp"

enum eGameState
{
	LOADING,
	ATTRACT,
 	DEATH,
// 	VICTORY,
	PAUSED,
	PLAYING,

	NUM_OF_GAMESTATES
};

class Actor;
class World;


class Game
{
public:
	Game();
	~Game();
	void Startup();
	void Shutdown();
	void RunFrame();

	void Update(float deltaSeconds);
	void Render();

	Vec2 GetMousePositionOnMainCamera();
	AABB2 GetUICamera() const;
	AABB2 GetCamera() const;
	void AddScreenShake( float screenShakeIncrement );

private:
	//Update State
	void UpdateLoading( float deltaSeconds );
	void UpdateAttract( float deltaSeconds );
	void UpdateDeath( float deltaSeconds );
	void UpdateVictory( float deltaSeconds );
	void UpdatePaused( float deltaSeconds );
	void UpdatePlaying( float deltaSeconds );

	//RenderState
	void RenderLoading();
	void RenderAttract();
	void RenderDeath();
	void RenderVictory();
	void RenderPaused();
	void RenderPlaying();

	//Add Tests
	void AddDevConsoleTest();
	void AddAlignedTextTest();
	void AddImageTest();
	void AddBlackboardTest();



	//Updates
	void UpdateCamera( float deltaSeconds );
	void UpdateCamera( const Vec2& centerPosition, Camera& camera );
	void UpdateCameras();
	void UpdateConsoleTest( float deltaSeconds );
	void UpdateAlignedTextTest( float deltaSeconds );
	void UpdateImageTest( float deltaSeconds );
	void UpdateBlackboardTest( float deltaSeconds );
	void UpdateDebugMouse();

	//Renders
	void RenderGame();
	void RenderMouse();
	void RenderUI();
	void RenderConsole() const;
	void RenderAlignedTextTest() const;
	void RenderImageTest() const;
	void RenderBlackboardTest() const;
	void RenderMouseTest() const;
	void RenderDebugMouse();

	//XML,Texture, Audio loading
	void InitializeDefinitions();
	void LoadAssets();

	//Input
	void CheckButtonPresses(float deltaSeconds);
	
	//Misc
	IntVec2 GetCurrentMapBounds() const;
	void	RebuildWorld();

private:
	World* m_world = nullptr;
	Actor* m_player = nullptr;

	int m_currentLevel = 0;

	float m_numTilesInViewVertically = 0.f;
	float m_numTilesInViewHorizontally = 0.f;

	//Cameras
	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	Camera m_camera;
	std::vector<Camera> m_cameras;
	//Vec2 m_cameraPosition;
	Vec2 m_mousePositionOnMainCamera;
	Vec2 m_mousePositionOnUICamera;

	//DevConsole
	bool m_isDevConsoleTestActive = false;

	//AlignedTest
	bool m_isAlignedTextTestActive = false;
	std::string m_alignedTestText;
	Vec2 m_alignment = ALIGN_BOTTOM_LEFT;
	AABB2 m_alignedTextTestBox;

	//ImageTest
	bool m_isImageTestActive = false;
	Image m_imageTest;
	AABB2 m_imageTextTestBox;
	AABB2 m_imageTestBox;
	Rgba8 m_pixelColor;
	std::string m_pixelToString;

	//BlackboardTest
	bool m_isBlackboardTestActive = false;
	AABB2 m_blackBoardTextBox;
	std::string m_blackBoardText;

	//XML Definitions
	XmlDocument* m_mapDefDoc	= nullptr;
	XmlDocument* m_tileDefDoc	= nullptr;
	XmlDocument* m_actorDefDoc	= nullptr;


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
	AABB2 m_deadRestartButton;
	AABB2 m_deadQuitButton;
	Rgba8 m_deadYOUDIEDTint = Rgba8::RED;
	Rgba8 m_deadRestartButtonTint = Rgba8::WHITE;
	Rgba8 m_deadQuitButtonTint = Rgba8::WHITE;
	Timer m_deathTimer;

public:
	eGameState m_gameState = LOADING;
	//Rand for game to use
	RandomNumberGenerator m_rand;

	int m_frameCounter = 0;
};
