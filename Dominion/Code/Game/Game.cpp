#pragma once
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/MonteCarlo.hpp"
#include "Game/MonteCarloNoTree.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/World.hpp"

#include "Engine/UI/UIManager.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/UI/WidgetGrid.hpp"
#include "Engine/UI/WidgetIncrementer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Delegate.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Vec4.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;


int constexpr maxDepth = 15;
int constexpr g_WhoStarts = PLAYER_2;

Game::Game()
{
	//m_world = new World(this);
}

Game::~Game(){}

void Game::Startup()
{
	EnableDebugRendering();
	m_camera = Camera();
	m_camera.SetColorTarget(nullptr); // we use this
	m_camera.CreateMatchingDepthStencilTarget( g_theRenderer );
	m_camera.SetOutputSize( Vec2( 16.f, 9.f ) );
	m_camera.SetProjectionPerspective( 60.f, -0.09f, -100.f );
	m_camera.Translate( Vec3( -2.f, 0.f, 1.5f ) );

// 	Sampler* testSampler = new Sampler( g_theRenderer, SAMPLER_BILINEAR );
// 	g_theRenderer->BindSampler( testSampler );

	m_gameClock = new Clock();
	m_gameClock->SetParent( Clock::GetMaster() );

	g_theRenderer->Setup( m_gameClock );

	m_mc = new MonteCarloNoTree();
	m_mcts = new MonteCarlo();

	//if reload
	//m_mcts->RestoreFromData();

	m_mcts->SetSimMethod( m_mctsSimMethod );
	m_mcts->SetExplorationParameter( m_mctsExplorationParameter );
	m_mcts->SetEpsilonValueZeroToOne( m_mctsEpsilon );
	m_mcts->SetExpansionStrategy( m_mctsExpansionStrategy );
	m_mcts->SetRolloutMethod( m_mctsRolloutMethod );

	CardDefinition::InitializeCards();
	InitializeGameState();

	StartupUI();

	m_timer.SetSeconds( 2 );
}

void Game::Shutdown()
{
	if( g_theUIManager )
	{
		delete g_theUIManager;
		g_theUIManager = nullptr;
	}

	if( m_baseCardWidget )
	{
		delete m_baseCardWidget;
		m_baseCardWidget = nullptr;
	}

	m_mcts->Shutdown();
	delete m_mcts;
	delete m_mc;
}

void Game::RunFrame(){}

void Game::Update()
{
	float dt = (float)m_gameClock->GetLastDeltaSeconds();

	m_currentTime += dt;
	if( m_currentTime > 255.f )
	{
		m_currentTime = 0.f;
	}
	Rgba8 clearColor;

	clearColor.g = 0;
	clearColor.r = 0;
	clearColor.b = 0;

	m_camera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT, clearColor, 0.f, 0 );

	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses( dt );
	}

	DebugDrawGame();
	

	if( m_isAutoPlayEnabled )
	{
		AutoPlayGame();
	}

	UpdateUI();
	g_theUIManager->Update( dt );

}

void Game::Render()
{
	Texture* backbuffer = g_theRenderer->GetBackBuffer();
	Texture* colorTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	
	m_camera.SetColorTarget( 0, colorTarget );

	g_theRenderer->BeginCamera(m_camera);

	g_theRenderer->DisableLight( 0 );

// 	Texture* testTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
// 	g_theRenderer->BindTexture( testTexture );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->EndCamera(m_camera);

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( &m_camera );


	
	g_theRenderer->CopyTexture( backbuffer, colorTarget );

	m_camera.SetColorTarget( nullptr );
	g_theRenderer->ReleaseRenderTarget( colorTarget );


	GUARANTEE_OR_DIE( g_theRenderer->GetTotalRenderTargetPoolSize() < 8, "Created too many render targets" );

	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
	DebugRenderEndFrame();

	g_theUIManager->Render();
}

void Game::StartupUI()
{
	g_theUIManager = new UIManager( Vec2( 16.f, 9.f ), g_theRenderer );
	g_theUIManager->Startup();

	Widget* rootWidget = g_theUIManager->GetRootWidget();


	Texture* cardBackTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Card_back.jpg" );

	m_cyanTexture				= g_theRenderer->CreateTextureFromColor( Rgba8::CYAN );
	m_redTexture				= g_theRenderer->CreateTextureFromColor( Rgba8::RED );
	m_greenTexture				= g_theRenderer->CreateTextureFromColor( Rgba8::GREEN );
	m_darkRedTexture			= g_theRenderer->CreateTextureFromColor( Rgba8::RedBrown );
	m_artichokeGreenTexture		= g_theRenderer->CreateTextureFromColor( Rgba8::ArtichokeGreen );
	m_forestGreenTexture		= g_theRenderer->CreateTextureFromColor( Rgba8::ForestGreen );
	m_darkDarkGreenTexture		= g_theRenderer->CreateTextureFromColor( Rgba8::DarkDarkGreen );
	m_greyGreenTexture			= g_theRenderer->CreateTextureFromColor( Rgba8::GreyGreen );
	m_darkForestGreenTexture	= g_theRenderer->CreateTextureFromColor( Rgba8::DarkForestGreen );

	rootWidget->SetTexture( m_darkDarkGreenTexture, m_darkDarkGreenTexture, m_darkDarkGreenTexture );
	rootWidget->SetIsVisible( true );
	AABB2 screenBounds = g_theUIManager->GetScreenBounds();

	Vec3 togglePlayerScale = Vec3( 1.7f, 0.5f, 1.f );
	Transform togglePlayerTransform;
	togglePlayerTransform.m_scale = togglePlayerScale;
	togglePlayerTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.14f, 0.13f ) );
	m_toggleCurrentViewedPlayer = new Widget( togglePlayerTransform );
	m_toggleCurrentViewedPlayer->SetTexture( m_greenTexture, m_cyanTexture, m_redTexture );
	m_toggleCurrentViewedPlayer->SetCanDrag( false );
	m_toggleCurrentViewedPlayer->SetCanHover( true );
	m_toggleCurrentViewedPlayer->SetCanSelect( true );
	m_toggleCurrentViewedPlayer->SetText( "Toggle Player" );
	m_toggleCurrentViewedPlayer->SetTextSize( 0.07f );
	
	Delegate<EventArgs const&>& toggleReleaseDel = m_toggleCurrentViewedPlayer->m_releaseDelegate;
	toggleReleaseDel.SubscribeMethod( this, &Game::ToggleWhoseViewedOnUI );
	rootWidget->AddChild( m_toggleCurrentViewedPlayer );

	//Base card
	Vec3 baseCardScale = Vec3( 1.5f, 2.25f, 1.f );
	Transform baseTransform = Transform();
	baseTransform.m_scale = baseCardScale;
	m_baseCardWidget = new Widget( baseTransform );
	m_baseCardWidget->SetCanDrag( true );
	

	Vec3 deckScale = Vec3( 1.f, 1.5f, 1.f );
	Transform deckTransform = Transform();
	deckTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.05f, 0.1f ) );
	deckTransform.m_scale = deckScale;
	Widget* deckWidget = new Widget( deckTransform );
	deckWidget->SetTexture( cardBackTexture, cardBackTexture, cardBackTexture );
	deckWidget->SetCanHover( true );
	deckWidget->SetText( "Hello" );
	deckWidget->SetTextSize( 0.1f );
	rootWidget->AddChild( deckWidget );
	m_player1DeckWidget = deckWidget;

	Widget* discardWidget = new Widget( *deckWidget );
	discardWidget->SetPosition( screenBounds.GetPointAtUV( Vec2( 0.95f, 0.1f ) ) );
	rootWidget->AddChild( discardWidget );
	m_player1DiscardWidget = discardWidget;

	Transform endTurnTransform = Transform();
	endTurnTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.95f, 0.25f ) );
	endTurnTransform.m_scale = Vec3( 1.5f, 0.75f, 1.f );
	m_playerNextPhaseWidget = new Widget( endTurnTransform );
	m_playerNextPhaseWidget->SetCanSelect( true );
	m_playerNextPhaseWidget->SetEventToFire( "endTurn" );
	m_playerNextPhaseWidget->SetText( "End Phase" );
	m_playerNextPhaseWidget->SetTextSize( 0.1f );
	m_playerNextPhaseWidget->SetTexture( m_greenTexture, m_cyanTexture, m_redTexture );
	Delegate<EventArgs const&>& nextPhaseReleaseDelegate = m_playerNextPhaseWidget->m_releaseDelegate;
	nextPhaseReleaseDelegate.SubscribeMethod( this, &Game::PlayMoveIfValid );
	EventArgs& nextPhaseReleaseArgs = m_playerNextPhaseWidget->m_releaseArgs;
	inputMove_t endMove;
	endMove.m_moveType = END_PHASE;
	endMove.m_whoseMoveIsIt = m_whoseUIPlaying;
	nextPhaseReleaseArgs = endMove.ToEventArgs();
	rootWidget->AddChild( m_playerNextPhaseWidget );


	Transform cardPilesTransform = Transform();
	cardPilesTransform.m_position= screenBounds.GetPointAtUV( Vec2( 0.5f, 0.6f ) );
	cardPilesTransform.m_scale = Vec3( 12.f, 4.5f, 1.f );
	
	m_cardPilesWidget = new WidgetGrid( cardPilesTransform, m_cardPileDimensions, nullptr );
	rootWidget->AddChild( m_cardPilesWidget );

	Transform playAreaTransform = Transform();
	playAreaTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.07f, 0.65f ) );
	playAreaTransform.m_scale = Vec3( 1.5f, 5.f, 1.f );
	m_player1PlayAreaWidget = new WidgetGrid( playAreaTransform, m_playAreaGridDimensions, nullptr );
	m_player1PlayAreaWidget->SetCanHover( false );
	m_player1PlayAreaWidget->SetCanSelect( false );
	m_player1PlayAreaWidget->SetText( "Play Area" );
	m_player1PlayAreaWidget->SetTextSize( 0.1f );
	m_player1PlayAreaWidget->SetTexture( m_artichokeGreenTexture, nullptr, nullptr );
	rootWidget->AddChild( m_player1PlayAreaWidget );

	//Gamestate
	Transform gameStateTransform = Transform();
	gameStateTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.93f, 0.6f ) );
	gameStateTransform.m_scale = Vec3( 1.5f, 4.f, 1.f );
	m_gameStateWidget = new WidgetGrid( gameStateTransform, m_gameStateGridDimensions, nullptr );
	m_gameStateWidget->SetText( "Game state" );
	m_gameStateWidget->SetTextSize( 0.1f );
	m_gameStateWidget->SetTexture( m_artichokeGreenTexture, nullptr, nullptr );
	rootWidget->AddChild( m_gameStateWidget );

	Transform gameStateChildTransform;
	gameStateChildTransform.m_scale = Vec3( 1.5f, 1.f, 1.f );
	m_currentControlledPlayer = new Widget( gameStateChildTransform );
	m_currentMoney = new Widget( gameStateChildTransform );
	m_currentBuys = new Widget( gameStateChildTransform );
	m_currentActions = new Widget( gameStateChildTransform );

	m_currentControlledPlayer->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );
	m_currentMoney->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );
	m_currentBuys->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );
	m_currentActions->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );

	m_currentControlledPlayer->SetText( Stringf( "Player: %i", m_whoseUIPlaying + 1 ) );
	m_currentControlledPlayer->SetTextSize( 0.1f );	
	m_currentMoney->SetText( Stringf( "Money: %i", 5 ) );
	m_currentMoney->SetTextSize( 0.1f );
	m_currentBuys->SetText( Stringf( "Buys: %i", 1 ) );
	m_currentBuys->SetTextSize( 0.1f );
	m_currentActions->SetText( Stringf( "Actions: %i", 1 ) );
	m_currentActions->SetTextSize( 0.1f );

	m_gameStateWidget->AddChild( m_currentControlledPlayer );
	m_gameStateWidget->AddChild( m_currentMoney );
	m_gameStateWidget->AddChild( m_currentBuys );
	m_gameStateWidget->AddChild( m_currentActions );


	Transform AITransform = Transform();
	AITransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.5f, 0.93f ) );
	AITransform.m_scale = Vec3( 12.f, 1.f, 1.f );
	m_AIWidget = new WidgetGrid( AITransform, m_AIGridDimensions );
	m_AIWidget->SetText( "AI Info" );
	m_AIWidget->SetTextSize( 0.1f );
	m_AIWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );
	rootWidget->AddChild( m_AIWidget );

	Transform moreAIInfoTransform = Transform();
	moreAIInfoTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.5f, 0.88f ) );
	moreAIInfoTransform.m_scale = Vec3( 0.75f, 0.5f, 1.f );
	m_showAIInfoButtonWidget = new Widget( moreAIInfoTransform );
	m_showAIInfoButtonWidget->SetText( "Edit AI" );
	m_showAIInfoButtonWidget->SetTextSize( 0.1f );
	m_showAIInfoButtonWidget->SetTexture( m_redTexture, m_darkRedTexture, m_redTexture );
	m_showAIInfoButtonWidget->SetCanHover( true );
	m_showAIInfoButtonWidget->SetCanSelect( true );
	Delegate<EventArgs const&>& showAIDelegate = m_showAIInfoButtonWidget->m_releaseDelegate;
	showAIDelegate.SubscribeMethod( this, &Game::ToggleAIScreen );
	rootWidget->AddChild( m_showAIInfoButtonWidget );

	Transform playerScoreTransform = Transform();
	playerScoreTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.075f, 0.23f ) );
	playerScoreTransform.m_scale = Vec3( 2.f, 0.8f, 1.f );
	m_playerScoreWidget = new WidgetGrid( playerScoreTransform, m_playerScoreGridDimensions );
	m_playerScoreWidget->SetTexture( m_forestGreenTexture, m_forestGreenTexture, m_forestGreenTexture );
	m_playerScoreWidget->SetText( "Score Area" );
	m_playerScoreWidget->SetTextSize( 0.1f );
	rootWidget->AddChild( m_playerScoreWidget );

	//Hand widget
	Vec3 handScale = Vec3( 10.f, 3.f, 1.f );
	Transform handTransform = Transform();
	handTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.5f, 0.15f ) );
	handTransform.m_scale = handScale;
	m_player1HandWidget = new Widget( handTransform );
	//m_handWidget->SetTexture( handTexture, nullptr, nullptr );
	m_player1HandWidget->SetIsVisible( true );
	m_player1HandWidget->SetTexture( m_greyGreenTexture, nullptr, nullptr );
	m_player1HandWidget->SetCanHover( false );
	//m_player1HandWidget->SetCanHover( false );
	rootWidget->AddChild( m_player1HandWidget );

	InitializeCardPilesWidgets();
	InitializeAISmallPanelWidget();
	InitializeAILargePanelWidget();
	MatchUIToGameState();
}

void Game::InitializeAISmallPanelWidget()
{
	Transform aiInfoTransform;
	aiInfoTransform.m_scale = Vec3( 2.f, 1.f, 1.f );
	m_AIInfoWidget = new Widget( aiInfoTransform );
	m_AIInfoWidget->SetText( "AI Controls: " );
	m_AIInfoWidget->SetTextSize( 0.1f );
	m_AIInfoWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );

	m_playAIMoveWidget = new Widget( aiInfoTransform );
	m_playAIMoveWidget->SetText( "Play Move" );
	m_playAIMoveWidget->SetTextSize( 0.1f );
	m_playAIMoveWidget->SetTexture( m_forestGreenTexture, m_darkForestGreenTexture, m_forestGreenTexture );
	Delegate<EventArgs const&>& playAIMoveDelegate = m_playAIMoveWidget->m_releaseDelegate;
	playAIMoveDelegate.SubscribeMethod( this, &Game::PlayCurrentAIMove );

	m_ToggleAutoPlayWidget = new Widget( aiInfoTransform );
	m_ToggleAutoPlayWidget->SetText( "Auto Play: OFF" );
	m_ToggleAutoPlayWidget->SetTextSize( 0.1f );
	m_ToggleAutoPlayWidget->SetTexture( m_forestGreenTexture, m_darkForestGreenTexture, m_forestGreenTexture );
	Delegate<EventArgs const&>& toggleAutoPlayDelegate = m_ToggleAutoPlayWidget->m_releaseDelegate;
	toggleAutoPlayDelegate.SubscribeMethod( this, &Game::ToggleAutoPlay );

	m_currentAIBestMoveWidget = new Widget( aiInfoTransform );
	m_currentAIBestMoveWidget->SetText( "Best move: Play x card" );
	m_currentAIBestMoveWidget->SetTextSize( 0.1f );
	m_currentAIBestMoveWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );

	m_currentAIWidget = new Widget( aiInfoTransform );
	std::string aiStrategy = "MCTS";
	m_currentAIWidget->SetText( Stringf("Current AI: %s", "MCTS") );
	m_currentAIWidget->SetTextSize( 0.1f );
	m_currentAIWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );

	m_AIWidget->AddChild( m_AIInfoWidget );
	m_AIWidget->AddChild( m_playAIMoveWidget );
	m_AIWidget->AddChild( m_ToggleAutoPlayWidget );
	m_AIWidget->AddChild( m_currentAIWidget );
	m_AIWidget->AddChild( m_currentAIBestMoveWidget );
}

