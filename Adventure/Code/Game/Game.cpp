#pragma once
#include "Game/Game.hpp"
#include "App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/World.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Actor.hpp"




Game::Game() : m_imageTest(IMAGETESTPATH)
{
	m_camera = Camera();
	m_UICamera = Camera();

	m_world = new World(this);
}

Game::~Game(){}

void Game::Startup()
{

	LoadAssets();
	InitializeDefinitions();
	m_world->Startup();
	m_player = m_world->GetPlayer();
	m_numTilesInViewVertically = GAME_CAMERA_Y;
	m_numTilesInViewHorizontally = GAME_CAMERA_Y * CLIENT_ASPECT;
	m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(m_numTilesInViewHorizontally, m_numTilesInViewVertically));
	m_UICamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(UI_CAMERA_X, UI_CAMERA_Y));

	AddDevConsoleTest();
	AddAlignedTextTest();
	AddImageTest();
	AddBlackboardTest();



}

void Game::Shutdown(){}

void Game::RunFrame(){}

void Game::Update( float deltaSeconds )
{
	m_world->Update(deltaSeconds);


	UpdateCamera( deltaSeconds );
	UpdateConsoleTest( deltaSeconds );
	UpdateAlignedTextTest( deltaSeconds );
	UpdateImageTest(deltaSeconds);
	UpdateBlackboardTest(deltaSeconds);
	CheckButtonPresses( deltaSeconds );


}

void Game::Render()
{

	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	g_theRenderer->BeginCamera( m_camera );
	RenderGame();
	g_theRenderer->EndCamera( m_camera );


	g_theRenderer->BeginCamera( m_UICamera );
	RenderUI();
	g_theRenderer->EndCamera( m_UICamera );

}


void Game::LoadAssets()
{
	g_theRenderer->CreateOrGetBitmapFont("Fonts/SquirrelFixedFont.png");

	g_theRenderer->CreateOrGetTextureFromFile(IMAGETESTPATH);

	Texture* tileSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	Texture* actorSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/KushnariovaCharacters_12x53.png" );

	g_tileSpriteSheet = new SpriteSheet(*tileSpriteSheetTexture,IntVec2(8,8));
	g_actorSpriteSheet = new SpriteSheet(*actorSpriteSheetTexture, IntVec2(12,53));
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	IntVec2 currentMapBounds = GetCurrentMapBounds();

	if( g_theApp->GetDebugCameraMode() )
	{
		if( currentMapBounds.x > currentMapBounds.y * CLIENT_ASPECT )
		{
			m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2((float)currentMapBounds.x, (float)currentMapBounds.x/CLIENT_ASPECT ) );
		}
		else
		{
			m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2((float)currentMapBounds.y * CLIENT_ASPECT, (float)currentMapBounds.y ) );
		}
	}
	else
	{
		//Using the map order, clamp to an imaginary smaller border including half the game camera size

		m_cameraPosition = m_player->GetPosition();
		m_cameraPosition.x = Clampf( m_cameraPosition.x, m_numTilesInViewHorizontally/2.f, currentMapBounds.x - m_numTilesInViewHorizontally/2.f );
		m_cameraPosition.y = Clampf( m_cameraPosition.y, m_numTilesInViewVertically/2.f, currentMapBounds.y - m_numTilesInViewVertically/2.f );


		m_camera.SetOrthoView( Vec2( m_cameraPosition.x - m_numTilesInViewHorizontally/2.f, m_cameraPosition.y - m_numTilesInViewVertically/2.f ), Vec2( m_cameraPosition.x + m_numTilesInViewHorizontally/2.f, m_cameraPosition.y + m_numTilesInViewVertically/2.f ) );
	}

}

void Game::RenderGame()
{
	m_world->Render();


}

void Game::RenderUI()
{
	RenderConsoleTest();
	RenderAlignedTextTest();
	RenderImageTest();
	RenderBlackboardTest();
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	UNUSED( deltaSeconds );

	const XboxController& controller = g_theInput->GetXboxController(0);
	
	if( controller.GetButtonState( XBOX_BUTTON_ID_Y ).WasJustPressed() )
	{
		g_theConsole->SetIsOpen(!g_theConsole->IsOpen());
	}



	const KeyButtonState& tildeKey = g_theInput->GetKeyStates(0xC0);	//tilde: ~
	if( tildeKey.WasJustPressed() )
	{
		g_theConsole->SetIsOpen(!g_theConsole->IsOpen());
	}



}

