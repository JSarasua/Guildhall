#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Image.hpp"


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

private:

	void LoadAssets();
	void UpdateCamera( float deltaSeconds );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);
	IntVec2 GetCurrentMapBounds() const;




private:

	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	Camera m_camera;
	Vec2 m_cameraPosition;

	int m_currentLevel = 0;

	World* m_world = nullptr;
	Actor* m_player = nullptr;


	float m_numTilesInViewVertically = 0.f;
	float m_numTilesInViewHorizontally = 0.f;
public:

	RandomNumberGenerator m_rand;

private:
	void AddDevConsoleTest();
	bool m_isDevConsoleTestActive = false;

	void AddAlignedTextTest();
	bool m_isAlignedTextTestActive = false;
	std::string m_alignedTestText;
	Vec2 m_alignment = ALIGN_BOTTOM_LEFT;
	AABB2 m_alignedTextTestBox;

	void AddImageTest();
	bool m_isImageTestActive = false;
	Image m_imageTest;
	AABB2 m_imageTextTestBox;
	AABB2 m_imageTestBox;
	Rgba8 m_pixelColor;
	std::string m_pixelToString;

	void AddBlackboardTest();
	bool m_isBlackboardTestActive = false;
	AABB2 m_blackBoardTextBox;
	std::string m_blackBoardText;

	void UpdateConsoleTest( float deltaSeconds );
	void UpdateAlignedTextTest( float deltaSeconds );
	void UpdateImageTest( float deltaSeconds );
	void UpdateBlackboardTest( float deltaSeconds );

	void RenderConsoleTest() const;
	void RenderAlignedTextTest() const;
	void RenderImageTest() const;
	void RenderBlackboardTest() const;



	void InitializeDefinitions();


private:
	XMLDocument* m_mapDefDoc	= nullptr;
	XMLDocument* m_tileDefDoc	= nullptr;
	XMLDocument* m_actorDefDoc	= nullptr;
};