void Game::InitializeAILargePanelWidget()
{
	Rgba8 darkdarkGreen = Rgba8::DarkDarkGreen;
	Rgba8 greyGreen = Rgba8::GreyGreen;
	Rgba8 black = Rgba8::BLACK;
	Texture const* blackTexture = g_theRenderer->CreateTextureFromColor( black );
	Texture const* darkdarkGreenTexture = g_theRenderer->CreateTextureFromColor( darkdarkGreen );
	Widget* rootWidget = g_theUIManager->GetRootWidget();

	Transform AIMoreInfoTransform;
	AIMoreInfoTransform.m_scale = Vec3( 8.f, 5.f, 1.f );
	m_AIMoreInfoWidget = new Widget( AIMoreInfoTransform );
	rootWidget->AddChild( m_AIMoreInfoWidget );
	m_AIMoreInfoWidget->SetTexture( darkdarkGreenTexture, darkdarkGreenTexture, darkdarkGreenTexture );
	m_AIMoreInfoWidget->SetIsEnabled( false );


	Transform chooseAITransform;
	chooseAITransform.m_scale = Vec3( 8.f/3.f, 5.f/4.f, 1.f );
	std::vector<std::string> aiStrategies;
	aiStrategies.push_back( "Random" );
	aiStrategies.push_back( "Big Money" );
	aiStrategies.push_back( "Single Witch" );
	aiStrategies.push_back( "Double Witch" );
	aiStrategies.push_back( "Sarasua1" );

	AABB2 AIMoreInfoBounds = m_AIMoreInfoWidget->GetLocalAABB2();
	AABB2 leftColumnAIBounds = AIMoreInfoBounds.CarveBoxOffLeft( 0.2f );
	leftColumnAIBounds = leftColumnAIBounds.GetBoxAtRight( 0.9f );
	AABB2 topLeftAIBounds = leftColumnAIBounds.CarveBoxOffTop( 0.3f );
	std::vector<AABB2> leftAIRows = topLeftAIBounds.GetBoxAsRows( 4 );
	m_player1AITextWidget = new Widget( leftAIRows[3], m_AIMoreInfoWidget );
	m_player1ChooseAIWidget = new WidgetIncrementer( aiStrategies, leftAIRows[2], 0.3f, 0.1f, m_AIMoreInfoWidget );
	m_player2AITextWidget = new Widget( leftAIRows[1], m_AIMoreInfoWidget );
	m_player2ChooseAIWidget = new WidgetIncrementer( aiStrategies, leftAIRows[0], 0.3f, 0.1f, m_AIMoreInfoWidget );

	m_player1AITextWidget->SetIsVisible( true );
	m_player2AITextWidget->SetIsVisible( true );
	m_player2AITextWidget->SetText( "Player 2" );
	m_player1AITextWidget->SetText( "Player 1" );
	m_player2AITextWidget->SetTextAlignment( Vec2( 0.1f, 0.5f ) );
	m_player1AITextWidget->SetTextAlignment( Vec2( 0.1f, 0.5f ) );
	m_player2AITextWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player2AITextWidget->SetTextSize( 0.1f );
	m_player1AITextWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player1AITextWidget->SetTextSize( 0.1f );
	m_player2ChooseAIWidget->SetTextures( blackTexture );
	m_player1ChooseAIWidget->SetTextures( blackTexture );

	//Player 1 MCTS AI Parameters
	AABB2 middleColumnAIBounds = AIMoreInfoBounds.CarveBoxOffLeft( 0.5f );
	middleColumnAIBounds = middleColumnAIBounds.GetBoxAtTop( 0.5f );
	std::vector<AABB2> middleAIRows = middleColumnAIBounds.GetBoxAsRows( 9 );
	m_player1MCTSAIParametersWidget = new Widget( middleAIRows[8], m_AIMoreInfoWidget );
	m_player1SelectionTextWidget = new Widget( middleAIRows[7], m_AIMoreInfoWidget );
	m_player1UCTScoreChangerWidget = new Widget( middleAIRows[6], m_AIMoreInfoWidget );
	m_player1ExpansionTextWidget = new Widget( middleAIRows[5], m_AIMoreInfoWidget );
	m_player1ExpansionChangerWidget = new Widget( middleAIRows[4], m_AIMoreInfoWidget );
	m_player1SimulationTextWidget = new Widget( middleAIRows[3], m_AIMoreInfoWidget );
	m_player1SimulationChangerWidget = new Widget( middleAIRows[2], m_AIMoreInfoWidget );
	m_player1UseChaosChanceWidget = new Widget( middleAIRows[1], m_AIMoreInfoWidget );
	m_player1ChaosChanceChangerWidget = new Widget( middleAIRows[0], m_AIMoreInfoWidget );

	m_player1MCTSAIParametersWidget->SetText( "Player 1 MCTS AI Parameters" );
	m_player1MCTSAIParametersWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player1MCTSAIParametersWidget->SetTextSize( 0.1f );
	m_player1MCTSAIParametersWidget->SetIsVisible( true );
	m_player1MCTSAIParametersWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player1SelectionTextWidget->SetText( "Selection" );
	m_player1SelectionTextWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player1SelectionTextWidget->SetTextSize( 0.1f );
	m_player1SelectionTextWidget->SetIsVisible( true );
	m_player1SelectionTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player1UCTScoreChangerWidget->SetText( "<< >> UCT Score: 0.5" );
	m_player1UCTScoreChangerWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player1UCTScoreChangerWidget->SetTextSize( 0.1f );
	m_player1UCTScoreChangerWidget->SetIsVisible( true );
	m_player1UCTScoreChangerWidget->SetTextAlignment( Vec2( 0.5f, 0.5f ) );

	m_player1ExpansionTextWidget->SetText( "Expansion" );
	m_player1ExpansionTextWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player1ExpansionTextWidget->SetTextSize( 0.1f );
	m_player1ExpansionTextWidget->SetIsVisible( true );
	m_player1ExpansionTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player1ExpansionChangerWidget->SetText( "<< >> Heuristics" );
	m_player1ExpansionChangerWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player1ExpansionChangerWidget->SetTextSize( 0.1f );
	m_player1ExpansionChangerWidget->SetIsVisible( true );
	m_player1ExpansionChangerWidget->SetTextAlignment( Vec2( 0.5f, 0.5f ) );

	m_player1SimulationTextWidget->SetText( "Simulation" );
	m_player1SimulationTextWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player1SimulationTextWidget->SetTextSize( 0.1f );
	m_player1SimulationTextWidget->SetIsVisible( true );
	m_player1SimulationTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player1SimulationChangerWidget->SetText( "<< >> Greedy" );
	m_player1SimulationChangerWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player1SimulationChangerWidget->SetTextSize( 0.1f );
	m_player1SimulationChangerWidget->SetIsVisible( true );
	m_player1SimulationChangerWidget->SetTextAlignment( Vec2( 0.5f, 0.5f ) );

	m_player1UseChaosChanceWidget->SetText( "Use Chaos Chance: [x]" );
	m_player1UseChaosChanceWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player1UseChaosChanceWidget->SetTextSize( 0.1f );
	m_player1UseChaosChanceWidget->SetIsVisible( true );
	m_player1UseChaosChanceWidget->SetTextAlignment( Vec2( 0.5f, 0.5f ) );

	m_player1ChaosChanceChangerWidget->SetText( "<< >> 0.15" );
	m_player1ChaosChanceChangerWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player1ChaosChanceChangerWidget->SetTextSize( 0.1f );
	m_player1ChaosChanceChangerWidget->SetIsVisible( true );
	m_player1ChaosChanceChangerWidget->SetTextAlignment( ALIGN_CENTER_RIGHT );

	//Player 2 MCTS AI Parameters
	AABB2 rightColumnAIBounds = AIMoreInfoBounds;
	rightColumnAIBounds = rightColumnAIBounds.GetBoxAtTop( 0.5f );
	std::vector<AABB2> rightAIRows = rightColumnAIBounds.GetBoxAsRows( 9 );
	m_player2MCTSAIParametersWidget = new Widget( rightAIRows[8], m_AIMoreInfoWidget );
	m_player2SelectionTextWidget = new Widget( rightAIRows[7], m_AIMoreInfoWidget );
	m_player2UCTScoreChangerWidget = new Widget( rightAIRows[6], m_AIMoreInfoWidget );
	m_player2ExpansionTextWidget = new Widget( rightAIRows[5], m_AIMoreInfoWidget );
	m_player2ExpansionChangerWidget = new Widget( rightAIRows[4], m_AIMoreInfoWidget );
	m_player2SimulationTextWidget = new Widget( rightAIRows[3], m_AIMoreInfoWidget );
	m_player2SimulationChangerWidget = new Widget( rightAIRows[2], m_AIMoreInfoWidget );
	m_player2UseChaosChanceWidget = new Widget( rightAIRows[1], m_AIMoreInfoWidget );
	m_player2ChaosChanceChangerWidget = new Widget( rightAIRows[0], m_AIMoreInfoWidget );

	m_player2MCTSAIParametersWidget->SetText( "Player 2 MCTS AI Parameters" );
	m_player2MCTSAIParametersWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player2MCTSAIParametersWidget->SetTextSize( 0.1f );
	m_player2MCTSAIParametersWidget->SetIsVisible( true );
	m_player2MCTSAIParametersWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player2SelectionTextWidget->SetText( "Selection" );
	m_player2SelectionTextWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player2SelectionTextWidget->SetTextSize( 0.1f );
	m_player2SelectionTextWidget->SetIsVisible( true );
	m_player2SelectionTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player2UCTScoreChangerWidget->SetText( "<< >> UCT Score: 0.5" );
	m_player2UCTScoreChangerWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player2UCTScoreChangerWidget->SetTextSize( 0.1f );
	m_player2UCTScoreChangerWidget->SetIsVisible( true );
	m_player2UCTScoreChangerWidget->SetTextAlignment( Vec2( 0.5f, 0.5f ) );

	m_player2ExpansionTextWidget->SetText( "Expansion" );
	m_player2ExpansionTextWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player2ExpansionTextWidget->SetTextSize( 0.1f );
	m_player2ExpansionTextWidget->SetIsVisible( true );
	m_player2ExpansionTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player2ExpansionChangerWidget->SetText( "<< >> Heuristics" );
	m_player2ExpansionChangerWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player2ExpansionChangerWidget->SetTextSize( 0.1f );
	m_player2ExpansionChangerWidget->SetIsVisible( true );
	m_player2ExpansionChangerWidget->SetTextAlignment( Vec2( 0.5f, 0.5f ) );

	m_player2SimulationTextWidget->SetText( "Simulation" );
	m_player2SimulationTextWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player2SimulationTextWidget->SetTextSize( 0.1f );
	m_player2SimulationTextWidget->SetIsVisible( true );
	m_player2SimulationTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player2SimulationChangerWidget->SetText( "<< >> Greedy" );
	m_player2SimulationChangerWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player2SimulationChangerWidget->SetTextSize( 0.1f );
	m_player2SimulationChangerWidget->SetIsVisible( true );
	m_player2SimulationChangerWidget->SetTextAlignment( Vec2( 0.5f, 0.5f ) );

	m_player2UseChaosChanceWidget->SetText( "Use Chaos Chance: [x]" );
	m_player2UseChaosChanceWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player2UseChaosChanceWidget->SetTextSize( 0.1f );
	m_player2UseChaosChanceWidget->SetIsVisible( true );
	m_player2UseChaosChanceWidget->SetTextAlignment( Vec2( 0.5f, 0.5f ) );

	m_player2ChaosChanceChangerWidget->SetText( "<< >> 0.15" );
	m_player2ChaosChanceChangerWidget->SetTexture( blackTexture, blackTexture, blackTexture );
	m_player2ChaosChanceChangerWidget->SetTextSize( 0.1f );
	m_player2ChaosChanceChangerWidget->SetIsVisible( true );
	m_player2ChaosChanceChangerWidget->SetTextAlignment( ALIGN_CENTER_RIGHT );
}

void Game::InitializeCardPilesWidgets()
{
	for( int cardIndex = 0; cardIndex < (int)eCards::NUM_CARDS; cardIndex++ )
	{
		Widget* cardPileWidget = new Widget( *m_baseCardWidget );
		cardPileWidget->SetCanDrag( false );
		CardDefinition const* cardDef = CardDefinition::GetCardDefinitionByType( (eCards)cardIndex );
		Texture const* cardTexture = cardDef->GetCardTexture();
		int cardCount = m_currentGameState->m_cardPiles[cardIndex].m_pileSize;
		cardPileWidget->SetTexture( cardTexture, cardTexture, cardTexture );
		m_cardPilesWidget->AddChild( cardPileWidget );
		AddCountToCardWidget( cardPileWidget, cardCount);

		Delegate<EventArgs const&>& cardPileReleaseDelegate = cardPileWidget->m_releaseDelegate;
		cardPileReleaseDelegate.SubscribeMethod( this, &Game::PlayMoveIfValid );
		EventArgs& cardPileReleaseArgs = cardPileWidget->m_releaseArgs;

		inputMove_t cardToBuy;
		cardToBuy.m_cardIndex = cardIndex;
		cardToBuy.m_moveType = BUY_MOVE;
		cardToBuy.m_whoseMoveIsIt = m_playerWidgetsToShow;
		cardPileReleaseArgs = cardToBuy.ToEventArgs();
	}
}

void Game::MatchUIToGameState()
{
	PlayerBoard const& playerBoard = m_currentGameState->m_playerBoards[m_whoseUIPlaying];
	CardPile const& playerHand = playerBoard.GetHand();
	CardPile const& playerPlayArea = playerBoard.GetPlayArea();

	UpdateAISmallPanelWidget();

	m_player1HandWidget->ClearChildren();

	AABB2 handBounds = m_player1HandWidget->GetLocalAABB2();
	int playerHandCount = playerHand.TotalCount();
	std::vector<AABB2> cardSlots = handBounds.GetBoxAsColumns( playerHandCount );
	std::vector<eCards> playerHandVector = playerHand.ToVector();
	for( size_t handIndex = 0; handIndex < playerHandVector.size(); handIndex++ )
	{
		Vec2 slotCenter = cardSlots[handIndex].GetCenter();
		Widget* cardWidget = new Widget( *m_baseCardWidget );
		cardWidget->SetPosition( slotCenter );
		CardDefinition const* cardDef = CardDefinition::GetCardDefinitionByType( (eCards)playerHandVector[handIndex] );
		cardWidget->SetTexture( cardDef->GetCardTexture(), cardDef->GetCardTexture(), cardDef->GetCardTexture() );

		EventArgs& releaseArgs = cardWidget->m_releaseArgs;
		int cardIndex = cardDef->GetCardIndex();

		inputMove_t playCardInHand;
		playCardInHand.m_cardIndex = cardIndex;
		playCardInHand.m_moveType = eMoveType::PLAY_CARD;
		playCardInHand.m_whoseMoveIsIt = m_whoseUIPlaying;

		releaseArgs = playCardInHand.ToEventArgs();

// 		releaseArgs.SetValue( "cardIndex", cardIndex );
// 		releaseArgs.SetValue( "moveType", (int)eCardType::ACTION_TYPE );
// 		releaseArgs.SetValue( "whoseMove", m_currentGameState->m_whoseMoveIsIt );

		releaseArgs.SetValue( "cardWidget", (std::uintptr_t)cardWidget );
		Delegate<EventArgs const&>& releaseDelegate = cardWidget->m_releaseDelegate;
		releaseDelegate.SubscribeMethod( this, &Game::PlayMoveIfValid );

		m_player1HandWidget->AddChild( cardWidget );
	}

	m_player1PlayAreaWidget->ClearChildren();
	std::vector<eCards> playAreaCards = playerPlayArea.ToVector();
	
	for( eCards const& card : playAreaCards )
	{
		CardDefinition const* cardDef = CardDefinition::GetCardDefinitionByType( card );
		Widget* cardWidget = new Widget( *m_baseCardWidget );
		cardWidget->SetTexture( cardDef->GetCardTexture(), m_cyanTexture, m_redTexture );

		m_player1PlayAreaWidget->AddChild( cardWidget );
	}

	int cardPileMaxCount = eCards::NUM_CARDS;
	for( int cardIndex = 0; cardIndex < cardPileMaxCount; cardIndex++ )
	{
		Widget *const cardPileWidget = m_cardPilesWidget->GetChildWidgetAtIndex( cardIndex );
		int cardPileCount = m_currentGameState->m_cardPiles[cardIndex].m_pileSize;
		UpdateCardCountOnWidget( cardPileWidget, cardPileCount );
	}

	UpdateGameStateWidget();
}

void Game::UpdateUI()
{
	if( m_isUIDirty )
	{
		m_isUIDirty = false;
		MatchUIToGameState();
	}
	PlayerBoard const& playerBoard  = m_currentGameState->m_playerBoards[0];


	int deckSize = playerBoard.m_sortedDeck.TotalCount();
	m_player1DeckWidget->SetText( Stringf( "%i", deckSize ) );

	int discardPileSize = playerBoard.m_discardPile.TotalCount();
	m_player1DiscardWidget->SetText( Stringf( "%i", discardPileSize ) );
}

void Game::UpdateGameStateWidget()
{
	PlayerBoard const& currentPlayer = m_currentGameState->m_playerBoards[m_whoseUIPlaying];
	int money = currentPlayer.m_currentCoins;
	int buys = currentPlayer.m_numberOfBuysAvailable;
	int actions = currentPlayer.m_numberOfActionsAvailable;

	m_currentControlledPlayer->SetText( Stringf( "Player: %i", m_whoseUIPlaying + 1 ) );
	m_currentMoney->SetText( Stringf( "Money: %i", money ) );
	m_currentBuys->SetText( Stringf( "Buys: %i", buys ) );
	m_currentActions->SetText( Stringf( "Actions: %i", actions ) );
}

void Game::UpdateAISmallPanelWidget()
{
	//Update next phase button
	eGamePhase currentPhase = m_currentGameState->m_currentPhase;
	switch( currentPhase )
	{
	case BUY_PHASE:
		m_playerNextPhaseWidget->SetText( "End Turn" );
		break;
	case ACTION_PHASE:
		m_playerNextPhaseWidget->SetText( "To Buy Phase" );
		break;
	default:
		break;
	}

	EventArgs& nextPhaseReleaseArgs = m_playerNextPhaseWidget->m_releaseArgs;
	inputMove_t endMove;
	endMove.m_moveType = END_PHASE;
	endMove.m_whoseMoveIsIt = m_whoseUIPlaying;
	nextPhaseReleaseArgs = endMove.ToEventArgs();

	std::string toggleAutoPlayStr;
	if( m_isAutoPlayEnabled )
	{
		toggleAutoPlayStr = "ON";
	}
	else
	{
		toggleAutoPlayStr = "OFF";
	}

	m_ToggleAutoPlayWidget->SetText( Stringf( "Auto Play: %s", toggleAutoPlayStr.c_str() ) );


	int whoseMove = m_currentGameState->m_whoseMoveIsIt;
	std::string aiStrategyStr;
	inputMove_t bestMoveForCurrentAI;
	if( whoseMove == PLAYER_1 )
	{
		bestMoveForCurrentAI = GetBestMoveUsingAIStrategy( m_player1Strategy );
		
		switch( m_player1Strategy )
		{
		case AIStrategy::RANDOM: aiStrategyStr = "Random";
			break;
		case AIStrategy::BIGMONEY: aiStrategyStr = "Big Money";
			break;
		case AIStrategy::SINGLEWITCH: aiStrategyStr = "Single Witch";
			break;
		case AIStrategy::SARASUA1: aiStrategyStr = "Sarasua1";
			break;
		case AIStrategy::MCTS: aiStrategyStr = "MCTS";
			break;
		case AIStrategy::DOUBLEWITCH: aiStrategyStr = "Double Witch";
			break;
		default:
			break;
		}
	}
	else if( whoseMove == PLAYER_2 )
	{
		bestMoveForCurrentAI = GetBestMoveUsingAIStrategy( m_player2Strategy );

		switch( m_player2Strategy )
		{
		case AIStrategy::RANDOM: aiStrategyStr = "Random";
			break;
		case AIStrategy::BIGMONEY: aiStrategyStr = "Big Money";
			break;
		case AIStrategy::SINGLEWITCH: aiStrategyStr = "Single Witch";
			break;
		case AIStrategy::SARASUA1: aiStrategyStr = "Sarasua1";
			break;
		case AIStrategy::MCTS: aiStrategyStr = "MCTS";
			break;
		case AIStrategy::DOUBLEWITCH: aiStrategyStr = "Double Witch";
			break;
		default:
			break;
		}
	}
	else
	{
		aiStrategyStr = "Game Over";
	}
	m_currentAIWidget->SetText( Stringf( "Current AI: %s", aiStrategyStr.c_str() ) );


	//Update Best Move
	std::string moveTypeStr;
	std::string bestMoveStr;

	if( bestMoveForCurrentAI.m_moveType == BUY_MOVE )
	{
		moveTypeStr = "Buy";
		int cardIndexToBuy = bestMoveForCurrentAI.m_cardIndex;
		cardIndexToBuy = m_currentGameState->m_cardPiles[cardIndexToBuy].m_cardIndex;
		CardDefinition const* cardDefToBuy = CardDefinition::GetCardDefinitionByType( (eCards)cardIndexToBuy );

		std::string cardToBuyStr = cardDefToBuy->GetCardName();

		bestMoveStr = Stringf( "%s %s", moveTypeStr.c_str(), cardToBuyStr.c_str() );
	}
	else if( bestMoveForCurrentAI.m_moveType == PLAY_CARD )
	{
		moveTypeStr = "Play";
		int cardIndexToPlay = bestMoveForCurrentAI.m_cardIndex;
		CardDefinition const* cardDefToPlay = CardDefinition::GetCardDefinitionByType( (eCards)cardIndexToPlay );

		std::string cardToPlayStr = cardDefToPlay->GetCardName();

		bestMoveStr = Stringf( "%s %s", moveTypeStr.c_str(), cardToPlayStr.c_str() );
	}
	else if( bestMoveForCurrentAI.m_moveType == END_PHASE )
	{
		bestMoveStr = Stringf( "End Phase" );
	}
	else
	{
		bestMoveStr = Stringf( "Invalid Move, MCTS needs sims" );
	}

	
	m_currentAIBestMoveWidget->SetText( bestMoveStr );
}