IntVec2 Game::GetCurrentMapBounds() const
{
	return m_world->getCurrentMapBounds();
}



void Game::RenderConsoleTest() const
{
	g_theConsole->Render(*g_theRenderer,m_UICamera,TESTTEXTLINEHEIGHT);}

void Game::AddAlignedTextTest()
{
	m_isAlignedTextTestActive = true;

	Vec2 topRight(155.f,40.f);
	Vec2 bottomLeft(90.f,20.f);
	m_alignedTextTestBox = AABB2(bottomLeft, topRight);


}

void Game::AddImageTest()
{
	m_isImageTestActive = true;

	Vec2 topRightText( 155.f, 55.f );
	Vec2 bottomLeftText( 90.f, 45.f );
	m_imageTextTestBox = AABB2( bottomLeftText, topRightText );

	Vec2 topRightImageBox(120.f, 85.f);
	Vec2 bottomLeftImageBox(90.f, 55.f);


	m_imageTestBox = AABB2(bottomLeftImageBox, topRightImageBox);

	g_theRenderer->CreateOrGetTextureFromFile(IMAGETESTPATH);


	//m_imageTest = Image(IMAGETESTPATH);
}

void Game::AddBlackboardTest()
{
	m_isBlackboardTestActive = true;


	Vec2 topRight( 150.f, 12.f );
	Vec2 bottomLeft( 30.f, 10.f );
	m_blackBoardTextBox = AABB2( bottomLeft, topRight );


// 	startLevel="WizardTower3"
// 		windowAspect="1.777"
// 		isFullscreen="false"
	std::string startLevel = g_gameConfigBlackboard->GetValue("startLevel", "Default StartValue");
	float windowAspect = g_gameConfigBlackboard->GetValue("windowAspect", 0.f);
	bool isFullscreen = g_gameConfigBlackboard->GetValue("isFullscreen", true);
	g_gameConfigBlackboard->SetValue("NewSetValue","Are you still there?");
	std::string newSetValue = g_gameConfigBlackboard->GetValue("NewSetValue", "Set String fail");

	m_blackBoardText = Stringf("Start level: %s, WindowAspect: %f, isFullScreen: %d, newSetValue: %s", startLevel.c_str(), windowAspect, isFullscreen, newSetValue.c_str());



}

void Game::UpdateConsoleTest( float deltaSeconds )
{
	static float consoleTimer = 0.f;

	consoleTimer += deltaSeconds;

	if( consoleTimer > 0.2f )
	{
		g_theConsole->PringString( Rgba8::GREEN, "Are you still there?" );
		consoleTimer = 0.f;
	}



}

void Game::UpdateAlignedTextTest( float deltaSeconds )
{
	if( !m_isAlignedTextTestActive )
	{
		return;
	}
	static float alignment = 0.f;

	alignment += deltaSeconds;

	if( alignment < 1.f )
	{
		m_alignment.x = alignment;
		m_alignment.y = 0.f;
	}
	else if( alignment < 2.f )
	{
		m_alignment.x = 1.f;
		m_alignment.y = alignment - 1.f;
	}
	else if( alignment < 3.f )
	{
		m_alignment.x = 3.f - alignment;
		m_alignment.y = 1.f;
	}
	else if( alignment < 4.f )
	{
		m_alignment.x = 0.f;
		m_alignment.y = 4.f - alignment;
	}
	else if( alignment < 5.f )
	{
		m_alignment.x = alignment - 4.f;
		m_alignment.y = alignment - 4.f;
	}
	else
	{
		alignment = 2.f;
	}


	m_alignedTestText = Stringf("Alignment: (%f,%f)",m_alignment.x, m_alignment.y);


}

void Game::UpdateImageTest( float deltaSeconds )
{
	if( !m_isImageTestActive )
	{
		return;
	}

	static float imageTestTimer = 0.f;
	imageTestTimer += deltaSeconds;

	int xDimension = m_imageTest.GetDimensions().x - 1;
	int xIndex = ClampInt((int)imageTestTimer,0,xDimension);

	m_pixelColor = m_imageTest.GetTexelColor( IntVec2( xIndex, 0 ) );

	m_pixelToString = Stringf( "Pixel number: %i = R: %uc, G: %uc, B: %uc, A: %uc",xIndex, m_pixelColor.r, m_pixelColor.g, m_pixelColor.b, m_pixelColor.a );

	if( xIndex >= xDimension )
	{
		imageTestTimer = 0.f;
	}
}