void Game::InitializeGameState()
{
	if( m_currentGameState )
	{
		delete m_currentGameState;
		m_currentGameState = nullptr;
	}


// 	PlayerBoard starterDeck = PlayerBoard( &m_rand );
// 	std::vector<CardData_t> starterCards;
// 	starterCards.reserve( 10 );
// 	CardDefinition const* copper = CardDefinition::GetCardDefinitionByType( eCards::COPPER );
// 	CardDefinition const* silver = CardDefinition::GetCardDefinitionByType( eCards::SILVER );
// 	CardDefinition const* gold = CardDefinition::GetCardDefinitionByType( eCards::GOLD );
// 	CardDefinition const* estate = CardDefinition::GetCardDefinitionByType( eCards::ESTATE );
// 	CardDefinition const* duchy = CardDefinition::GetCardDefinitionByType( eCards::DUCHY );
// 	CardDefinition const* province = CardDefinition::GetCardDefinitionByType( eCards::PROVINCE );
// 	CardDefinition const* curse = CardDefinition::GetCardDefinitionByType( eCards::CURSE );
// 	
// 	CardDefinition const* village = CardDefinition::GetCardDefinitionByType( eCards::Village );
// 	CardDefinition const* smithy = CardDefinition::GetCardDefinitionByType( eCards::Smithy );
// 	CardDefinition const* festival = CardDefinition::GetCardDefinitionByType( eCards::Festival );
// 	CardDefinition const* laboratory = CardDefinition::GetCardDefinitionByType( eCards::Laboratory );
// 	CardDefinition const* market = CardDefinition::GetCardDefinitionByType( eCards::Market );
// 	CardDefinition const* councilRoom = CardDefinition::GetCardDefinitionByType( eCards::CouncilRoom );
// 	CardDefinition const* witch = CardDefinition::GetCardDefinitionByType( eCards::Witch );
// 
// 
// 	CardData_t copperData( copper, (int)COPPER );
// 	CardData_t estateData( estate, (int)ESTATE );
// 
// 	//7 copper
// 	starterCards.push_back( copperData );
// 	starterCards.push_back( copperData );
// 	starterCards.push_back( copperData );
// 	starterCards.push_back( copperData );
// 	starterCards.push_back( copperData );
// 	starterCards.push_back( copperData );
// 	starterCards.push_back( copperData );
// 	//3 estate
// 	starterCards.push_back( estateData );
// 	starterCards.push_back( estateData );
// 	starterCards.push_back( estateData );
// 	starterDeck.InitializeDeck( starterCards );

	//m_currentGameState = new gamestate_t();
	m_currentGameState = new gamestate_t( GetRandomInitialGameState() );
// 	m_currentGameState->m_isFirstMove = false;
// 	m_currentGameState->m_playerBoards[0] = starterDeck;
// 	m_currentGameState->m_playerBoards[1] = starterDeck;
// 	
// 	m_currentGameState->m_playerBoards[0].ShuffleDeck();
// 	m_currentGameState->m_playerBoards[1].ShuffleDeck();
// 
// 	m_currentGameState->m_playerBoards[0].Draw5();
// 	m_currentGameState->m_playerBoards[1].Draw5();
// 
// 	m_currentGameState->m_currentPhase = ACTION_PHASE;
// 
// 	//std::array<pileData_t>& cardPiles = m_currentGameState->m_cardPiles;
// 	pileData_t* cardPiles = &m_currentGameState->m_cardPiles[0];
// 	cardPiles[(int)eCards::COPPER].m_card = copper;
// 	cardPiles[(int)eCards::COPPER].m_pileSize = MONEYPILESIZE;
// 	cardPiles[(int)eCards::SILVER].m_card = silver;
// 	cardPiles[(int)eCards::SILVER].m_pileSize = MONEYPILESIZE;
// 	cardPiles[(int)eCards::GOLD].m_card = gold;
// 	cardPiles[(int)eCards::GOLD].m_pileSize = MONEYPILESIZE;
// 	cardPiles[(int)eCards::ESTATE].m_card = estate;
// 	cardPiles[(int)eCards::ESTATE].m_pileSize = VPPileSize;
// 	cardPiles[(int)eCards::DUCHY].m_card = duchy;
// 	cardPiles[(int)eCards::DUCHY].m_pileSize = VPPileSize;
// 	cardPiles[(int)eCards::PROVINCE].m_card = province;
// 	cardPiles[(int)eCards::PROVINCE].m_pileSize = VPPileSize;
// 	cardPiles[(int)eCards::CURSE].m_card = curse;
// 	cardPiles[(int)eCards::CURSE].m_pileSize = CURSEPILESIZE;
// 	cardPiles[(int)eCards::Village].m_card = village;
// 	cardPiles[(int)eCards::Village].m_pileSize = ACTIONPILESIZE;
// 	cardPiles[(int)eCards::Smithy].m_card = smithy;
// 	cardPiles[(int)eCards::Smithy].m_pileSize = ACTIONPILESIZE;
// 	cardPiles[(int)eCards::Festival].m_card = festival;
// 	cardPiles[(int)eCards::Festival].m_pileSize = ACTIONPILESIZE;
// 	cardPiles[(int)eCards::Laboratory].m_card = laboratory;
// 	cardPiles[(int)eCards::Laboratory].m_pileSize = ACTIONPILESIZE;
// 	cardPiles[(int)eCards::Market].m_card = market;
// 	cardPiles[(int)eCards::Market].m_pileSize = ACTIONPILESIZE;
// 	cardPiles[(int)eCards::CouncilRoom].m_card = councilRoom;
// 	cardPiles[(int)eCards::CouncilRoom].m_pileSize = ACTIONPILESIZE;
// 	cardPiles[(int)eCards::Witch].m_card = witch;
// 	cardPiles[(int)eCards::Witch].m_pileSize = ACTIONPILESIZE;
// 
// 	
// 	m_currentGameState->m_whoseMoveIsIt = PLAYER_1;

// 	m_mc->SetCurrentGameState( *m_currentGameState );
// 	m_mc->ResetPossibleMoves();
 	m_mcts->Shutdown();
 	m_mcts->Startup();
	m_mcts->SetInitialGameState( *m_currentGameState );
}

void Game::AddCountToCardWidget( Widget* cardWidget, int cardCount )
{
	AABB2 cardBounds = cardWidget->GetLocalAABB2();
	Vec3 cardCountPosition = cardBounds.GetPointAtUV( Vec2( 0.1f, 0.93f ) );
	Transform cardCountTransform = Transform();
	cardCountTransform.m_position = cardCountPosition;
	cardCountTransform.m_scale = Vec3( 0.2f, 0.2f, 1.f );
	Widget* countWidget = new Widget( cardCountTransform );
	countWidget->SetTexture( m_redTexture, m_redTexture, m_redTexture );
	countWidget->SetText( Stringf( "%i", cardCount ) );
	countWidget->SetTextSize( 0.1f );
	cardWidget->AddChild( countWidget );
}

void Game::UpdateCardCountOnWidget( Widget* cardWidget, int cardCount )
{
	if( cardWidget->m_childWidgets.size() > 0 )
	{
		Widget* childWidget = cardWidget->m_childWidgets[0];
		childWidget->SetText( Stringf( "%i", cardCount ) );
	}
	else
	{
		ERROR_AND_DIE( "Tried to update card count on a widget without card count" );
	}

}

void Game::RestartGame()
{
	*m_currentGameState = GetRandomInitialGameState();
	m_mcts->SetInitialGameState( *m_currentGameState );
}

void Game::CheckCollisions()
{}