void Game::UpdateBlackboardTest( float deltaSeconds )
{
	if( !m_isBlackboardTestActive )
	{
		return;
	}

	static float blackBoardTimer = 0.f;
	blackBoardTimer += deltaSeconds;

	if( blackBoardTimer > 1.f )
	{
		g_gameConfigBlackboard->SetValue( "stop", "Please stop" );
		std::string stopString = g_gameConfigBlackboard->GetValue( "stops", "But I won't stop" );
		m_blackBoardText = m_blackBoardText + " Stop?: " + stopString;
		blackBoardTimer = 0.f;
	}
}

void Game::RenderAlignedTextTest() const
{
	if( !m_isAlignedTextTestActive )
	{
		return;
	}


	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawAABB2Filled(m_alignedTextTestBox,Rgba8(0,0,0,128));

	g_theRenderer->DrawAlignedTextAtPosition(m_alignedTestText.c_str(), m_alignedTextTestBox, TESTTEXTLINEHEIGHT, m_alignment);
}

void Game::RenderImageTest() const
{
	if( !m_isImageTestActive )
	{
		return;
	}
	g_theRenderer->DrawAlignedTextAtPosition(m_pixelToString.c_str(), m_imageTextTestBox, TESTTEXTLINEHEIGHT, Vec2(0.f,0.f));

	Texture* testImage = g_theRenderer->CreateOrGetTextureFromFile(IMAGETESTPATH);
	g_theRenderer->BindTexture(testImage);
	g_theRenderer->DrawAABB2Filled(m_imageTestBox, Rgba8::WHITE);
}

void Game::RenderBlackboardTest() const
{
	if( !m_isBlackboardTestActive )
	{
		return;
	}


	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawAABB2Filled( m_blackBoardTextBox, Rgba8( 0, 0, 0, 128 ) );

	g_theRenderer->DrawAlignedTextAtPosition("Blackboard test", m_blackBoardTextBox, TESTTEXTLINEHEIGHT, Vec2(0.f, 1.f));
	g_theRenderer->DrawAlignedTextAtPosition(m_blackBoardText.c_str(), m_blackBoardTextBox, TESTTEXTLINEHEIGHT, Vec2(0.f, 0.f));
}

void Game::InitializeDefinitions()
{
	m_mapDefDoc		= new XMLDocument;
	m_tileDefDoc	= new XMLDocument;
	m_actorDefDoc	= new XMLDocument;

	const XMLElement& mapDef = GetRootElement(*m_mapDefDoc, "Data/Gameplay/MapDefs.xml");
	const XMLElement& tileDef = GetRootElement(*m_tileDefDoc, "Data/Gameplay/TileDefs.xml");
	const XMLElement& actorDef = GetRootElement(*m_actorDefDoc, "Data/Gameplay/Actors.xml");

	MapDefinition::InitializeMapDefinitions(mapDef);
	TileDefinition::InitializeTileDefinitions(tileDef);
	ActorDefinition::InitializeActorDefinitions(actorDef);
}

void Game::AddDevConsoleTest()
{
	m_isDevConsoleTestActive = true;
	g_theConsole->PringString( Rgba8::RED, "Hello World!" );
	g_theConsole->PringString( Rgba8::GREEN, "Are you still there?" );
	g_theConsole->PringString( Rgba8::BLUE, "Why?" );

	g_theConsole->PringString( Rgba8::RED, "Hello World!" );
	g_theConsole->PringString( Rgba8::GREEN, "Are you still there?" );
	g_theConsole->PringString( Rgba8::BLUE, "Why?" );

	g_theConsole->PringString( Rgba8::RED, "Hello World!" );
	g_theConsole->PringString( Rgba8::GREEN, "Are you still there?" );
	g_theConsole->PringString( Rgba8::BLUE, "Why?" );

	g_theConsole->PringString( Rgba8::RED, "Hello World!" );
	g_theConsole->PringString( Rgba8::GREEN, "Are you still there?" );
	g_theConsole->PringString( Rgba8::BLUE, "Why?" );
}