void Game::UpdateEntities( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::RenderGame()
{
	//m_world->Render();
}

void Game::RenderUI()
{
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	const XboxController& controller = g_theInput->GetXboxController(0);
	UNUSED( deltaSeconds );
	UNUSED( controller );

 	const KeyButtonState& leftArrow = g_theInput->GetKeyStates( 0x25 );
// 	const KeyButtonState& upArrow = g_theInput->GetKeyStates( 0x26 );
 	const KeyButtonState& rightArrow = g_theInput->GetKeyStates( 0x27 );
// 	const KeyButtonState& downArrow = g_theInput->GetKeyStates( 0x28 );

// 	const KeyButtonState& wKey = g_theInput->GetKeyStates( 'W' );
// 	const KeyButtonState& aKey = g_theInput->GetKeyStates( 'A' );
// 	const KeyButtonState& sKey = g_theInput->GetKeyStates( 'S' );
// 	const KeyButtonState& dKey = g_theInput->GetKeyStates( 'D' );
// 	const KeyButtonState& cKey = g_theInput->GetKeyStates( 'C' );
// 	const KeyButtonState& spaceKey = g_theInput->GetKeyStates( SPACE_KEY );
// 	const KeyButtonState& shiftKey = g_theInput->GetKeyStates( SHIFT_KEY );
 	const KeyButtonState& f1Key = g_theInput->GetKeyStates( F1_KEY );
	const KeyButtonState& f2Key = g_theInput->GetKeyStates( F2_KEY );
	const KeyButtonState& f3Key = g_theInput->GetKeyStates( F3_KEY );
	const KeyButtonState& f4Key = g_theInput->GetKeyStates( F4_KEY );
 	const KeyButtonState& f5Key = g_theInput->GetKeyStates( F5_KEY );
 	const KeyButtonState& f6Key = g_theInput->GetKeyStates( F6_KEY );
 	const KeyButtonState& f7Key = g_theInput->GetKeyStates( F7_KEY );
 	const KeyButtonState& f8Key = g_theInput->GetKeyStates( F8_KEY );
	const KeyButtonState& f9Key = g_theInput->GetKeyStates( F9_KEY );
	const KeyButtonState& f10Key = g_theInput->GetKeyStates( F10_KEY );
 	const KeyButtonState& f11Key = g_theInput->GetKeyStates( F11_KEY );
//	const KeyButtonState& f12Key = g_theInput->GetKeyStates( F12_KEY );
	const KeyButtonState& num1Key = g_theInput->GetKeyStates( '1' );
	const KeyButtonState& num2Key = g_theInput->GetKeyStates( '2' );
	const KeyButtonState& num3Key = g_theInput->GetKeyStates( '3' );
 	const KeyButtonState& num4Key = g_theInput->GetKeyStates( '4' );
 	const KeyButtonState& num5Key = g_theInput->GetKeyStates( '5' );
 	const KeyButtonState& num6Key = g_theInput->GetKeyStates( '6' );
 	const KeyButtonState& num7Key = g_theInput->GetKeyStates( '7' );
 	const KeyButtonState& num8Key = g_theInput->GetKeyStates( '8' );
	const KeyButtonState& num9Key = g_theInput->GetKeyStates( '9' );
 	const KeyButtonState& qKey = g_theInput->GetKeyStates( 'Q' );
 	const KeyButtonState& wKey = g_theInput->GetKeyStates( 'W' );
 	const KeyButtonState& eKey = g_theInput->GetKeyStates( 'E' );
 	const KeyButtonState& rKey = g_theInput->GetKeyStates( 'R' );
 	const KeyButtonState& tKey = g_theInput->GetKeyStates( 'T' );
 	const KeyButtonState& yKey = g_theInput->GetKeyStates( 'Y' );
 	const KeyButtonState& uKey = g_theInput->GetKeyStates( 'U' );
 	const KeyButtonState& iKey = g_theInput->GetKeyStates( 'I' );
// 	const KeyButtonState& num0Key = g_theInput->GetKeyStates( '0' );
	const KeyButtonState& lBracketKey = g_theInput->GetKeyStates( LBRACKET_KEY );
	const KeyButtonState& rBracketKey = g_theInput->GetKeyStates( RBRACKET_KEY );
// 	const KeyButtonState& fKey = g_theInput->GetKeyStates( 'F' );
// 	const KeyButtonState& tKey = g_theInput->GetKeyStates( 'T' );
// 	const KeyButtonState& gKey = g_theInput->GetKeyStates( 'G' );
// 	const KeyButtonState& hKey = g_theInput->GetKeyStates( 'H' );
// 	//const KeyButtonState& yKey = g_theInput->GetKeyStates( 'Y' );
// 	const KeyButtonState& vKey = g_theInput->GetKeyStates( 'V' );
// 	const KeyButtonState& bKey = g_theInput->GetKeyStates( 'B' );
	const KeyButtonState& nKey = g_theInput->GetKeyStates( 'N' );
	const KeyButtonState& mKey = g_theInput->GetKeyStates( 'M' );
// 	const KeyButtonState& jKey = g_theInput->GetKeyStates( 'J' );
// 	const KeyButtonState& kKey = g_theInput->GetKeyStates( 'K' );
	const KeyButtonState& zKey = g_theInput->GetKeyStates( 'Z' );
	const KeyButtonState& xKey = g_theInput->GetKeyStates( 'X' );
	const KeyButtonState& plusKey = g_theInput->GetKeyStates( PLUS_KEY );
	const KeyButtonState& minusKey = g_theInput->GetKeyStates( MINUS_KEY );
	const KeyButtonState& semiColonKey = g_theInput->GetKeyStates( SEMICOLON_KEY );
	const KeyButtonState& singleQuoteKey = g_theInput->GetKeyStates( SINGLEQUOTE_KEY );
	const KeyButtonState& commaKey = g_theInput->GetKeyStates( COMMA_KEY );
	const KeyButtonState& periodKey = g_theInput->GetKeyStates( PERIOD_KEY );
	const KeyButtonState& enterKey = g_theInput->GetKeyStates( ENTER_KEY );

	if( lBracketKey.WasJustPressed() )
	{
		switch( m_player2Strategy )
		{
		case AIStrategy::RANDOM: m_player2Strategy = AIStrategy::DOUBLEWITCH;
			break;
		case AIStrategy::BIGMONEY: m_player2Strategy = AIStrategy::RANDOM;
			break;
		case AIStrategy::SINGLEWITCH: m_player2Strategy = AIStrategy::BIGMONEY;
			break;
		case AIStrategy::SARASUA1: m_player2Strategy = AIStrategy::SINGLEWITCH;
			break;
		case AIStrategy::MCTS: m_player2Strategy = AIStrategy::SARASUA1;
			break;
		case AIStrategy::DOUBLEWITCH: m_player2Strategy = AIStrategy::MCTS;
			break;
		default: ERROR_AND_DIE("Invalid AI strategy");
			break;
		}
	}
	if( rBracketKey.WasJustPressed() )
	{
		switch( m_player2Strategy )
		{
		case AIStrategy::RANDOM: m_player2Strategy = AIStrategy::BIGMONEY;
			break;
		case AIStrategy::BIGMONEY: m_player2Strategy = AIStrategy::SINGLEWITCH;
			break;
		case AIStrategy::SINGLEWITCH: m_player2Strategy = AIStrategy::SARASUA1;
			break;
		case AIStrategy::SARASUA1: m_player2Strategy = AIStrategy::MCTS;
			break;
		case AIStrategy::MCTS: m_player2Strategy = AIStrategy::DOUBLEWITCH;
			break;
		case AIStrategy::DOUBLEWITCH: m_player2Strategy = AIStrategy::RANDOM;
			break;
		default: ERROR_AND_DIE( "Invalid AI strategy" );
			break;
		}
	}
	if( semiColonKey.WasJustPressed() )
	{
		switch( m_mctsRolloutMethod )
		{
		case ROLLOUTMETHOD::RANDOM: m_mctsRolloutMethod = ROLLOUTMETHOD::EPSILONHEURISTIC;
			break;
		case ROLLOUTMETHOD::HEURISTIC: m_mctsRolloutMethod = ROLLOUTMETHOD::RANDOM;
			break;
		case ROLLOUTMETHOD::EPSILONHEURISTIC: m_mctsRolloutMethod = ROLLOUTMETHOD::HEURISTIC;
			break;
		default: ERROR_AND_DIE( "Invalid rollout method" );
			break;
		}

		m_mcts->SetRolloutMethod( m_mctsRolloutMethod );
	}
	if( singleQuoteKey.WasJustPressed() )
	{
		switch( m_mctsRolloutMethod )
		{
		case ROLLOUTMETHOD::RANDOM: m_mctsRolloutMethod = ROLLOUTMETHOD::HEURISTIC;
			break;
		case ROLLOUTMETHOD::HEURISTIC: m_mctsRolloutMethod = ROLLOUTMETHOD::EPSILONHEURISTIC;
			break;
		case ROLLOUTMETHOD::EPSILONHEURISTIC: m_mctsRolloutMethod = ROLLOUTMETHOD::RANDOM;
			break;
		default: ERROR_AND_DIE( "Invalid rollout method" );
			break;
		}
		m_mcts->SetRolloutMethod( m_mctsRolloutMethod );
	}
	if( plusKey.WasJustPressed() )
	{
		switch( m_mctsExpansionStrategy )
		{
		case EXPANSIONSTRATEGY::ALLMOVES: m_mctsExpansionStrategy = EXPANSIONSTRATEGY::HEURISTICS;
			break;
		case EXPANSIONSTRATEGY::HEURISTICS: m_mctsExpansionStrategy = EXPANSIONSTRATEGY::ALLMOVES;
			break;
		default: ERROR_AND_DIE( "Invalid expansion strategy" );
			break;
		}
		m_mcts->SetExpansionStrategy( m_mctsExpansionStrategy );
	}
	if( minusKey.WasJustPressed() )
	{
		switch( m_mctsExpansionStrategy )
		{
		case EXPANSIONSTRATEGY::ALLMOVES: m_mctsExpansionStrategy = EXPANSIONSTRATEGY::HEURISTICS;
			break;
		case EXPANSIONSTRATEGY::HEURISTICS: m_mctsExpansionStrategy = EXPANSIONSTRATEGY::ALLMOVES;
			break;
		default: ERROR_AND_DIE( "Invalid expansion strategy" );
			break;
		}
		m_mcts->SetExpansionStrategy( m_mctsExpansionStrategy );
	}
	if( zKey.IsPressed() )
	{
		m_mctsExplorationParameter -= deltaSeconds;
		m_mctsExplorationParameter = Max( 0.f, m_mctsExplorationParameter );

		m_mcts->SetExplorationParameter( m_mctsExplorationParameter );
	}
	if( xKey.IsPressed() )
	{
		m_mctsExplorationParameter += deltaSeconds;
		m_mctsExplorationParameter = Clampf( m_mctsExplorationParameter, 0.f, 30.f );

		m_mcts->SetExplorationParameter( m_mctsExplorationParameter );
	}
	if( nKey.IsPressed() )
	{
		m_mctsEpsilon -= 0.5f * deltaSeconds;
		m_mctsEpsilon = Clampf( m_mctsEpsilon, 0.f, 1.f );

		m_mcts->SetEpsilonValueZeroToOne( m_mctsEpsilon );
	}
	if( mKey.IsPressed() )
	{
		m_mctsEpsilon += 0.5f * deltaSeconds;
		m_mctsEpsilon = Clampf( m_mctsEpsilon, 0.f, 1.f );

		m_mcts->SetEpsilonValueZeroToOne( m_mctsEpsilon );
	}
	if( commaKey.WasJustPressed() )
	{
		switch( m_player1Strategy )
		{
		case AIStrategy::RANDOM: m_player1Strategy = AIStrategy::DOUBLEWITCH;
			break;
		case AIStrategy::BIGMONEY: m_player1Strategy = AIStrategy::RANDOM;
			break;
		case AIStrategy::SINGLEWITCH: m_player1Strategy = AIStrategy::BIGMONEY;
			break;
		case AIStrategy::SARASUA1: m_player1Strategy = AIStrategy::SINGLEWITCH;
			break;
		case AIStrategy::MCTS: m_player1Strategy = AIStrategy::SARASUA1;
			break;
		case AIStrategy::DOUBLEWITCH: m_player1Strategy = AIStrategy::MCTS;
			break;
		default: ERROR_AND_DIE("Invalid AI strategy");
			break;
		}
	}
	if( periodKey.WasJustPressed() )
	{
		switch( m_player1Strategy )
		{
		case AIStrategy::RANDOM: m_player1Strategy = AIStrategy::BIGMONEY;
			break;
		case AIStrategy::BIGMONEY: m_player1Strategy = AIStrategy::SINGLEWITCH;
			break;
		case AIStrategy::SINGLEWITCH: m_player1Strategy = AIStrategy::SARASUA1;
			break;
		case AIStrategy::SARASUA1: m_player1Strategy = AIStrategy::MCTS;
			break;
		case AIStrategy::MCTS: m_player1Strategy = AIStrategy::DOUBLEWITCH;
			break;
		case AIStrategy::DOUBLEWITCH: m_player1Strategy = AIStrategy::RANDOM;
			break;
		default: ERROR_AND_DIE("Invalid AI strategy");
			break;
		}
	}
	if( leftArrow.WasJustPressed() )
	{
		switch( m_mctsSimMethod )
		{
		case SIMMETHOD::RANDOM: m_mctsSimMethod = SIMMETHOD::DOUBLEWITCH;
			break;
		case SIMMETHOD::BIGMONEY: m_mctsSimMethod = SIMMETHOD::RANDOM;
			break;
		case SIMMETHOD::SINGLEWITCH: m_mctsSimMethod = SIMMETHOD::BIGMONEY;
			break;
		case SIMMETHOD::SARASUA1: m_mctsSimMethod = SIMMETHOD::SINGLEWITCH;
			break;
		case SIMMETHOD::GREEDY: m_mctsSimMethod = SIMMETHOD::SINGLEWITCH;
			break;
		case SIMMETHOD::DOUBLEWITCH: m_mctsSimMethod = SIMMETHOD::SARASUA1;
			break;
		default:
			break;
		}

		m_mcts->SetSimMethod( m_mctsSimMethod );
	}
	if( rightArrow.WasJustPressed() )
	{
		switch( m_mctsSimMethod )
		{
		case SIMMETHOD::RANDOM: m_mctsSimMethod = SIMMETHOD::BIGMONEY;
			break;
		case SIMMETHOD::BIGMONEY: m_mctsSimMethod = SIMMETHOD::SINGLEWITCH;
			break;
		case SIMMETHOD::SINGLEWITCH: m_mctsSimMethod = SIMMETHOD::SARASUA1;
			break;
		case  SIMMETHOD::SARASUA1: m_mctsSimMethod = SIMMETHOD::DOUBLEWITCH;
			break;
		case SIMMETHOD::GREEDY: m_mctsSimMethod = SIMMETHOD::RANDOM;
			break;
		case SIMMETHOD::DOUBLEWITCH: m_mctsSimMethod = SIMMETHOD::RANDOM;
			break;
		default:
			break;
		}

		m_mcts->SetSimMethod( m_mctsSimMethod );
	}
	if( f1Key.WasJustPressed() )
	{
		//AppendGameStateToFile( *m_currentGameState, "test.mcts" );

		m_mcts->StopThreads();
		m_mcts->FlushJobSystemQueues();
		m_mcts->SaveTree();

	}
	if( f2Key.WasJustPressed() )
	{
// 		//Load gamestate
// 		size_t outSize;
// 		byte* buffer = FileReadToNewBuffer( "test.mcts", &outSize );
// 		gamestate_t gameState = gamestate_t::ParseGameStateFromBuffer( buffer );
// 
// 		*m_currentGameState = gameState;
		m_mcts->StopThreads();
		m_mcts->FlushJobSystemQueues();
		m_mcts->LoadTree();
		m_mcts->StartThreads();
		m_mcts->SetInitialGameState( *m_currentGameState );
	}
	if( f3Key.WasJustPressed() )
	{
		m_mcts->AddSimulations( 1'000'000 );

	}
	if( f4Key.WasJustPressed() )
	{
		m_mcts->AddSimulations( 1 );
	}
	if( f5Key.WasJustPressed() )
	{
		inputMove_t bigMoneyAI = GetMoveUsingBigMoney( *m_currentGameState );
		PlayMoveIfValid( bigMoneyAI );

	}
	if( f6Key.WasJustPressed() )
	{
		if( m_currentGameState->m_whoseMoveIsIt == PLAYER_1 )
		{
			//inputMove_t move = m_mc->GetBestMove();
			inputMove_t move = GetBestMoveUsingAIStrategy( m_player1Strategy );
			if( move.m_moveType == INVALID_MOVE )
			{

			}
			else
			{
				PlayMoveIfValid( move );
			}

			DebugAddScreenPoint( Vec2( 0.5, 0.5f ), 100.f, Rgba8::YELLOW, 0.f );
			m_mcts->AddSimulations( 400 );
			m_simCount += 400;
		}
		else
		{
			inputMove_t move = GetBestMoveUsingAIStrategy( m_player2Strategy );
			PlayMoveIfValid( move );
		}
	}
	if( f7Key.WasJustPressed() )
	{
		m_isAutoPlayEnabled = !m_isAutoPlayEnabled;

		//InitializeGameState();
	}
	if( f8Key.WasJustPressed() )
	{
		RestartGame();
	}
	if( f9Key.WasJustPressed() )
	{
		m_randomMove = GetRandomMoveAtGameState( *m_currentGameState );
	}
	if( f10Key.WasJustPressed() )
	{
		PlayMoveIfValid( m_randomMove );
	}
	if( f11Key.WasJustPressed() )
	{
		m_mcts->AddSimulations( 1'000 );
		//m_mcts->RunSimulations( 10000 );
	}
	if( enterKey.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = END_PHASE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		PlayMoveIfValid( move );
	}

	if( num1Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 0;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			if( m_bufferedInputMove.m_cardIndex == eCards::Remodel )
			{
				if( m_bufferedInputMove.m_parameterCardIndex1 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex1 = m_currentGameState->m_playerBoards[m_currentGameState->m_whoseMoveIsIt].GetCardIndexFromHandIndex( 0 );
				}
				else if( m_bufferedInputMove.m_parameterCardIndex2 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex2 = 0;
					if( IsMoveValid( m_bufferedInputMove ) )
					{
						PlayMoveIfValid( m_bufferedInputMove );
					}

					m_bufferedInputMove = inputMove_t();
				}
			}
			else
			{
				inputMove_t move;
				move.m_moveType = PLAY_CARD;
				move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
				move.m_cardIndex = m_currentGameState->m_playerBoards[move.m_whoseMoveIsIt].GetCardIndexFromHandIndex( 0 );
				
				if( move.m_cardIndex == eCards::Remodel )
				{
					m_bufferedInputMove = move;
				}
				else
				{
					PlayMoveIfValid( move );
				}
			}

		}

	}
	if( num2Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 1;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			if( m_bufferedInputMove.m_cardIndex == eCards::Remodel )
			{
				if( m_bufferedInputMove.m_parameterCardIndex1 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex1 = m_currentGameState->m_playerBoards[m_currentGameState->m_whoseMoveIsIt].GetCardIndexFromHandIndex( 1 );
				}
				else if( m_bufferedInputMove.m_parameterCardIndex2 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex2 = 1;
					if( IsMoveValid( m_bufferedInputMove ) )
					{
						PlayMoveIfValid( m_bufferedInputMove );
					}
					m_bufferedInputMove = inputMove_t();
				}
			}
			else
			{
				inputMove_t move;
				move.m_moveType = PLAY_CARD;
				move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
				move.m_cardIndex = m_currentGameState->m_playerBoards[move.m_whoseMoveIsIt].GetCardIndexFromHandIndex( 1 );
				if( move.m_cardIndex == eCards::Remodel )
				{
					m_bufferedInputMove = move;
				}
				else
				{
					PlayMoveIfValid( move );
				}
			}
		}
	}
	if( num3Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 2;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			if( m_bufferedInputMove.m_cardIndex == eCards::Remodel )
			{
				if( m_bufferedInputMove.m_parameterCardIndex1 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex1 = m_currentGameState->m_playerBoards[m_currentGameState->m_whoseMoveIsIt].GetCardIndexFromHandIndex( 2 );
				}
				else if( m_bufferedInputMove.m_parameterCardIndex2 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex2 = 2;
					if( IsMoveValid( m_bufferedInputMove ) )
					{
						PlayMoveIfValid( m_bufferedInputMove );
					}
					m_bufferedInputMove = inputMove_t();
				}
			}
			else
			{
				inputMove_t move;
				move.m_moveType = PLAY_CARD;
				move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
				move.m_cardIndex = m_currentGameState->m_playerBoards[move.m_whoseMoveIsIt].GetCardIndexFromHandIndex( 2 );
				if( move.m_cardIndex == eCards::Remodel )
				{
					m_bufferedInputMove = move;
				}
				else
				{
					PlayMoveIfValid( move );
				}
			}
		}
	}
	if( num4Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 3;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			if( m_bufferedInputMove.m_cardIndex == eCards::Remodel )
			{
				if( m_bufferedInputMove.m_parameterCardIndex1 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex1 = m_currentGameState->m_playerBoards[m_currentGameState->m_whoseMoveIsIt].GetCardIndexFromHandIndex( 3 );
				}
				else if( m_bufferedInputMove.m_parameterCardIndex2 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex2 = 3;
					if( IsMoveValid( m_bufferedInputMove ) )
					{
						PlayMoveIfValid( m_bufferedInputMove );
					}
					m_bufferedInputMove = inputMove_t();
				}
			}
			else
			{
				inputMove_t move;
				move.m_moveType = PLAY_CARD;
				move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
				move.m_cardIndex = m_currentGameState->m_playerBoards[move.m_whoseMoveIsIt].GetCardIndexFromHandIndex( 3 );
				if( move.m_cardIndex == eCards::Remodel )
				{
					m_bufferedInputMove = move;
				}
				else
				{
					PlayMoveIfValid( move );
				}
			}
		}
	}
	if( num5Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 4;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			if( m_bufferedInputMove.m_cardIndex == eCards::Remodel )
			{
				if( m_bufferedInputMove.m_parameterCardIndex1 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex1 = m_currentGameState->m_playerBoards[m_currentGameState->m_whoseMoveIsIt].GetCardIndexFromHandIndex( 4 );
				}
				else if( m_bufferedInputMove.m_parameterCardIndex2 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex2 = 4;
					if( IsMoveValid( m_bufferedInputMove ) )
					{
						PlayMoveIfValid( m_bufferedInputMove );
					}
					m_bufferedInputMove = inputMove_t();
				}
			}
			else
			{
				inputMove_t move;
				move.m_moveType = PLAY_CARD;
				move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
				move.m_cardIndex = m_currentGameState->m_playerBoards[move.m_whoseMoveIsIt].GetCardIndexFromHandIndex( 4 );
				if( move.m_cardIndex == eCards::Remodel )
				{
					m_bufferedInputMove = move;
				}
				else
				{
					PlayMoveIfValid( move );
				}
			}
		}
	}
	if( num6Key.WasJustPressed() )
	{

		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 5;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			if( m_bufferedInputMove.m_cardIndex == eCards::Remodel )
			{
				if( m_bufferedInputMove.m_parameterCardIndex1 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex1 = m_currentGameState->m_playerBoards[m_currentGameState->m_whoseMoveIsIt].GetCardIndexFromHandIndex( 5 );
				}
				else if( m_bufferedInputMove.m_parameterCardIndex2 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex2 = 5;
					if( IsMoveValid( m_bufferedInputMove ) )
					{
						PlayMoveIfValid( m_bufferedInputMove );
					}
					m_bufferedInputMove = inputMove_t();
				}
			}
			else
			{
				inputMove_t move;
				move.m_moveType = PLAY_CARD;
				move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
				move.m_cardIndex = m_currentGameState->m_playerBoards[move.m_whoseMoveIsIt].GetCardIndexFromHandIndex( 5 );
				if( move.m_cardIndex == eCards::Remodel )
				{
					m_bufferedInputMove = move;
				}
				else
				{
					PlayMoveIfValid( move );
				}
			}
		}
	}
	if( num7Key.WasJustPressed() )
	{		

		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 6;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			if( m_bufferedInputMove.m_cardIndex == eCards::Remodel )
			{
				if( m_bufferedInputMove.m_parameterCardIndex1 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex1 = m_currentGameState->m_playerBoards[m_currentGameState->m_whoseMoveIsIt].GetCardIndexFromHandIndex( 6 );
				}
				else if( m_bufferedInputMove.m_parameterCardIndex2 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex2 = 6;
					if( IsMoveValid( m_bufferedInputMove ) )
					{
						PlayMoveIfValid( m_bufferedInputMove );
					}
					m_bufferedInputMove = inputMove_t();
				}
			}
			else
			{
				inputMove_t move;
				move.m_moveType = PLAY_CARD;
				move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
				move.m_cardIndex = m_currentGameState->m_playerBoards[move.m_whoseMoveIsIt].GetCardIndexFromHandIndex( 6 );
				if( move.m_cardIndex == eCards::Remodel )
				{
					m_bufferedInputMove = move;
				}
				else
				{
					PlayMoveIfValid( move );
				}
			}
		}
	}
	if( num8Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 7;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			if( m_bufferedInputMove.m_cardIndex == eCards::Remodel )
			{
				if( m_bufferedInputMove.m_parameterCardIndex1 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex1 = m_currentGameState->m_playerBoards[m_currentGameState->m_whoseMoveIsIt].GetCardIndexFromHandIndex( 7 );
				}
				else if( m_bufferedInputMove.m_parameterCardIndex2 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex2 = 7;
					if( IsMoveValid( m_bufferedInputMove ) )
					{
						PlayMoveIfValid( m_bufferedInputMove );
					}
					m_bufferedInputMove = inputMove_t();
				}
			}
			else
			{
				inputMove_t move;
				move.m_moveType = PLAY_CARD;
				move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
				move.m_cardIndex = m_currentGameState->m_playerBoards[move.m_whoseMoveIsIt].GetCardIndexFromHandIndex( 7 );
				if( move.m_cardIndex == eCards::Remodel )
				{
					m_bufferedInputMove = move;
				}
				else
				{
					PlayMoveIfValid( move );
				}
			}
		}
	}
	if( num9Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 8;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			if( m_bufferedInputMove.m_cardIndex == eCards::Remodel )
			{
				if( m_bufferedInputMove.m_parameterCardIndex1 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex1 = m_currentGameState->m_playerBoards[m_currentGameState->m_whoseMoveIsIt].GetCardIndexFromHandIndex( 8 );
				}
				else if( m_bufferedInputMove.m_parameterCardIndex2 == -1 )
				{
					m_bufferedInputMove.m_parameterCardIndex2 = 8;
					if( IsMoveValid( m_bufferedInputMove ) )
					{
						PlayMoveIfValid( m_bufferedInputMove );
					}
					m_bufferedInputMove = inputMove_t();
				}
			}
			else
			{
				inputMove_t move;
				move.m_moveType = PLAY_CARD;
				move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
				move.m_cardIndex = m_currentGameState->m_playerBoards[move.m_whoseMoveIsIt].GetCardIndexFromHandIndex( 8 );
				if( move.m_cardIndex == eCards::Remodel )
				{
					m_bufferedInputMove = move;
				}
				else
				{
					PlayMoveIfValid( move );
				}
			}
		}
	}
	if( qKey.WasJustPressed() )
	{
		if( m_bufferedInputMove.m_moveType == PLAY_CARD &&
			m_bufferedInputMove.m_cardIndex == eCards::Remodel &&
			m_bufferedInputMove.m_parameterCardIndex1 != -1 )
		{
			m_bufferedInputMove.m_parameterCardIndex2 = 8;
			if( IsMoveValid( m_bufferedInputMove ) )
			{
				PlayMoveIfValid( m_bufferedInputMove );
			}
			
			m_bufferedInputMove = inputMove_t();
		}
		else
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 8;
			PlayMoveIfValid( move );
		}
	}
	if( wKey.WasJustPressed() )
	{
		if( m_bufferedInputMove.m_moveType == PLAY_CARD &&
			m_bufferedInputMove.m_cardIndex == eCards::Remodel &&
			m_bufferedInputMove.m_parameterCardIndex1 != -1 )
		{
			m_bufferedInputMove.m_parameterCardIndex2 = 9;
			if( IsMoveValid( m_bufferedInputMove ) )
			{
				PlayMoveIfValid( m_bufferedInputMove );
			}

			m_bufferedInputMove = inputMove_t();
		}
		else
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 9;
			PlayMoveIfValid( move );
		}
	}
	if( eKey.WasJustPressed() )
	{
		if( m_bufferedInputMove.m_moveType == PLAY_CARD &&
			m_bufferedInputMove.m_cardIndex == eCards::Remodel &&
			m_bufferedInputMove.m_parameterCardIndex1 != -1 )
		{
			m_bufferedInputMove.m_parameterCardIndex2 = 10;
			if( IsMoveValid( m_bufferedInputMove ) )
			{
				PlayMoveIfValid( m_bufferedInputMove );
			}

			m_bufferedInputMove = inputMove_t();
		}
		else
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 10;
			PlayMoveIfValid( move );
		}
	}
	if( rKey.WasJustPressed() )
	{
		if( m_bufferedInputMove.m_moveType == PLAY_CARD &&
			m_bufferedInputMove.m_cardIndex == eCards::Remodel &&
			m_bufferedInputMove.m_parameterCardIndex1 != -1 )
		{
			m_bufferedInputMove.m_parameterCardIndex2 = 11;
			if( IsMoveValid( m_bufferedInputMove ) )
			{
				PlayMoveIfValid( m_bufferedInputMove );
			}

			m_bufferedInputMove = inputMove_t();
		}
		else
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 11;
			PlayMoveIfValid( move );
		}
	}
	if( tKey.WasJustPressed() )
	{
		if( m_bufferedInputMove.m_moveType == PLAY_CARD &&
			m_bufferedInputMove.m_cardIndex == eCards::Remodel &&
			m_bufferedInputMove.m_parameterCardIndex1 != -1 )
		{
			m_bufferedInputMove.m_parameterCardIndex2 = 12;
			if( IsMoveValid( m_bufferedInputMove ) )
			{
				PlayMoveIfValid( m_bufferedInputMove );
			}

			m_bufferedInputMove = inputMove_t();
		}
		else
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 12;
			PlayMoveIfValid( move );
		}
	}
	if( yKey.WasJustPressed() )
	{
		if( m_bufferedInputMove.m_moveType == PLAY_CARD &&
			m_bufferedInputMove.m_cardIndex == eCards::Remodel &&
			m_bufferedInputMove.m_parameterCardIndex1 != -1 )
		{
			m_bufferedInputMove.m_parameterCardIndex2 = 13;
			if( IsMoveValid( m_bufferedInputMove ) )
			{
				PlayMoveIfValid( m_bufferedInputMove );
			}

			m_bufferedInputMove = inputMove_t();
		}
		else
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 13;
			PlayMoveIfValid( move );
		}
	}
	if( uKey.WasJustPressed() )
	{
		if( m_bufferedInputMove.m_moveType == PLAY_CARD &&
			m_bufferedInputMove.m_cardIndex == eCards::Remodel &&
			m_bufferedInputMove.m_parameterCardIndex1 != -1 )
		{
			m_bufferedInputMove.m_parameterCardIndex2 = 14;
			if( IsMoveValid( m_bufferedInputMove ) )
			{
				PlayMoveIfValid( m_bufferedInputMove );
			}

			m_bufferedInputMove = inputMove_t();
		}
		else
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 14;
			PlayMoveIfValid( move );
		}
	}
	if( iKey.WasJustPressed() )
	{
		if( m_bufferedInputMove.m_moveType == PLAY_CARD &&
			m_bufferedInputMove.m_cardIndex == eCards::Remodel &&
			m_bufferedInputMove.m_parameterCardIndex1 != -1 )
		{
			m_bufferedInputMove.m_parameterCardIndex2 = 15;
			if( IsMoveValid( m_bufferedInputMove ) )
			{
				PlayMoveIfValid( m_bufferedInputMove );
			}

			m_bufferedInputMove = inputMove_t();
		}
		else
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = 15;
			PlayMoveIfValid( move );
		}
	}
}



void Game::DebugDrawGame()
{
	//Render Player 2s hand at top of screen
	int numberOfUniqueCards = CardPile::GetNumberOfPossibleUniqueCards();
	CardPile player1Hand = m_currentGameState->m_playerBoards[0].GetHand();
	CardPile player2Hand = m_currentGameState->m_playerBoards[1].GetHand();

	CardPile player1PlayArea = m_currentGameState->m_playerBoards[0].GetPlayArea();
	CardPile player2PlayArea = m_currentGameState->m_playerBoards[1].GetPlayArea();

	pileData_t const* piles = &m_currentGameState->m_cardPiles[0];


	Rgba8 textColor = Rgba8::BLACK;
	Rgba8 borderColor = Rgba8::BLACK;
	AABB2 gameBoard = DebugGetScreenBounds();
	DebugAddScreenAABB2( gameBoard, Rgba8::TuscanTan, 0.f );
	Vec2 gameDims = gameBoard.GetDimensions();
	//gameDims.y = gameDims.x;
	AABB2 gameDataArea = gameBoard;
	AABB2 deckDataArea = gameBoard;
	gameDims.x *= 0.8f;
	gameDims.y *= 0.9f;
	gameBoard.SetDimensions( gameDims );

	gameDataArea = gameDataArea.GetBoxAtLeft( 1.f / 9.f );
	Vec2 simMethodPos = gameDataArea.GetPointAtUV( Vec2( 0.1f, 0.93f ) );
	Vec2 expansionPos = gameDataArea.GetPointAtUV( Vec2( 0.1f, 0.8f ) );
	Vec2 rolloutPos = gameDataArea.GetPointAtUV( Vec2( 0.1f, 0.77f ) );
	Vec2 ucbPos = gameDataArea.GetPointAtUV( Vec2( 0.1f, 0.74f ) );
	Vec2 epsilonPos = gameDataArea.GetPointAtUV( Vec2( 0.1f, 0.71f ) );
	Vec2 phasePos = gameDataArea.GetPointAtUV( Vec2( 0.1f, 0.9f ) );
	Vec2 turnPos = gameDataArea.GetPointAtUV( Vec2( 0.1f, 0.85f ) );
	Vec2 coinPos = gameDataArea.GetPointAtUV( Vec2( 0.1f, 0.5f ) );
	Vec2 buysPos = gameDataArea.GetPointAtUV( Vec2( 0.1f, 0.4f ) );
	Vec2 actionsPos = gameDataArea.GetPointAtUV( Vec2( 0.1f, 0.3f ) );

	deckDataArea = deckDataArea.GetBoxAtRight( 1.f / 9.f );
	Vec2 player2DeckPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.8f ) );
	Vec2 player2DiscardPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.7f ) );
	Vec2 player2VPPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.6f ) );
	Vec2 player1DeckPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.3f ) );
	Vec2 player1DiscardPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.2f ) );
	Vec2 player1VPPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.1f ) );

	PlayerBoard const* playerDeck = nullptr;
	if( m_currentGameState->m_whoseMoveIsIt == PLAYER_1 )
	{
		playerDeck = &m_currentGameState->m_playerBoards[0];
	}
	else
	{
		playerDeck = &m_currentGameState->m_playerBoards[1];
	}

	int currentCoins = playerDeck->m_currentCoins;
	int buysAmount = playerDeck->m_numberOfBuysAvailable;
	int actionsAmount = playerDeck->m_numberOfActionsAvailable;

	std::string coinStr = Stringf( "Coins: %i", currentCoins );
	std::string buysStr = Stringf( "Buys: %i", buysAmount );
	std::string actionsStr = Stringf( "Actions: %i", actionsAmount );
	eGamePhase phase = m_currentGameState->m_currentPhase;
	int whoseTurn = m_currentGameState->m_whoseMoveIsIt;
	std::string phaseStr;
	std::string whoseTurnStr;
	std::string player1ActionPhaseStr = "Player 1: Action Phase";
	std::string player1BuyPhaseStr = "Player 1: Buy Phase";
	std::string player2ActionPhaseStr = "Player 2: Action Phase";
	std::string player2BuyPhaseStr = "Player 2: Buy Phase";
	std::string simMethodStr;
	std::string expansionStrategyStr;
	std::string rolloutStr;
	std::string epsilonStr;
	std::string ucbStr;

	switch( m_mctsSimMethod )
	{
	case SIMMETHOD::RANDOM: simMethodStr = "Larrow, Rarrow | Rollout Method: RANDOM";
		break;
	case SIMMETHOD::BIGMONEY: simMethodStr = "Larrow, Rarrow | Rollout Method: BIG MONEY";
		break;
	case SIMMETHOD::SINGLEWITCH: simMethodStr = "Larrow, Rarrow | Rollout Method: SINGLE Witch";
		break;
	case SIMMETHOD::GREEDY: simMethodStr = "Larrow, Rarrow | Rollout Method: GREEDY";
		break;
	case SIMMETHOD::SARASUA1: simMethodStr = "Larrow, Rarrow | Rollout Method: SARASUA1";
		break;
	case SIMMETHOD::DOUBLEWITCH: simMethodStr = "Larrow, Rarrow | Rollout Method: DOUBLE Witch";
		break;
	default: simMethodStr = "Larrow, Rarrow | Rollout Method: DEFAULT";
		break;
	}

	switch( m_mctsExpansionStrategy )
	{
	case EXPANSIONSTRATEGY::ALLMOVES: expansionStrategyStr = "-,+ | Expansion: All Moves";
		break;
	case EXPANSIONSTRATEGY::HEURISTICS: expansionStrategyStr = "-,+ | Expansion: Heuristics";
		break;
	default:
		break;
	}

	switch( m_mctsRolloutMethod )
	{
	case ROLLOUTMETHOD::RANDOM: rolloutStr = ";,' | Rollout: Random";
		break;
	case ROLLOUTMETHOD::HEURISTIC: rolloutStr = ";,' | Rollout: Heuristic";
		break;
	case ROLLOUTMETHOD::EPSILONHEURISTIC: rolloutStr = ";,' | Rollout: Epsilon Heuristic";
		break;
	default:
		break;
	}

	ucbStr = Stringf( "Z,X | UCB Score: %f", m_mctsExplorationParameter );
	epsilonStr = Stringf( "N,M | Epsilon: %f", m_mctsEpsilon );
	
	if( whoseTurn == PLAYER_1 )
	{
		whoseTurnStr = "Turn: Player 1";
	}
	else if( whoseTurn == PLAYER_2 )
	{
		whoseTurnStr = "Turn: Player 2";
	}
	else
	{
		whoseTurnStr = "No current player to move";
	}

	if( phase == ACTION_PHASE )
	{
		phaseStr = "Phase: Action";
	}
	else if( phase == BUY_PHASE )
	{
		phaseStr = "Phase: Buy";
	}
	else if( phase == CLEANUP_PHASE )
	{
		phaseStr = "Phase: Cleanup";
	}


	int player1DeckSize = (int)m_currentGameState->m_playerBoards[0].GetDeckSize();
	int player1DiscardSize = (int)m_currentGameState->m_playerBoards[0].GetDiscardSize();
	int player1VPCount = m_currentGameState->m_playerBoards[0].GetCurrentVictoryPoints();
	int player2DeckSize = (int)m_currentGameState->m_playerBoards[1].GetDeckSize();
	int player2DiscardSize = (int)m_currentGameState->m_playerBoards[1].GetDiscardSize();
	int player2VPCount = m_currentGameState->m_playerBoards[1].GetCurrentVictoryPoints();
	std::string player2DeckStr = Stringf( "Deck Size: %i", player2DeckSize );
	std::string player2DiscardStr = Stringf( "Discard Size: %i", player2DiscardSize );
	std::string player1DeckStr = Stringf( "Deck Size: %i", player1DeckSize );
	std::string player1DiscardStr = Stringf( "Discard Size: %i", player1DiscardSize );

	std::string player1VPStr = Stringf( "VP: %i", player1VPCount );
	std::string player2VPStr = Stringf( "VP: %i", player2VPCount );

	DebugAddScreenText( Vec4( Vec2(), player2DeckPos ), Vec2( 0.5f, 0.5f ), 12.f, textColor, textColor, 0.f, player2DeckStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), player2DiscardPos ), Vec2( 0.5f, 0.5f ), 12.f, textColor, textColor, 0.f, player2DiscardStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), player1DeckPos ), Vec2( 0.5f, 0.5f ), 12.f, textColor, textColor, 0.f, player1DeckStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), player1DiscardPos ), Vec2( 0.5f, 0.5f ), 12.f, textColor, textColor, 0.f, player1DiscardStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), player1VPPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, player1VPStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), player2VPPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, player2VPStr.c_str() );

	DebugAddScreenText( Vec4( Vec2(), simMethodPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, simMethodStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), phasePos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, phaseStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), turnPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, whoseTurnStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), coinPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, coinStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), buysPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, buysStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), actionsPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, actionsStr.c_str() );

	DebugAddScreenText( Vec4( Vec2(), expansionPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, expansionStrategyStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), rolloutPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, rolloutStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), ucbPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, ucbStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), epsilonPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, epsilonStr.c_str() );


	AABB2 carvingBoard = gameBoard;


	float handHeightFraction = 0.15f;
	float playAreaHeightFraction = 0.15f;
	float pileAreaHeightFraction = 0.4f;

	float currentProportionalFractionSize = 1.f;
	float currentProportionalSize = 1.f;
	float currentFraction = handHeightFraction;
	float fractionToCarve = currentFraction;


	AABB2 player2HandAABB = carvingBoard.CarveBoxOffTop( fractionToCarve );

	currentFraction = playAreaHeightFraction;
	currentProportionalFractionSize = 1.f - fractionToCarve;
	currentProportionalSize = currentProportionalSize / currentProportionalFractionSize;
	fractionToCarve = currentFraction * currentProportionalSize;

	AABB2 player2PlayAreaAABB = carvingBoard.CarveBoxOffTop( fractionToCarve );
	currentFraction = pileAreaHeightFraction;
	currentProportionalFractionSize = 1.f - fractionToCarve;
	currentProportionalSize = currentProportionalSize / currentProportionalFractionSize;
	fractionToCarve = currentFraction * currentProportionalSize;

	AABB2 pilesArea = carvingBoard.CarveBoxOffTop( fractionToCarve );

	currentFraction = playAreaHeightFraction;
	currentProportionalFractionSize = 1.f - fractionToCarve;
	currentProportionalSize = currentProportionalSize / currentProportionalFractionSize;
	fractionToCarve = currentFraction * currentProportionalSize;

	AABB2 player1PlayAreaAABB = carvingBoard.CarveBoxOffTop( fractionToCarve );

	// 	currentFraction = playAreaHeightFraction;
	// 	currentProportionalFractionSize = 1.f - fractionToCarve;
	// 	currentProportionalSize = currentProportionalSize / currentProportionalFractionSize;
	// 	fractionToCarve = currentFraction * currentProportionalSize;

	AABB2 player1HandAABB = carvingBoard;

	//AABB2 carvingPlayer2Hand = player2HandAABB;

	Rgba8 handColor = Rgba8::WindsorTan;
	Rgba8 playAreaColor = Rgba8::Tan;
	Rgba8 pileAreaColor = Rgba8::SandyTan;

	DebugAddScreenAABB2( player2HandAABB, handColor, 0.f );
	DebugAddScreenAABB2( player2PlayAreaAABB, playAreaColor, 0.f );
	DebugAddScreenAABB2( pilesArea, pileAreaColor, 0.f );
	DebugAddScreenAABB2( player1PlayAreaAABB, playAreaColor, 0.f );
	DebugAddScreenAABB2( player1HandAABB, handColor, 0.f );

	Vec4 debugCenter = Vec4( Vec2(), player2HandAABB.GetCenter() );

	float player2HandSize = (float)player2Hand.TotalCount();
	int player2HandIndex = 0;
	for( int cardIndex = 0; cardIndex < numberOfUniqueCards; cardIndex++ )
	{
		int countOfCard = player2Hand.CountOfCard( cardIndex );
		CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)cardIndex );
		while( countOfCard > 0 )
		{
			float carvingNumber = player2HandSize - (float)player2HandIndex;
			AABB2 cardArea = player2HandAABB.CarveBoxOffLeft( 1.f / carvingNumber );
			Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
			std::string cardName = card->GetCardName();
			DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, textColor, textColor, 0.f, cardName.c_str() );

			countOfCard--;
			player2HandIndex++;
		}
	}

	float player2PlayAreaSize = (float)player2PlayArea.TotalCount();
	int player2PlayAreaIndex = 0;
	for( int cardIndex = 0; cardIndex < numberOfUniqueCards; cardIndex++ )
	{
		int countOfCard = player2PlayArea.CountOfCard( cardIndex );
		CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)cardIndex );
		while( countOfCard > 0 )
		{
			float carvingNumber = player2PlayAreaSize - (float)player2PlayAreaIndex;
			AABB2 cardArea = player2PlayAreaAABB.CarveBoxOffLeft( 1.f / carvingNumber );
			Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
			std::string cardName = card->GetCardName();
			DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, textColor, textColor, 0.f, cardName.c_str() );

			countOfCard--;
			player2PlayAreaIndex++;
		}
	}

	std::vector<int> aiValidMoves = GetCurrentBuyIndexes();
	AABB2 bottomPilesArea;
	bool isTwoRows = false;
	int cardsPerRow = 8;
	float cardsPerRowFloat = (float)cardsPerRow;
	if( NUMBEROFPILES > cardsPerRow )
	{
		isTwoRows = true;
		bottomPilesArea = pilesArea.CarveBoxOffBottom( 0.5f );
	}
	for( size_t pilesIndex = 0; pilesIndex < NUMBEROFPILES; pilesIndex++ )
	{
		//float pileSize = 8.f;
		float carvingNumber = 0.f;

		AABB2 cardArea;
		if( isTwoRows )
		{
			if( pilesIndex < (size_t)cardsPerRow )
			{
				carvingNumber = cardsPerRowFloat - (float)pilesIndex;
				cardArea = pilesArea.CarveBoxOffLeft( 1.f / carvingNumber );
			}
			else
			{
				carvingNumber = (2.f*cardsPerRowFloat) - (float)pilesIndex;
				cardArea = bottomPilesArea.CarveBoxOffLeft( 1.f / carvingNumber );
			}
		}
		else
		{
			carvingNumber = cardsPerRowFloat - (float)pilesIndex;
			cardArea = pilesArea.CarveBoxOffLeft( 1.f / carvingNumber );
		}

		for( size_t aiValidIndexes = 0; aiValidIndexes < aiValidMoves.size(); aiValidIndexes++ )
		{
			int aiPileIndex = aiValidMoves[aiValidIndexes];
			if( pilesIndex == aiPileIndex )
			{
				DebugAddScreenAABB2( cardArea, Rgba8::RedBrown, 0.f );
				break;
			}
		}

		int cardIndex = piles[pilesIndex].m_cardIndex;
		CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)cardIndex );
		Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
		Vec4 cardCostPos = Vec4( Vec2(), cardArea.maxs );
		Vec4 cardCoinsPos = Vec4( Vec2(), cardArea.mins );
		Vec4 cardVPsPos = Vec4( Vec2(), Vec2( cardArea.maxs.x, cardArea.mins.y ) );
		Vec4 pileSizePos = Vec4( Vec2(), Vec2( cardArea.mins.x, cardArea.maxs.y ) );
		std::string cardName = card->GetCardName();
		std::string cardCost = Stringf( "Cost:%i", card->GetCardCost() );
		std::string cardCoins = Stringf( "Coins:%i", card->GetCoins() );
		std::string cardVPs = Stringf( "VPs:%i", card->GetCardVPs() );
		std::string pileCount = Stringf( "Count:%i", piles[pilesIndex].m_pileSize );
		DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 15.f, textColor, textColor, 0.f, cardName.c_str() );
		DebugAddScreenText( cardCostPos, Vec2( 1.1f, 1.5f ), 12.f, textColor, textColor, 0.f, cardCost.c_str() );
		DebugAddScreenText( cardCoinsPos, Vec2( -0.1f, -0.5f ), 12.f, textColor, textColor, 0.f, cardCoins.c_str() );
		DebugAddScreenText( cardVPsPos, Vec2( 1.1f, -0.5f ), 12.f, textColor, textColor, 0.f, cardVPs.c_str() );
		DebugAddScreenText( pileSizePos, Vec2( -0.1f, 1.5f ), 12.f, textColor, textColor, 0.f, pileCount.c_str() );
		DebugAddScreenAABB2Border( cardArea, borderColor, 1.5f, 0.f );
	}

	float player1PlayAreaSize = (float)player1PlayArea.TotalCount();
	int player1PlayAreaIndex = 0;
	for( int cardIndex = 0; cardIndex < numberOfUniqueCards; cardIndex++ )
	{
		int countOfCard = player1PlayArea.CountOfCard( cardIndex );
		CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)cardIndex );
		while( countOfCard > 0 )
		{
			float carvingNumber = player1PlayAreaSize - (float)player1PlayAreaIndex;
			AABB2 cardArea = player1PlayAreaAABB.CarveBoxOffLeft( 1.f / carvingNumber );
			Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
			std::string cardName = card->GetCardName();
			DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, textColor, textColor, 0.f, cardName.c_str() );

			countOfCard--;
			player1PlayAreaIndex++;
		}
	}

	float player1HandSize = (float)player1Hand.TotalCount();
	int player1HandIndex = 0;
	for( int cardIndex = 0; cardIndex < numberOfUniqueCards; cardIndex++ )
	{
		int countOfCard = player1Hand.CountOfCard( cardIndex );
		CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)cardIndex );
		while( countOfCard > 0 )
		{
			float carvingNumber = player1HandSize - (float)player1HandIndex;
			AABB2 cardArea = player1HandAABB.CarveBoxOffLeft( 1.f / carvingNumber );
			Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
			std::string cardName = card->GetCardName();
			DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, textColor, textColor, 0.f, cardName.c_str() );

			countOfCard--;
			player1HandIndex++;
		}
	}


	if( IsGameOver() == PLAYER_1 )
	{
		DebugAddScreenText( Vec4( 0.2f, 0.9f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Game Over: Player 1 won!" ).c_str() );
	}
	else if( IsGameOver() == PLAYER_2 )
	{
		DebugAddScreenText( Vec4( 0.2f, 0.9f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Game Over: Player 2 won!" ).c_str() );
	}
	else if( IsGameOver() == TIE )
	{
		DebugAddScreenText( Vec4( 0.2f, 0.9f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Game Over: Tie!" ).c_str() );
	}

	if( m_currentGameState->m_whoseMoveIsIt == PLAYER_1 )
	{
		inputMove_t player1BestMove = GetBestMoveUsingAIStrategy( m_player1Strategy );
		std::string aiStratStr;
		switch( m_player1Strategy )
		{
		case AIStrategy::RANDOM: aiStratStr = "RANDOM";
			break;
		case AIStrategy::BIGMONEY: aiStratStr = "BIGMONEY";
			break;
		case AIStrategy::SINGLEWITCH: aiStratStr = "SINGLEWITCH";
			break;
		case AIStrategy::SARASUA1: aiStratStr = "SARASUA1";
			break;
		case AIStrategy::MCTS: aiStratStr = "MCTS";
			break;
		case AIStrategy::DOUBLEWITCH: aiStratStr = "DOUBLEWITCH";
			break;
		default: aiStratStr = "Invalid Strat for player 1";
			break;
		}
		//inputMove_t currentBestMove = m_mc->GetBestMove();
		inputMove_t currentBestMove = player1BestMove;
		if( currentBestMove.m_moveType != INVALID_MOVE )
		{
			std::string moveStr;

			if( currentBestMove.m_moveType == BUY_MOVE )
			{
				eCards cardIndex = m_currentGameState->m_cardPiles[currentBestMove.m_cardIndex].m_cardIndex;
				CardDefinition const* card = CardDefinition::GetCardDefinitionByType( cardIndex );
				moveStr = Stringf( "Best Move for Player 1 using %s strategy: Buy %s", aiStratStr.c_str(), card->GetCardName().c_str() );
			}
			else if( currentBestMove.m_moveType == PLAY_CARD )
			{
				int cardHandIndex = currentBestMove.m_cardIndex;
				CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)cardHandIndex );
				moveStr = Stringf( "Best Move for Player 1 using %s strategy: Play %s", aiStratStr.c_str(), card->GetCardName().c_str() );
			}
			else if( currentBestMove.m_moveType == END_PHASE )
			{
				moveStr = Stringf( "Best Move for Player 1 using %s strategy: End Phase", aiStratStr.c_str() );
			}
			DebugAddScreenText( Vec4( 0.3f, 0.97f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::BLACK, Rgba8::BLACK, 0.f, moveStr.c_str() );
		}
		else
		{
			std::string moveStr;
			moveStr = Stringf( "Move for Player 1 using %s strategy is Invalid, MCTS needs sims", aiStratStr.c_str() );
			DebugAddScreenText( Vec4( 0.3f, 0.97f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::BLACK, Rgba8::BLACK, 0.f, moveStr.c_str() );
		}
	}
	else if( m_currentGameState->m_whoseMoveIsIt == PLAYER_2 )
	{
		inputMove_t player2BestMove = GetBestMoveUsingAIStrategy( m_player2Strategy );
		std::string aiStratStr;
		switch( m_player2Strategy )
		{
		case AIStrategy::RANDOM: aiStratStr = "RANDOM";
			break;
		case AIStrategy::BIGMONEY: aiStratStr = "BIGMONEY";
			break;
		case AIStrategy::SINGLEWITCH: aiStratStr = "SINGLEWITCH";
			break;
		case AIStrategy::SARASUA1: aiStratStr = "SARASUA1";
			break;
		case AIStrategy::MCTS: aiStratStr = "MCTS";
			break;
		case AIStrategy::DOUBLEWITCH: aiStratStr = "DOUBLEWITCH";
			break;
		default: aiStratStr = "Invalid Strat for player 2";
			break;
		}

		inputMove_t currentBestMove = player2BestMove;
		if( currentBestMove.m_moveType != INVALID_MOVE )
		{
			//inputMove_t bigMoneyMove = GetMoveUsingSingleWitch( *m_currentGameState );
			//inputMove_t bigMoneyMove = GetMoveUsingBigMoney( *m_currentGameState );
			std::string moveStr;
			if( currentBestMove.m_moveType == BUY_MOVE )
			{
				eCards cardIndex = m_currentGameState->m_cardPiles[currentBestMove.m_cardIndex].m_cardIndex;
				CardDefinition const* card = CardDefinition::GetCardDefinitionByType( cardIndex );
				moveStr = Stringf( "Best Move for Player 2 using %s strategy: Buy %s", aiStratStr.c_str(), card->GetCardName().c_str() );
			}
			else if( currentBestMove.m_moveType == PLAY_CARD )
			{
				int cardHandIndex = currentBestMove.m_cardIndex;
				CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)cardHandIndex );
				moveStr = Stringf( "Best Move for Player 2 using %s strategy: Play %s", aiStratStr.c_str(), card->GetCardName().c_str() );
			}
			else if( currentBestMove.m_moveType == END_PHASE )
			{
				moveStr = Stringf( "Best Move for Player 2 using %s strategy: End Phase", aiStratStr.c_str() );
			}
			DebugAddScreenText( Vec4( 0.3f, 0.97f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::BLACK, Rgba8::BLACK, 0.f, moveStr.c_str() );
		}
		else
		{
			std::string moveStr;
			moveStr = Stringf( "Move for Player 2 using %s strategy is Invalid, MCTS needs sims", aiStratStr.c_str() );
			DebugAddScreenText( Vec4( 0.3f, 0.97f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::BLACK, Rgba8::BLACK, 0.f, moveStr.c_str() );
		}

	}

	if( m_randomMove.m_moveType != INVALID_MOVE )
	{
		std::string randomMoveStr;
		if( m_randomMove.m_moveType == BUY_MOVE )
		{
			eCards cardIndex = m_currentGameState->m_cardPiles[m_randomMove.m_cardIndex].m_cardIndex;
			CardDefinition const* card = CardDefinition::GetCardDefinitionByType( cardIndex );
			randomMoveStr = Stringf( "Random Move: Buy %s", card->GetCardName().c_str() );
		}
		else if( m_randomMove.m_moveType == PLAY_CARD )
		{
			int cardHandIndex = m_randomMove.m_cardIndex;
			CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)cardHandIndex );
			randomMoveStr = Stringf( "Random Move: Play %s", card->GetCardName().c_str() );
		}
		else if( m_randomMove.m_moveType == END_PHASE )
		{
			randomMoveStr = Stringf( "Random move: End Phase" );
		}
		DebugAddScreenText( Vec4( 0.5f, 0.93f, 0.f, 0.f ), Vec2(), 10.f, Rgba8::BLACK, Rgba8::BLACK, 0.f, randomMoveStr.c_str() );
	}


	m_totalSimCount = m_mcts->GetNumberOfSimulationsRun();
	m_simCount = m_mcts->GetCurrentNumberOfSimulationsLeft();
	DebugAddScreenText( Vec4( 0.5f, 0.92f, 0.f, 0.f ), Vec2(), 12.f, Rgba8::BLACK, Rgba8::BLACK, 0.f, Stringf( "Current sim count: %i", m_totalSimCount ).c_str() );
	DebugAddScreenText( Vec4( 0.5f, 0.9f, 0.f, 0.f ), Vec2(), 12.f, Rgba8::BLACK, Rgba8::BLACK, 0.f, Stringf( "Current remaining sim count: %i", m_simCount ).c_str() );

// 	DebugAddScreenText( Vec4( 0.5f, 0.85f, 0.f, 0.f ), Vec2(), 10.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Current total Sim time: %f", (float)m_mcts->m_totalTime ).c_str() );
// 	DebugAddScreenText( Vec4( 0.5f, 0.83f, 0.f, 0.f ), Vec2(), 10.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Current select time: %f", (float)m_mcts->m_selectTime ).c_str() );
// 	DebugAddScreenText( Vec4( 0.5f, 0.81f, 0.f, 0.f ), Vec2(), 10.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Current expand time: %f", (float)m_mcts->m_expandTime ).c_str() );
// 	DebugAddScreenText( Vec4( 0.5f, 0.79f, 0.f, 0.f ), Vec2(), 10.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Current sim time: %f", (float)m_mcts->m_simTime ).c_str() );
// 	DebugAddScreenText( Vec4( 0.5f, 0.77f, 0.f, 0.f ), Vec2(), 10.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Current backpropagation time: %f", (float)m_mcts->m_backpropagationTime ).c_str() );



	if( m_isDebugScreenEnabled )
	{
		DebugDrawGameStateInfo();
	}

}

void Game::DebugDrawGameStateInfo()
{
	AABB2 debugDrawCanvas = DebugGetScreenBounds();
	Vec2 gameDims = debugDrawCanvas.GetDimensions();
	debugDrawCanvas.ScaleDimensionsNonUniform( Vec2( 0.5f, 0.8f ) );


	AABB2 leftColumn = debugDrawCanvas.CarveBoxOffLeft( 1.f / 3.f );
	AABB2 middleColumn = debugDrawCanvas.CarveBoxOffLeft( 1.f / 2.f );
	AABB2 rightColum = debugDrawCanvas;

	AABB2 validMovesBox = leftColumn.CarveBoxOffTop( 1.f / 5.f );
	AABB2 bigMoneyBox = middleColumn.CarveBoxOffTop( 1.f / 5.f );
	AABB2 mcBox = rightColum.CarveBoxOffTop( 1.f / 5.f );

	DebugAddScreenAABB2( validMovesBox, Rgba8::TuscanTan, 0.f );
	DebugAddScreenAABB2( bigMoneyBox, Rgba8::TuscanTan, 0.f );
	DebugAddScreenAABB2( mcBox, Rgba8::TuscanTan, 0.f );
	DebugAddScreenAABB2Border( validMovesBox, Rgba8::BLACK, 10.f, 0.f );
	DebugAddScreenAABB2Border( bigMoneyBox, Rgba8::BLACK, 10.f, 0.f );
	DebugAddScreenAABB2Border( mcBox, Rgba8::BLACK, 10.f, 0.f );
	DebugAddScreenText( Vec4( Vec2(), validMovesBox.GetCenter() ), Vec2( 0.5f, 0.5f ), 20.f, Rgba8::BLACK, Rgba8::BLACK, 0.f, "Valid Moves" );
	DebugAddScreenText( Vec4( Vec2(), bigMoneyBox.GetCenter() ), Vec2( 0.5f, 0.5f ), 20.f, Rgba8::BLACK, Rgba8::BLACK, 0.f, "Big Money" );
	DebugAddScreenText( Vec4( Vec2(), mcBox.GetCenter() ), Vec2( 0.5f, 0.5f ), 20.f, Rgba8::BLACK, Rgba8::BLACK, 0.f, "MonteCarlo" );

	std::vector<inputMove_t> validMoves = GetValidMovesAtGameState( *m_currentGameState );
	int numberOfValidMoves = (int)validMoves.size();
	std::vector<AABB2> validMovesAABBs = leftColumn.GetBoxAsRows( numberOfValidMoves );

	//FINISH

	inputMove_t aiMove = m_mc->GetBestMove();
	if( aiMove.m_moveType == INVALID_MOVE )
	{

	
	}

}

void Game::AutoPlayGame()
{
	if( IsGameOver() == GAMENOTOVER )
	{
		if( m_currentGameState->m_whoseMoveIsIt == PLAYER_1 )
		{
			if( m_player1Strategy == AIStrategy::MCTS )
			{
				m_simCount = m_mcts->GetCurrentNumberOfSimulationsLeft();
				if( m_timer.CheckAndDecrementAll() )
				{
					m_mcts->AddSimulations( 5000 );

					//inputMove_t move = m_mc->GetBestMove();
					inputMove_t move = m_mcts->GetBestMove();
					if( move.m_moveType == INVALID_MOVE )
					{

					}
					else
					{
						PlayMoveIfValid( move );
					}

					DebugAddScreenPoint( Vec2( 0.5, 0.5f ), 100.f, Rgba8::YELLOW, 0.f );
				}
				else
				{

				}
			}
			else
			{
				inputMove_t move = GetBestMoveUsingAIStrategy( m_player1Strategy );
				PlayMoveIfValid( move );
			}
			

		}
		else
		{
			if( m_player2Strategy == AIStrategy::MCTS )
			{
				m_simCount = m_mcts->GetCurrentNumberOfSimulationsLeft();
				if( m_simCount < 500 )
				{
					if( m_timer.CheckAndDecrement() )
					{
						m_mcts->AddSimulations( 1000 );

						//inputMove_t move = m_mc->GetBestMove();
						inputMove_t move = m_mcts->GetBestMove();
						if( move.m_moveType == INVALID_MOVE )
						{

						}
						else
						{
							PlayMoveIfValid( move );
						}

						DebugAddScreenPoint( Vec2( 0.5, 0.5f ), 100.f, Rgba8::YELLOW, 0.f );
					}
					else
					{

					}
				}
			}
			else
			{
				inputMove_t move = GetBestMoveUsingAIStrategy( m_player2Strategy );
				PlayMoveIfValid( move );
			}
		}
	}

}

void Game::PlayMoveIfValid( inputMove_t const& moveToPlay )
{
	if( IsGameOver() != GAMENOTOVER )
	{
		return;
	}
	if( IsMoveValid( moveToPlay ) )
	{
		PlayerBoard* playerDeck = nullptr;
		int nextPlayersTurn = 0;
		int whoseMoveBase1 = moveToPlay.m_whoseMoveIsIt + 1;
		if( moveToPlay.m_whoseMoveIsIt == PLAYER_1 )
		{
			playerDeck = &m_currentGameState->m_playerBoards[0];
			nextPlayersTurn = PLAYER_2;
		}
		else
		{
			playerDeck = &m_currentGameState->m_playerBoards[1];
			nextPlayersTurn = PLAYER_1;
		}


		if( moveToPlay.m_moveType == PLAY_CARD )
		{
			if( playerDeck->m_numberOfActionsAvailable > 0 )
			{
				playerDeck->m_numberOfActionsAvailable--;
				int handIndex = moveToPlay.m_cardIndex;
				playerDeck->PlayCard( moveToPlay, m_currentGameState );
				
				eCards eCardToTrash = (eCards)moveToPlay.m_parameterCardIndex1;
				eCards eCardToAcquire = (eCards)moveToPlay.m_parameterCardIndex2;

 				CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)handIndex );

				if( handIndex == eCards::Remodel )
				{
					CardDefinition const* cardToTrash = CardDefinition::GetCardDefinitionByType( eCardToTrash );
					CardDefinition const* cardToAcquire = CardDefinition::GetCardDefinitionByType( eCardToAcquire );
					std::string playStr = Stringf( "Player %i played %s, Trashing %s and Acquiring %s", whoseMoveBase1, card->GetCardName().c_str(), cardToTrash->GetCardName().c_str(), cardToAcquire->GetCardName().c_str() );
					g_theConsole->PrintString( Rgba8::CYAN, playStr );
				}
				else
				{
					std::string playStr = Stringf( "Player %i played %s", whoseMoveBase1, card->GetCardName().c_str() );
					g_theConsole->PrintString( Rgba8::CYAN, playStr );
				}

			}
		}
		else if( moveToPlay.m_moveType == BUY_MOVE )
		{
			int pileIndex = moveToPlay.m_cardIndex;
			pileData_t& pileData = m_currentGameState->m_cardPiles[pileIndex];
			pileData.m_pileSize -= 1;
			CardDefinition const* card = CardDefinition::GetCardDefinitionByType( pileData.m_cardIndex );
			int cardCost = card->GetCardCost();
			//Decrement coins
			//Put card in discard pile
			playerDeck->DecrementCoins( cardCost );
			playerDeck->AddCardToDiscardPile( pileIndex );
			playerDeck->m_numberOfBuysAvailable--;

			std::string playStr = Stringf( "Player %i bought %s", whoseMoveBase1, card->GetCardName().c_str() );
			g_theConsole->PrintString( Rgba8::CYAN, playStr );
		}
		else if( moveToPlay.m_moveType == END_PHASE )
		{
			if( m_currentGameState->m_currentPhase == ACTION_PHASE )
			{
				//PLAY ALL TREASURE CARDS
				playerDeck->PlayTreasureCards();
				m_currentGameState->m_currentPhase = BUY_PHASE;
			}
			else if( m_currentGameState->m_currentPhase == BUY_PHASE )
			{
				//DISCARD ALL CARDS, DRAW NEW HAND, AND PASS TURN
				playerDeck->DiscardHand();
				playerDeck->DiscardPlayArea();
				playerDeck->Draw5();

				m_currentGameState->m_currentPhase = ACTION_PHASE;
				m_currentGameState->m_whoseMoveIsIt = nextPlayersTurn;
				playerDeck->m_numberOfActionsAvailable = 1;
				playerDeck->m_numberOfBuysAvailable = 1;
				playerDeck->m_currentCoins = 0;
			}


			std::string playStr = Stringf( "Player %i ended phase", whoseMoveBase1 );
			g_theConsole->PrintString( Rgba8::CYAN, playStr );

			std::string phaseStr;
			if( m_currentGameState->m_currentPhase == BUY_PHASE )
			{
				phaseStr = "Buy Phase";
			}
			else
			{
				phaseStr = "Action Phase";
			}
			if( m_currentGameState->m_whoseMoveIsIt == nextPlayersTurn )
			{
				std::string startTurnStr = Stringf( "Beginning player %i's turn", nextPlayersTurn + 1 );
				g_theConsole->PrintString( Rgba8::RED, startTurnStr );
			}
			std::string currentPhaseStr = Stringf( "Current phase: %s", phaseStr.c_str() );
			g_theConsole->PrintString( Rgba8::GREEN, currentPhaseStr );
		}
		else
		{
			ERROR_AND_DIE( "INVALID PHASE" );
			return;
		}

	}

	m_isUIDirty = true;
	m_mc->SetCurrentGameState( *m_currentGameState );
	m_mc->ResetPossibleMoves();

	m_mcts->UpdateGame( moveToPlay, *m_currentGameState );
	m_randomMove = inputMove_t();
}

bool Game::PlayMoveIfValid( EventArgs const& args )
{
	inputMove_t moveToPlay = inputMove_t::CreateFromEventArgs( args );
	PlayMoveIfValid( moveToPlay );

	//m_isUIDirty = true;

	return true;
}

bool Game::ToggleWhoseViewedOnUI( EventArgs const& args )
{
	UNUSED( args );

	m_whoseUIPlaying++;
	if( m_whoseUIPlaying != PLAYER_2 )
	{
		m_whoseUIPlaying = PLAYER_1;
	}

	m_isUIDirty = true;

	return true;
}

bool Game::ToggleAIScreen( EventArgs const& args )
{
	UNUSED( args );
	m_isAIMoreInfoScreenActive = !m_isAIMoreInfoScreenActive;

	if( m_isAIMoreInfoScreenActive )
	{
		m_AIMoreInfoWidget->SetIsEnabled( true );
	}
	else
	{
		m_AIMoreInfoWidget->SetIsEnabled( false );
	}
	return true;
}

bool Game::PlayCurrentAIMove( EventArgs const& args )
{
	UNUSED( args );
	if( m_currentGameState->m_whoseMoveIsIt == PLAYER_1 )
	{
		inputMove_t move = GetBestMoveUsingAIStrategy( m_player1Strategy );
		if( move.m_moveType == INVALID_MOVE )
		{

		}
		else
		{
			PlayMoveIfValid( move );
		}

		DebugAddScreenPoint( Vec2( 0.5, 0.5f ), 100.f, Rgba8::YELLOW, 0.f );
		m_mcts->AddSimulations( 400 );
		m_simCount += 400;
	}
	else
	{
		inputMove_t move = GetBestMoveUsingAIStrategy( m_player2Strategy );
		PlayMoveIfValid( move );
	}

	return true;
}

bool Game::ToggleAutoPlay( EventArgs const& args )
{
	UNUSED( args );

	m_isAutoPlayEnabled = !m_isAutoPlayEnabled;
	m_isUIDirty = true;
	return true;
}

bool Game::IsMoveValid( inputMove_t const& moveToPlay ) const
{
	return IsMoveValidForGameState( moveToPlay, *m_currentGameState );
}

bool Game::IsMoveValidForGameState( inputMove_t const& moveToPlay, gamestate_t const& gameState ) const
{
	eGamePhase const& gamePhase = gameState.m_currentPhase;
	eMoveType const& moveType = moveToPlay.m_moveType;
	int whoseMove = moveToPlay.m_whoseMoveIsIt;

	if( whoseMove != gameState.m_whoseMoveIsIt )
	{
		return false;
	}

	if( gamePhase != moveType && moveType != END_PHASE )
	{
		return false;
	}

	PlayerBoard const* playerDeck = nullptr;
	if( whoseMove == PLAYER_1 )
	{
		playerDeck = &gameState.m_playerBoards[0];
	}
	else
	{
		playerDeck = &gameState.m_playerBoards[1];
	}

	if( moveType == BUY_MOVE )
	{
		if( playerDeck->m_numberOfBuysAvailable > 0 )
		{
			int cardIndex = moveToPlay.m_cardIndex;
			if( cardIndex >= 0 && cardIndex < NUMBEROFPILES )
			{
				pileData_t const& pileData = gameState.m_cardPiles[cardIndex];
				CardDefinition const* card = CardDefinition::GetCardDefinitionByType( pileData.m_cardIndex );
				if( pileData.m_pileSize > 0 )
				{
					if( card->GetCardCost() <= playerDeck->GetCurrentMoney() )
					{
						return true;
					}
				}
			}
		}

		return false;
	}
	else if( moveType == PLAY_CARD )
	{
		return playerDeck->CanPlayCard( moveToPlay, &gameState );
	}
	else if( moveType == END_PHASE )
	{
		return true;
	}
	else
	{
		ERROR_AND_DIE( "INVALID PHASE" );
		//return false;
	}

}

int Game::IsGameOverForGameState( gamestate_t const& gameState ) const
{
	bool isGameOver = false;
	pileData_t const& provinceData = gameState.m_cardPiles[(int)eCards::PROVINCE];
	if( provinceData.m_pileSize <= 0 )
	{
		isGameOver = true;
	}
	else
	{
		int emptyPileCount = 0;
		for( size_t pileIndex = 0; pileIndex < NUMBEROFPILES; pileIndex++ )
		{
			pileData_t const& pileData = gameState.m_cardPiles[pileIndex];
			CardDefinition const* card = CardDefinition::GetCardDefinitionByType( pileData.m_cardIndex );
			if( card && pileData.m_pileSize <= 0 )
			{
				emptyPileCount++;
			}
		}
		if( emptyPileCount >= 3 )
		{
			isGameOver = true;
		}
	}

	if( isGameOver )
	{
		//Victory goes to highest score. In case of tie, it goes to who has played fewer turns. If still a tie, then its a tie.
		int player1Score = gameState.m_playerBoards[0].GetCurrentVictoryPoints();
		int player2Score = gameState.m_playerBoards[1].GetCurrentVictoryPoints();
		int whoseTurnIsIt = gameState.m_whoseMoveIsIt;
		if( player1Score > player2Score )
		{
			return PLAYER_1;
		}
		else if( player2Score > player1Score )
		{
			return PLAYER_2;
		}
		else if( whoseTurnIsIt == PLAYER_1 )
		{
			return PLAYER_2;
		}
		else
		{
			return TIE;
		}
	}

	//Game is not over
	return GAMENOTOVER;
}

int Game::IsGameOver()
{
	return IsGameOverForGameState( *m_currentGameState );
}



std::vector<inputMove_t> Game::GetValidMovesAtGameState( gamestate_t const& gameState ) const
{
	std::vector<inputMove_t> validMoves;

	if( IsGameOverForGameState( gameState ) != GAMENOTOVER )
	{
		return validMoves;
	}

	int whoseMove = gameState.m_whoseMoveIsIt;
	eGamePhase currentPhase = gameState.m_currentPhase;
	
	//You can always end the phase
	inputMove_t move;
	move.m_whoseMoveIsIt = whoseMove;
	move.m_moveType = END_PHASE;
	validMoves.push_back( move );

	PlayerBoard const* playerDeck = nullptr;
	pileData_t const* piles = &gameState.m_cardPiles[0];
	if( whoseMove == PLAYER_1 )
	{
		playerDeck = &gameState.m_playerBoards[0];
	}
	else
	{
		playerDeck = &gameState.m_playerBoards[1];
	}

	if( currentPhase == ACTION_PHASE )
	{
		int numberOfActions = playerDeck->m_numberOfActionsAvailable;
		if( numberOfActions > 0 )
		{
			CardPile const& hand = playerDeck->GetHand();
			for( int cardIndex = eCards::Village; cardIndex < eCards::NUM_CARDS; cardIndex++ )
			{
				if( hand.CountOfCard( cardIndex ) > 0 )
				{
					if( cardIndex == eCards::Remodel )
					{
						for( int trashCardIndex = 0; trashCardIndex < eCards::NUM_CARDS; trashCardIndex++ )
						{
							if( trashCardIndex != eCards::Remodel || (trashCardIndex == eCards::Remodel && hand.CountOfCard( trashCardIndex ) > 1) )
							{
								CardDefinition const* trashCard = CardDefinition::GetCardDefinitionByType( (eCards)trashCardIndex );
								int trashCardCost = trashCard->GetCardCost();
								for( int acquireCardIndex = 0; acquireCardIndex < eCards::NUM_CARDS; acquireCardIndex++ )
								{
									CardDefinition const* acquireCard = CardDefinition::GetCardDefinitionByType( (eCards)acquireCardIndex );
									int acquireCardPileSize = gameState.m_cardPiles[acquireCardIndex].m_pileSize;
									int acquireCardCost = acquireCard->GetCardCost();
									if( acquireCardCost <= trashCardCost + 2 && acquireCardPileSize > 0 )
									{
										inputMove_t newMove = move;
										newMove.m_moveType = PLAY_CARD;
										newMove.m_cardIndex = cardIndex;
										newMove.m_parameterCardIndex1 = trashCardIndex;
										newMove.m_parameterCardIndex2 = acquireCardIndex;
										validMoves.push_back( newMove );
									}
								}
							}
						}
					}
					else
					{
						inputMove_t newMove = move;
						newMove.m_moveType = PLAY_CARD;
						newMove.m_cardIndex = cardIndex;
						validMoves.push_back( newMove );
					}
				}
			}
		}
	}
	else if( currentPhase == BUY_PHASE )
	{
		int numberOfBuys = playerDeck->m_numberOfBuysAvailable;
		if( numberOfBuys > 0 )
		{

			int currentMoney = playerDeck->GetCurrentMoney();

			for( size_t pileIndex = 0; pileIndex < NUMBEROFPILES; pileIndex++ )
			{
				eCards cardIndex = piles[pileIndex].m_cardIndex;
				CardDefinition const* card = CardDefinition::GetCardDefinitionByType( cardIndex );
				int pileSize = piles[pileIndex].m_pileSize;
				int cardCost = card->GetCardCost();

				if( pileSize > 0 && currentMoney >= cardCost )
				{
					inputMove_t newMove = move;
					newMove.m_moveType = BUY_MOVE;
					newMove.m_cardIndex = (int)pileIndex;
					validMoves.push_back( newMove );
				}
			}
		}
	}

	return validMoves;
}

int Game::GetNumberOfValidMovesAtGameState( gamestate_t const& gameState )
{
	int numberOfValidMoves = 0;

	if( IsGameOverForGameState( gameState ) != GAMENOTOVER )
	{
		return numberOfValidMoves;
	}

	//You can always end the phase as a move
	numberOfValidMoves++;

	eGamePhase currentPhase = gameState.m_currentPhase;
	int whoseMove = gameState.m_whoseMoveIsIt;
	
	PlayerBoard const* playerDeck = nullptr;
	if( whoseMove == PLAYER_1 )
	{
		playerDeck = &gameState.m_playerBoards[0];
	}
	else
	{
		playerDeck = &gameState.m_playerBoards[1];
	}

	if( currentPhase == ACTION_PHASE )
	{
		numberOfValidMoves += playerDeck->GetNumberOfValidSimpleActionsToPlay();
		CardPile const& hand = playerDeck->GetHand();
		if( hand.CountOfCard( eCards::Remodel ) > 0 )
		{
			for( int trashCardIndex = 0; trashCardIndex < eCards::NUM_CARDS; trashCardIndex++ )
			{
				if( trashCardIndex != eCards::Remodel || (trashCardIndex == eCards::Remodel && hand.CountOfCard( trashCardIndex ) > 1) )
				{
					CardDefinition const* trashCard = CardDefinition::GetCardDefinitionByType( (eCards)trashCardIndex );
					int trashCardCost = trashCard->GetCardCost();
					for( int acquireCardIndex = 0; acquireCardIndex < eCards::NUM_CARDS; acquireCardIndex++ )
					{
						//Is there enough of the card to acquire and is its cost valid for remodel
						CardDefinition const* acquireCard = CardDefinition::GetCardDefinitionByType( (eCards)acquireCardIndex );
						int acquireCardPileSize = gameState.m_cardPiles[acquireCardIndex].m_pileSize;
						int acquireCardCost = acquireCard->GetCardCost();
						if( acquireCardCost <= trashCardCost + 2 && acquireCardPileSize > 0 )
						{
							numberOfValidMoves++;
						}
					}
				}
			}
		}


	}
	else if( currentPhase == BUY_PHASE )
	{
		int numberOfBuys = playerDeck->m_numberOfBuysAvailable;
		if( numberOfBuys > 0 )
		{
			pileData_t const* piles = &gameState.m_cardPiles[0];
			int currentMoney = playerDeck->GetCurrentMoney();

			for( size_t pileIndex = 0; pileIndex < NUMBEROFPILES; pileIndex++ )
			{
				eCards cardIndex = piles[pileIndex].m_cardIndex;
				CardDefinition const* card = CardDefinition::GetCardDefinitionByType( cardIndex );
				int pileSize = piles[pileIndex].m_pileSize;
				int cardCost = card->GetCardCost();

				if( pileSize > 0 && currentMoney >= cardCost )
				{
					numberOfValidMoves++;
				}
			}
		}
	}

	return numberOfValidMoves;
}

gamestate_t Game::GetGameStateAfterMove( gamestate_t const& currentGameState, inputMove_t const& move )
{
	static double validCheckTime = 0;
	static double endPhaseTime = 0;
	static double buyPhaseTime = 0;
	static double playPhaseTime = 0;
	static double gameOverTime = 0;
	static double moveTime = 0;

	double gameOverStart = GetCurrentTimeSeconds();
	gamestate_t newGameState = currentGameState;
	if( IsGameOverForGameState( currentGameState ) != GAMENOTOVER )
	{
		return newGameState;
	}
	double gameOverEnd = GetCurrentTimeSeconds();
	gameOverTime += gameOverEnd - gameOverStart;
	
	double validCheckStart = GetCurrentTimeSeconds();
	if( IsMoveValidForGameState( move, newGameState ) )
	{
		double validCheckEnd = GetCurrentTimeSeconds();
		validCheckTime += validCheckEnd - validCheckStart;

		PlayerBoard* playerDeck = nullptr;
		int nextPlayersTurn = 0;
		if( move.m_whoseMoveIsIt == PLAYER_1 )
		{
			playerDeck = &newGameState.m_playerBoards[0];
			nextPlayersTurn = PLAYER_2;
		}
		else
		{
			playerDeck = &newGameState.m_playerBoards[1];
			nextPlayersTurn = PLAYER_1;
		}

		double buyPhaseStart = GetCurrentTimeSeconds();
		double playPhaseStart = GetCurrentTimeSeconds();
		double endPhaseStart = GetCurrentTimeSeconds();
		if( move.m_moveType == PLAY_CARD )
		{
			if( playerDeck->m_numberOfActionsAvailable > 0 )
			{
				playerDeck->m_numberOfActionsAvailable--;
				//int handIndex = move.m_cardIndex;
				playerDeck->PlayCard( move, &newGameState );
			}

			double playPhaseEnd = GetCurrentTimeSeconds();
			playPhaseTime += playPhaseEnd - playPhaseStart;
			moveTime += playPhaseEnd - playPhaseStart;
			//return newGameState;
		}
		else if( move.m_moveType == BUY_MOVE )
		{
			int pileIndex = move.m_cardIndex;
			pileData_t& pileData = newGameState.m_cardPiles[pileIndex];
			pileData.m_pileSize -= 1;
			eCards cardIndex = pileData.m_cardIndex;
			CardDefinition const* card = CardDefinition::GetCardDefinitionByType( cardIndex );
			int cardCost = card->GetCardCost();
			//Decrement coins
			//Put card in discard pile
			playerDeck->DecrementCoins( cardCost );
			playerDeck->AddCardToDiscardPile( pileIndex );
			playerDeck->m_numberOfBuysAvailable--;

			double buyPhaseEnd = GetCurrentTimeSeconds();
			buyPhaseTime += buyPhaseEnd - buyPhaseStart;
			moveTime += buyPhaseEnd - buyPhaseStart;
		}
		else if( move.m_moveType == END_PHASE )
		{
			if( newGameState.m_currentPhase == ACTION_PHASE )
			{
				//PLAY ALL TREASURE CARDS
				playerDeck->PlayTreasureCards();
				newGameState.m_currentPhase = BUY_PHASE;
			}
			else if( newGameState.m_currentPhase == BUY_PHASE )
			{
				//DISCARD ALL CARDS, DRAW NEW HAND, AND PASS TURN
				playerDeck->DiscardHand();
				playerDeck->DiscardPlayArea();
				playerDeck->Draw5();

				newGameState.m_currentPhase = ACTION_PHASE;
				newGameState.m_whoseMoveIsIt = nextPlayersTurn;
				playerDeck->m_numberOfActionsAvailable = 1;
				playerDeck->m_numberOfBuysAvailable = 1;
				playerDeck->m_currentCoins = 0;
			}

			double endPhaseEnd = GetCurrentTimeSeconds();
			endPhaseTime += endPhaseEnd - endPhaseStart;
			moveTime += endPhaseEnd - endPhaseStart;
		}
		else
		{
			ERROR_AND_DIE( "INVALID PHASE" );
		}

	}

	return newGameState;
}

inputMove_t Game::GetBestMoveUsingAIStrategy( AIStrategy aiStrategy )
{
	switch( aiStrategy )
	{
	case AIStrategy::RANDOM: return GetRandomMoveAtGameState( *m_currentGameState );
		break;
	case AIStrategy::BIGMONEY: return GetMoveUsingBigMoney( *m_currentGameState );
		break;
	case AIStrategy::SINGLEWITCH: return GetMoveUsingSingleWitch( *m_currentGameState );
		break;
	case AIStrategy::DOUBLEWITCH: return GetMoveUsingDoubleWitch( *m_currentGameState );
		break;
	case AIStrategy::SARASUA1: return GetMoveUsingSarasua1( *m_currentGameState );
		break;
	case AIStrategy::MCTS: return m_mcts->GetBestMove();
		break;
	default: return inputMove_t();
		break;
	}
}

inputMove_t Game::GetRandomMoveAtGameState( gamestate_t const& currentGameState )
{
	std::vector<inputMove_t> validMoves = GetValidMovesAtGameState( currentGameState );

	int randIndex = m_rand.RollRandomIntInRange( 0, (int)validMoves.size() - 1 );
	inputMove_t randMove = validMoves[randIndex];
	return randMove;

}

inputMove_t Game::GetMoveUsingBigMoney( gamestate_t const& currentGameState )
{
	inputMove_t newMove;
	if( IsGameOverForGameState( currentGameState ) != GAMENOTOVER )
	{
		return newMove;
	}

	int whoseMove = currentGameState.m_whoseMoveIsIt;
	newMove.m_whoseMoveIsIt = whoseMove;

	if( currentGameState.m_currentPhase == ACTION_PHASE )
	{
		std::vector<inputMove_t> validMoves = GetValidMovesAtGameState( currentGameState );
		if( validMoves.size() == 1 )
		{
			return validMoves[0];
		}
		else
		{
			for( size_t moveIndex = 0; moveIndex < validMoves.size(); moveIndex++ )
			{
				if( validMoves[moveIndex].m_moveType != END_PHASE )
				{
					return validMoves[moveIndex];
				}
			}
			
			newMove.m_moveType = END_PHASE;
			return newMove;
		}

	}
	else
	{

		PlayerBoard const* playerDeck = &currentGameState.m_playerBoards[0];
		if( currentGameState.m_whoseMoveIsIt == PLAYER_2 )
		{
			playerDeck = &currentGameState.m_playerBoards[1];
		}

		int currentMoney = playerDeck->GetCurrentMoney();
		newMove.m_moveType = BUY_MOVE;
		if( playerDeck->m_numberOfBuysAvailable == 0 )
		{
			newMove.m_moveType = END_PHASE;
		}
		else if( currentMoney >= 8 )
		{
			newMove.m_cardIndex = (int)PROVINCE;
		}
		else if( currentMoney >= 6 )
		{
			newMove.m_cardIndex = (int)GOLD;
		}
		else if( currentMoney >= 3 )
		{
			newMove.m_cardIndex = (int)SILVER;
		}
		else
		{
			newMove.m_moveType = END_PHASE;
		}

		return newMove;
	}

}

inputMove_t Game::GetMoveUsingSingleWitch( gamestate_t const& currentGameState )
{
	inputMove_t newMove;
	if( IsGameOverForGameState( currentGameState ) != GAMENOTOVER )
	{
		return newMove;
	}

	int whoseMove = currentGameState.m_whoseMoveIsIt;
	newMove.m_whoseMoveIsIt = whoseMove;

	if( currentGameState.m_currentPhase == ACTION_PHASE )
	{
		std::vector<inputMove_t> validMoves = GetValidMovesAtGameState( currentGameState );
		if( validMoves.size() == 1 )
		{
			return validMoves[0];
		}
		else
		{
			for( size_t moveIndex = 0; moveIndex < validMoves.size(); moveIndex++ )
			{
				if( validMoves[moveIndex].m_moveType != END_PHASE )
				{
					return validMoves[moveIndex];
				}
			}

			newMove.m_moveType = END_PHASE;
			return newMove;
		}

	}
	else
	{

		PlayerBoard const* playerDeck = &currentGameState.m_playerBoards[0];
		if( currentGameState.m_whoseMoveIsIt == PLAYER_2 )
		{
			playerDeck = &currentGameState.m_playerBoards[1];
		}

		int currentMoney = playerDeck->GetCurrentMoney();
		newMove.m_moveType = BUY_MOVE;
		if( playerDeck->m_numberOfBuysAvailable == 0 )
		{
			newMove.m_moveType = END_PHASE;
		}
		else if( !playerDeck->HasCard( Witch ) && currentMoney >= 5 )
		{
			newMove.m_cardIndex = (int)Witch;
		}
		else if( currentMoney >= 8 )
		{
			newMove.m_cardIndex = (int)PROVINCE;
		}
		else if( currentMoney >= 6 )
		{
			newMove.m_cardIndex = (int)GOLD;
		}
		else if( currentMoney >= 3 )
		{
			newMove.m_cardIndex = (int)SILVER;
		}
		else
		{
			newMove.m_moveType = END_PHASE;
		}

		return newMove;
	}

}

inputMove_t Game::GetMoveUsingDoubleWitch( gamestate_t const& currentGameState )
{
	inputMove_t newMove;
	if( IsGameOverForGameState( currentGameState ) != GAMENOTOVER )
	{
		return newMove;
	}

	int whoseMove = currentGameState.m_whoseMoveIsIt;
	newMove.m_whoseMoveIsIt = whoseMove;

	if( currentGameState.m_currentPhase == ACTION_PHASE )
	{
		std::vector<inputMove_t> validMoves = GetValidMovesAtGameState( currentGameState );
		if( validMoves.size() == 1 )
		{
			return validMoves[0];
		}
		else
		{
			for( size_t moveIndex = 0; moveIndex < validMoves.size(); moveIndex++ )
			{
				if( validMoves[moveIndex].m_moveType != END_PHASE )
				{
					return validMoves[moveIndex];
				}
			}

			newMove.m_moveType = END_PHASE;
			return newMove;
		}

	}
	else
	{

		PlayerBoard const* playerDeck = &currentGameState.m_playerBoards[0];
		if( currentGameState.m_whoseMoveIsIt == PLAYER_2 )
		{
			playerDeck = &currentGameState.m_playerBoards[1];
		}

		int currentMoney = playerDeck->GetCurrentMoney();
		int witchCount = playerDeck->GetCountOfCard( eCards::Witch );
		newMove.m_moveType = BUY_MOVE;
		if( playerDeck->m_numberOfBuysAvailable == 0 )
		{
			newMove.m_moveType = END_PHASE;
		}
		else if( witchCount < 2 && currentMoney >= 5 )
		{
			newMove.m_cardIndex = (int)Witch;
		}
		else if( currentMoney >= 8 )
		{
			newMove.m_cardIndex = (int)PROVINCE;
		}
		else if( currentMoney >= 6 )
		{
			newMove.m_cardIndex = (int)GOLD;
		}
		else if( currentMoney >= 3 )
		{
			newMove.m_cardIndex = (int)SILVER;
		}
		else
		{
			newMove.m_moveType = END_PHASE;
		}

		return newMove;
	}
}

inputMove_t Game::GetMoveUsingSarasua1( gamestate_t const& currentGameState )
{
	inputMove_t newMove;
	if( IsGameOverForGameState( currentGameState ) != GAMENOTOVER )
	{
		return newMove;
	}
	inputMove_t randomMove = GetRandomMoveAtGameState( currentGameState );

	int whoseMove = currentGameState.m_whoseMoveIsIt;
	newMove.m_whoseMoveIsIt = whoseMove;

	PlayerBoard const& playerBoard = currentGameState.m_playerBoards[whoseMove];


	if( currentGameState.m_currentPhase == ACTION_PHASE )
	{
		if( playerBoard.m_numberOfActionsAvailable == 0 )
		{
			newMove.m_moveType = END_PHASE;
		}
		else
		{
			newMove.m_moveType = PLAY_CARD;
			if( playerBoard.CanPlayCard( inputMove_t( PLAY_CARD, whoseMove, Village ), &currentGameState ) )
			{
				newMove.m_cardIndex = Village;
			}
			else if( playerBoard.CanPlayCard( inputMove_t( PLAY_CARD, whoseMove, Festival ), &currentGameState ) )
			{
				newMove.m_cardIndex = Festival;
			}
			else if( playerBoard.CanPlayCard( inputMove_t( PLAY_CARD, whoseMove, Laboratory ), &currentGameState ) )
			{
				newMove.m_cardIndex = Laboratory;
			}
			else if( playerBoard.CanPlayCard( inputMove_t( PLAY_CARD, whoseMove, Market ), &currentGameState ) )
			{
				newMove.m_cardIndex = Market;
			}
			else if( playerBoard.CanPlayCard( inputMove_t( PLAY_CARD, whoseMove, Witch ), &currentGameState ) )
			{
				newMove.m_cardIndex = Witch;
			}
			else
			{
				return randomMove;
				//newMove = GetRandomMoveAtGameState( currentGameState );
			}
		}

	}
	else if( currentGameState.m_currentPhase == BUY_PHASE )
	{
		newMove.m_moveType = BUY_MOVE;
		int currentMoney = playerBoard.GetCurrentMoney();
		int buys = playerBoard.m_numberOfBuysAvailable;

		if( buys > 0 )
		{
			int witchCount = playerBoard.GetCountOfCard( eCards::Witch );
			int laboratoryCount = playerBoard.GetCountOfCard( eCards::Laboratory );
			int villageCount = playerBoard.GetCountOfCard( eCards::Village );
			//int goldCount = playerBoard.GetCountOfCard( GOLD );
			int silverCount = playerBoard.GetCountOfCard( eCards::SILVER );
			int provincesLeft = currentGameState.m_cardPiles[eCards::PROVINCE].m_pileSize;

			if( currentMoney >= 5 && (witchCount < 2 || laboratoryCount < 1) )
			{
				if( witchCount < 2 )
				{
					newMove.m_cardIndex = eCards::Witch;
				}
				else if( laboratoryCount < 1 )
				{
					newMove.m_cardIndex = eCards::Laboratory;
				}
			}
			else if( currentMoney >= 8 )
			{
				newMove.m_cardIndex = eCards::PROVINCE;
			}
			else if( currentMoney >= 6 )
			{
				if( provincesLeft < 3 )
				{
					newMove.m_cardIndex = eCards::DUCHY;
				}
				else
				{
					newMove.m_cardIndex = eCards::GOLD;
				}
			}
			else if( currentMoney >= 5 )
			{
				if( provincesLeft <= 5 )
				{
					newMove.m_cardIndex = eCards::DUCHY;
				}
				else if( silverCount < 3 )
				{
					newMove.m_cardIndex = eCards::SILVER;
				}
				else if( laboratoryCount < 2 )
				{
					newMove.m_cardIndex = eCards::Laboratory;
				}
				else if( villageCount < 1 )
				{
					newMove.m_cardIndex = eCards::Village;
				}
				else
				{
					newMove.m_cardIndex = eCards::SILVER;
				}
			}
			else if( currentMoney >= 3 )
			{
				if( provincesLeft <= 2 )
				{
					newMove.m_cardIndex = eCards::ESTATE;
				}
				else if( silverCount < 3 )
				{
					newMove.m_cardIndex = eCards::SILVER;
				}
				else if( villageCount < 1 )
				{
					newMove.m_cardIndex = eCards::Village;
				}
				else if( silverCount < 7 )
				{
					newMove.m_cardIndex = eCards::SILVER;
				}
				else
				{
					newMove.m_moveType = END_PHASE;
				}
			}
			else
			{
				newMove.m_moveType = END_PHASE;
			}

		}
		else
		{
			newMove.m_moveType = END_PHASE;
		}
	}

	if( IsMoveValidForGameState( newMove, currentGameState ) )
	{
		return newMove;
	}
	else
	{
		return randomMove;
	}
}

inputMove_t Game::GetMoveUsingSarasua2( gamestate_t const& currentGameState )
{
	inputMove_t newMove;
	if( IsGameOverForGameState( currentGameState ) != GAMENOTOVER )
	{
		return newMove;
	}

	int whoseMove = currentGameState.m_whoseMoveIsIt;
	newMove.m_whoseMoveIsIt = whoseMove;
	PlayerBoard const& playerBoard = currentGameState.m_playerBoards[whoseMove];

	if( currentGameState.m_currentPhase == ACTION_PHASE )
	{
		std::vector<inputMove_t> validMoves = GetValidMovesAtGameState( currentGameState );
		if( validMoves.size() == 1 )
		{
			return validMoves[0];
		}
		else
		{
			newMove.m_moveType = PLAY_CARD;
			if( playerBoard.CanPlayCard( inputMove_t( PLAY_CARD, whoseMove, Village ), &currentGameState ) )
			{
				newMove.m_cardIndex = Village;
			}
			else if( playerBoard.CanPlayCard( inputMove_t( PLAY_CARD, whoseMove, Festival ), &currentGameState ) )
			{
				newMove.m_cardIndex = Festival;
			}
			else if( playerBoard.CanPlayCard( inputMove_t( PLAY_CARD, whoseMove, Laboratory ), &currentGameState ) )
			{
				newMove.m_cardIndex = Laboratory;
			}
			else if( playerBoard.CanPlayCard( inputMove_t( PLAY_CARD, whoseMove, Market ), &currentGameState ) )
			{
				newMove.m_cardIndex = Market;
			}

			inputMove_t remodelMove;
			bool canGetRemodelMove = TryGetBestRemodelMove( validMoves, remodelMove );
			if( canGetRemodelMove )
			{
				return remodelMove;
			}

			for( size_t moveIndex = 0; moveIndex < validMoves.size(); moveIndex++ )
			{
				if( validMoves[moveIndex].m_moveType != END_PHASE )
				{
					return validMoves[moveIndex];
				}
			}

			newMove.m_moveType = END_PHASE;
			return newMove;
		}

	}
	else
	{

		PlayerBoard const* playerDeck = &currentGameState.m_playerBoards[0];
		if( currentGameState.m_whoseMoveIsIt == PLAYER_2 )
		{
			playerDeck = &currentGameState.m_playerBoards[1];
		}

		int currentMoney = playerDeck->GetCurrentMoney();
		int witchCount = playerDeck->GetCountOfCard( eCards::Witch );
		newMove.m_moveType = BUY_MOVE;
		if( playerDeck->m_numberOfBuysAvailable == 0 )
		{
			newMove.m_moveType = END_PHASE;
		}
		else if( witchCount < 2 && currentMoney >= 5 )
		{
			newMove.m_cardIndex = (int)Witch;
		}
		else if( currentMoney >= 8 )
		{
			newMove.m_cardIndex = (int)PROVINCE;
		}
		else if( currentMoney >= 6 )
		{
			newMove.m_cardIndex = (int)GOLD;
		}
		else if( currentMoney >= 3 )
		{
			newMove.m_cardIndex = (int)SILVER;
		}
		else
		{
			newMove.m_moveType = END_PHASE;
		}

		return newMove;
	}
}

bool Game::TryGetBestRemodelMove( std::vector<inputMove_t> const& validMoves, inputMove_t& inputMoveToUpdate )
{
	std::vector<inputMove_t> remodelMoves;
	for( inputMove_t const& validMove : validMoves )
	{
		if( validMove.m_cardIndex == eCards::Remodel )
		{
			remodelMoves.push_back( validMove );
		}
	}

	if( remodelMoves.size() == 0 )
	{
		return false;
	}

	bool canDoCurseToEstate = false;
	inputMove_t curseToEstate;
	bool canDoEstateToSilver = false;
	inputMove_t estateToSilver;
	bool canDoRemodelToGold = false;
	inputMove_t remodelToGold;

	std::vector<inputMove_t> goldAcquireMoves;
	std::vector<inputMove_t> fiveCostAcquireMoves;
	std::vector<inputMove_t> fourCostAcquireMoves;
	std::vector<inputMove_t> threeCostAcquireMoves;

	for( inputMove_t const& remodelMove : remodelMoves )
	{
		eCards cardToTrash = (eCards)remodelMove.m_parameterCardIndex1;
		eCards cardToAcquire = (eCards)remodelMove.m_parameterCardIndex2;

		CardDefinition const* cardToAcquireDef = CardDefinition::GetCardDefinitionByType( cardToAcquire );
		int costOfAcquireCard = cardToAcquireDef->GetCardCost();
		
		if( cardToTrash == eCards::GOLD && cardToAcquire == eCards::PROVINCE )
		{
			inputMoveToUpdate = remodelMove;
			return true;
		}

		if( cardToTrash == eCards::CURSE && cardToAcquire == eCards::ESTATE )
		{
			curseToEstate = remodelMove;
			canDoCurseToEstate = true;
			//return true;
		}		
		
		if( cardToTrash == eCards::ESTATE && cardToAcquire == eCards::SILVER )
		{
			estateToSilver = remodelMove;
			canDoEstateToSilver = true;
			//return true;
		}

		if( cardToTrash == eCards::Remodel && cardToAcquire == eCards::GOLD )
		{
			remodelToGold = remodelMove;
			canDoRemodelToGold = true;
			//return true;
		}

		if( cardToAcquire == eCards::GOLD )
		{
			goldAcquireMoves.push_back( remodelMove );
		}
		else if( costOfAcquireCard >= 5 )
		{
			fiveCostAcquireMoves.push_back( remodelMove );
		}
		else if( costOfAcquireCard == 4 )
		{
			fourCostAcquireMoves.push_back( remodelMove );
		}
		else if( costOfAcquireCard == 3 )
		{
			threeCostAcquireMoves.push_back( remodelMove );
		}
	}

	if( canDoCurseToEstate )
	{
		inputMoveToUpdate = curseToEstate;
	}
	if( canDoRemodelToGold )
	{
		inputMoveToUpdate = remodelToGold;
	}
	if( canDoEstateToSilver )
	{
		inputMoveToUpdate = estateToSilver;
	}

	//Get random gold move
	size_t goldAcquireCount = goldAcquireMoves.size();
	if( goldAcquireCount > 0 )
	{
		int randGoldMove = m_rand.RollRandomIntInRange( 0, (int)goldAcquireCount - 1 );
		inputMoveToUpdate = goldAcquireMoves[randGoldMove];
		return true;
	}
	//Get random 5 cost move
	size_t fiveAcquireCount = fiveCostAcquireMoves.size();
	if( fiveAcquireCount > 0 )
	{
		int randFiveMove = m_rand.RollRandomIntInRange( 0, (int)fiveAcquireCount - 1 );
		inputMoveToUpdate = fiveCostAcquireMoves[randFiveMove];
		return true;
	}
	//Get random 4 cost move
	size_t fourAcquireCount = fourCostAcquireMoves.size();
	if( fourAcquireCount > 0 )
	{
		int randfourMove = m_rand.RollRandomIntInRange( 0, (int)fourAcquireCount - 1 );
		inputMoveToUpdate = fourCostAcquireMoves[randfourMove];
		return true;
	}
	//Get random 3 cost move
	size_t threeAcquireCount = threeCostAcquireMoves.size();
	if( threeAcquireCount > 0 )
	{
		int randThreeMove = m_rand.RollRandomIntInRange( 0, (int)threeAcquireCount - 1 );
		inputMoveToUpdate = threeCostAcquireMoves[randThreeMove];
		return true;
	}

	//Couldn't find a good move
	return false;
}

inputMove_t Game::GetMoveUsingHighestVP( gamestate_t const& currentGameState )
{
	int whoseMove = currentGameState.m_whoseMoveIsIt;
	PlayerBoard const& currentPlayerBoard = currentGameState.m_playerBoards[whoseMove];

	eGamePhase currentPhase = currentGameState.m_currentPhase;

	if( currentPhase == eGamePhase::ACTION_PHASE )
	{
		return GetRandomMoveAtGameState( currentGameState );
	}
	else
	{
		int currentMoney = currentPlayerBoard.GetCurrentMoney();
		int currentBuys = currentPlayerBoard.m_numberOfBuysAvailable;

		inputMove_t move;
		move.m_whoseMoveIsIt = whoseMove;

		if( currentBuys > 0 )
		{
			move.m_moveType = BUY_MOVE;
			if( currentMoney >= 8 )
			{
				move.m_cardIndex = PROVINCE;
			}
			else if( currentMoney >= 5 )
			{
				move.m_cardIndex = DUCHY;
			}
			else if( currentMoney >= 2 )
			{
				move.m_cardIndex = ESTATE;
			}
			else
			{
				move.m_moveType = END_PHASE;
			}
		}
		else
		{
			move.m_moveType = END_PHASE;
		}

		return move;
	}
}

gamestate_t Game::GetRandomInitialGameState()
{
	PlayerBoard starterDeck = PlayerBoard();
	std::vector<CardData_t> starterCards;
	starterCards.reserve( 10 );
	CardDefinition const* copper = CardDefinition::GetCardDefinitionByType( eCards::COPPER );
//	CardDefinition const* silver = CardDefinition::GetCardDefinitionByType( eCards::SILVER );
// 	CardDefinition const* gold = CardDefinition::GetCardDefinitionByType( eCards::GOLD );
 	CardDefinition const* estate = CardDefinition::GetCardDefinitionByType( eCards::ESTATE );
// 	CardDefinition const* duchy = CardDefinition::GetCardDefinitionByType( eCards::DUCHY );
// 	CardDefinition const* province = CardDefinition::GetCardDefinitionByType( eCards::PROVINCE );
// 	CardDefinition const* curse = CardDefinition::GetCardDefinitionByType( eCards::CURSE );
// 
// 	CardDefinition const* village = CardDefinition::GetCardDefinitionByType( eCards::Village );
// 	CardDefinition const* smithy = CardDefinition::GetCardDefinitionByType( eCards::Smithy );
// 	CardDefinition const* festival = CardDefinition::GetCardDefinitionByType( eCards::Festival );
// 	CardDefinition const* laboratory = CardDefinition::GetCardDefinitionByType( eCards::Laboratory );
// 	CardDefinition const* market = CardDefinition::GetCardDefinitionByType( eCards::Market );
// 	CardDefinition const* councilRoom = CardDefinition::GetCardDefinitionByType( eCards::CouncilRoom );
// 	CardDefinition const* witch = CardDefinition::GetCardDefinitionByType( eCards::Witch );

	CardData_t copperData( copper, (int)COPPER );
	CardData_t estateData( estate, (int)ESTATE );

	//7 copper
	starterCards.push_back( copperData );
	starterCards.push_back( copperData );
	starterCards.push_back( copperData );
	starterCards.push_back( copperData );
	starterCards.push_back( copperData );
	starterCards.push_back( copperData );
	starterCards.push_back( copperData );
	//3 estate
	starterCards.push_back( estateData );
	starterCards.push_back( estateData );
	starterCards.push_back( estateData );
	starterDeck.InitializeDeck( starterCards );

	gamestate_t newGameState;
	newGameState.m_playerBoards[0] = starterDeck;
	newGameState.m_playerBoards[1] = starterDeck;

	newGameState.m_playerBoards[0].ShuffleDeck();
	newGameState.m_playerBoards[1].ShuffleDeck();

	newGameState.m_playerBoards[0].Draw5();
	newGameState.m_playerBoards[1].Draw5();

	newGameState.m_currentPhase = ACTION_PHASE;

	pileData_t* cardPiles = &newGameState.m_cardPiles[0];
	cardPiles[(int)eCards::COPPER].m_cardIndex = eCards::COPPER;
	cardPiles[(int)eCards::COPPER].m_pileSize = MONEYPILESIZE;
	cardPiles[(int)eCards::SILVER].m_cardIndex = eCards::SILVER;
	cardPiles[(int)eCards::SILVER].m_pileSize = MONEYPILESIZE;
	cardPiles[(int)eCards::GOLD].m_cardIndex = eCards::GOLD;
	cardPiles[(int)eCards::GOLD].m_pileSize = MONEYPILESIZE;
	cardPiles[(int)eCards::ESTATE].m_cardIndex = eCards::ESTATE;
	cardPiles[(int)eCards::ESTATE].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::DUCHY].m_cardIndex = eCards::DUCHY;
	cardPiles[(int)eCards::DUCHY].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::PROVINCE].m_cardIndex = eCards::PROVINCE;
	cardPiles[(int)eCards::PROVINCE].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::CURSE].m_cardIndex = eCards::CURSE;
	cardPiles[(int)eCards::CURSE].m_pileSize = CURSEPILESIZE;
	cardPiles[(int)eCards::Village].m_cardIndex = eCards::Village;
	cardPiles[(int)eCards::Village].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Smithy].m_cardIndex = eCards::Smithy;
	cardPiles[(int)eCards::Smithy].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Festival].m_cardIndex = eCards::Festival;
	cardPiles[(int)eCards::Festival].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Laboratory].m_cardIndex = eCards::Laboratory;
	cardPiles[(int)eCards::Laboratory].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Market].m_cardIndex = eCards::Market;
	cardPiles[(int)eCards::Market].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::CouncilRoom].m_cardIndex = eCards::CouncilRoom;
	cardPiles[(int)eCards::CouncilRoom].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Witch].m_cardIndex = eCards::Witch;
	cardPiles[(int)eCards::Witch].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Remodel].m_cardIndex = eCards::Remodel;
	cardPiles[(int)eCards::Remodel].m_pileSize = ACTIONPILESIZE;


	newGameState.m_whoseMoveIsIt = PLAYER_1;
	newGameState.m_isFirstMove = false;

	return newGameState;
}

void Game::RandomizeUnknownInfoForGameState( gamestate_t& currentGameState )
{
	int whoseMove = currentGameState.m_whoseMoveIsIt;
	int opponent = -1;

	if( whoseMove == PLAYER_1 )
	{
		opponent = PLAYER_2;
	}
	else
	{
		opponent = PLAYER_1;
	}

	currentGameState.m_playerBoards[whoseMove].ShuffleDeck();
	currentGameState.m_playerBoards[opponent].RandomizeHandAndDeck();
}

int Game::GetCurrentPlayersScore( gamestate_t const& currentGameState )
{
	int whoseMove = currentGameState.m_whoseMoveIsIt;

	PlayerBoard const& playerBoard = currentGameState.m_playerBoards[whoseMove];
	int playerScore = playerBoard.GetCurrentVictoryPoints();
	return playerScore;
}

int Game::GetOpponentsScore( gamestate_t const& currentGameState )
{
	int whoseMove = currentGameState.m_whoseMoveIsIt;
	int opponentsMove = -1;
	if( whoseMove == PLAYER_1 )
	{
		opponentsMove = PLAYER_2;
	}
	else
	{
		opponentsMove = PLAYER_1;
	}

	PlayerBoard const& playerBoard = currentGameState.m_playerBoards[opponentsMove];
	int playerScore = playerBoard.GetCurrentVictoryPoints();
	return playerScore;
}

std::vector<int> Game::GetCurrentBuyIndexes() const
{
	std::vector<int> buyIndexes;
	
	std::vector<inputMove_t> validMoves = GetValidMovesAtGameState( *m_currentGameState );
	for( size_t moveIndex = 0; moveIndex < validMoves.size(); moveIndex++ )
	{
		inputMove_t const& move = validMoves[moveIndex];
		if( move.m_moveType == BUY_MOVE )
		{
			buyIndexes.push_back( move.m_cardIndex );
		}
	}

	return buyIndexes;
}

void Game::AppendGameStateToFile( gamestate_t const& gameState, std::string const& filePath )
{
	std::vector<byte> buffer;
	size_t startIndex = 0;
	gameState.AppendGameStateToBuffer( buffer, startIndex );

	int errorCode = AppendBufferToFile( filePath, buffer.size(), &buffer[0] );

	if( errorCode == -1 )
	{
		ERROR_AND_DIE( "Failed to write to file" );
	}
}

gamestate_t Game::ParseGameStateFromBuffer( byte*& buffer )
{
	return gamestate_t::ParseGameStateFromBuffer( buffer );
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}