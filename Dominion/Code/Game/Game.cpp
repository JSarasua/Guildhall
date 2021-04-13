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
#include "Engine/UI/WidgetIncrementerFloat.hpp"
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

	//RunTestCases();

 	m_mc = new MonteCarloNoTree();
	m_player1MCTS = new MonteCarlo();
	m_player2MCTS = new MonteCarlo();


	m_player1MCTS->SetSimMethod( m_player1MCTSSimMethod );
	m_player1MCTS->SetExplorationParameter( m_player1MCTSExplorationParameter );
	m_player1MCTS->SetEpsilonValueZeroToOne( m_player1MCTSChaosChance );
	m_player1MCTS->SetExpansionStrategy( m_player1ExpansionStrategy );
	m_player1MCTS->SetRolloutMethod( m_player1MCTSRolloutMethod );

	m_player2MCTS->SetSimMethod( m_player2MCTSSimMethod );
	m_player2MCTS->SetExplorationParameter( m_player2MCTSExplorationParameter );
	m_player2MCTS->SetEpsilonValueZeroToOne( m_player2MCTSChaosChance );
	m_player2MCTS->SetExpansionStrategy( m_player2ExpansionStrategy );
	m_player2MCTS->SetRolloutMethod( m_player2MCTSRolloutMethod );

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

	m_player1MCTS->Shutdown();
	m_player2MCTS->Shutdown();
	delete m_player1MCTS;
	delete m_player2MCTS;
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

	//DebugDrawGame();
	

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
	m_AIWidget = new Widget( AITransform, rootWidget );
	//m_AIWidget->SetText( "AI Info" );
	m_AIWidget->SetTextSize( 0.1f );
	m_AIWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );
	//rootWidget->AddChild( m_AIWidget );

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

	AABB2 playerScoreAABB = m_playerScoreWidget->GetLocalAABB2();
	AABB2 player1ScoreAABB = playerScoreAABB.CarveBoxOffTop( 0.333f );
	AABB2 player2ScoreAABB = player1ScoreAABB;
	m_player2ScoreWidget = new Widget( player2ScoreAABB, m_playerScoreWidget );
	m_player1ScoreWidget = new Widget( player1ScoreAABB, m_playerScoreWidget );
	Widget* playerScoreTextWidget = new Widget( player1ScoreAABB, m_playerScoreWidget );

	playerScoreTextWidget->SetIsVisible( true );
	playerScoreTextWidget->SetCanHover( false );
	playerScoreTextWidget->SetTextSize( 0.1f );
	playerScoreTextWidget->SetTexture( m_forestGreenTexture, m_forestGreenTexture, m_forestGreenTexture );
	playerScoreTextWidget->SetText( "Player Scores" );
	m_player1ScoreWidget->SetIsVisible( true );
	m_player2ScoreWidget->SetIsVisible( true );
	m_player1ScoreWidget->SetCanHover( false );
	m_player2ScoreWidget->SetCanHover( false );
	m_player1ScoreWidget->SetTextSize( 0.1f );
	m_player2ScoreWidget->SetTextSize( 0.1f );
	m_player1ScoreWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );
	m_player2ScoreWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );
	m_player1ScoreWidget->SetText( "Player 1: 3 VP" );
	m_player2ScoreWidget->SetText( "Player 2: 3 VP" );



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
	AABB2 AIBounds = m_AIWidget->GetLocalAABB2();
	AABB2 leftBounds = AIBounds.GetBoxAtLeft( 0.4f );
	AABB2 rightBounds = AIBounds.GetBoxAtRight( 0.55f );
	AABB2 farRightBounds = AIBounds.GetBoxAtRight( 0.1f );
	farRightBounds.ScaleDimensionsUniform( 0.9f );
	std::vector<AABB2> AILeftColumnBounds = leftBounds.GetBoxAsColumns( 2 );
	std::vector<AABB2> AIRightColumnBounds = rightBounds.GetBoxAsColumns( 2 );
	AILeftColumnBounds[0].ScaleDimensionsUniform( 0.95f );
	AILeftColumnBounds[1].ScaleDimensionsUniform( 0.95f );
	//AILeftColumnBounds[2].ScaleDimensionsUniform( 0.95f );
	AIRightColumnBounds[0].ScaleDimensionsUniform( 0.95f );
	AIRightColumnBounds[1].ScaleDimensionsUniform( 0.95f );
	std::vector<AABB2> leftLeftRowsBounds = AILeftColumnBounds[0].GetBoxAsRows( 3 );
	std::vector<AABB2> leftRowsBounds = AILeftColumnBounds[1].GetBoxAsRows( 3 );
	//std::vector<AABB2> middleRowsBounds = AILeftColumnBounds[2].GetBoxAsRows( 3 );
	std::vector<AABB2> rightRowsBounds = AIRightColumnBounds[0].GetBoxAsRows( 3 );
	std::vector<AABB2> rightRightRowsBounds = AIRightColumnBounds[1].GetBoxAsRows( 3 );
	std::vector<AABB2> farthestRightRowsBounds = farRightBounds.GetBoxAsRows( 3 );

	m_AIInfoWidget = new Widget( leftLeftRowsBounds[2], m_AIWidget );
	m_AIInfoWidget->SetTextAlignment( ALIGN_CENTER_LEFT );
	m_AIInfoWidget->SetText( "AI Controls: " );
	m_AIInfoWidget->SetTextSize( 0.1f );
	m_AIInfoWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );

	m_toggleAutoPlayWidget = new Widget( leftLeftRowsBounds[1], m_AIWidget );
	//m_ToggleAutoPlayWidget->SetTextAlignment( ALIGN_CENTER_LEFT );
	m_toggleAutoPlayWidget->SetText( "Auto Play: OFF" );
	m_toggleAutoPlayWidget->SetTextSize( 0.1f );
	m_toggleAutoPlayWidget->SetTexture( m_forestGreenTexture, m_darkForestGreenTexture, m_forestGreenTexture );
	Delegate<EventArgs const&>& toggleAutoPlayDelegate = m_toggleAutoPlayWidget->m_releaseDelegate;
	toggleAutoPlayDelegate.SubscribeMethod( this, &Game::ToggleAutoPlay );

	m_restartGameWidget = new Widget( leftLeftRowsBounds[0], m_AIWidget );
	m_restartGameWidget->SetText( "Restart Game" );
	m_restartGameWidget->SetTextSize( 0.1f );
	m_restartGameWidget->SetTexture( m_forestGreenTexture, m_darkForestGreenTexture, m_forestGreenTexture );
	Delegate<EventArgs const&>& restartGameDelegate = m_restartGameWidget->m_releaseDelegate;
	restartGameDelegate.SubscribeMethod( this, &Game::RestartGame );



	m_playAIMoveWidget = new Widget( leftRowsBounds[0], m_AIWidget );
	//m_playAIMoveWidget->SetTextAlignment( ALIGN_CENTER_LEFT );
	m_playAIMoveWidget->SetText( "Play Move" );
	m_playAIMoveWidget->SetTextSize( 0.1f );
	m_playAIMoveWidget->SetTexture( m_forestGreenTexture, m_darkForestGreenTexture, m_forestGreenTexture );
	Delegate<EventArgs const&>& playAIMoveDelegate = m_playAIMoveWidget->m_releaseDelegate;
	playAIMoveDelegate.SubscribeMethod( this, &Game::PlayCurrentAIMove );


	m_currentAIBestMoveWidget = new Widget( leftRowsBounds[1], m_AIWidget );
	m_currentAIBestMoveWidget->SetTextAlignment( ALIGN_CENTER_LEFT );
	m_currentAIBestMoveWidget->SetText( "Best move: Play x card" );
	m_currentAIBestMoveWidget->SetTextSize( 0.1f );
	m_currentAIBestMoveWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );

	m_currentAIWidget = new Widget( leftRowsBounds[2], m_AIWidget );
	m_currentAIWidget->SetTextAlignment( ALIGN_CENTER_LEFT );
	std::string aiStrategy = "MCTS";
	m_currentAIWidget->SetText( Stringf("Current AI: %s", "MCTS") );
	m_currentAIWidget->SetTextSize( 0.1f );
	m_currentAIWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );

	m_player1MCTSSimulationsWidget = new Widget( rightRowsBounds[2], m_AIWidget );
	m_player1MCTSSimulationsWidget->SetTextAlignment( ALIGN_CENTER_LEFT );
	m_player1MCTSSimulationsWidget->SetText( "Player 1 MCTS Sims: 123" );
	m_player1MCTSSimulationsWidget->SetTextSize( 0.1f );
	m_player1MCTSSimulationsWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );

	m_player2MCTSSimulationsWidget = new Widget( rightRowsBounds[1], m_AIWidget );
	m_player2MCTSSimulationsWidget->SetTextAlignment( ALIGN_CENTER_LEFT );
	m_player2MCTSSimulationsWidget->SetText( "Player 2 MCTS Sims: 123" );
	m_player2MCTSSimulationsWidget->SetTextSize( 0.1f );
	m_player2MCTSSimulationsWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );

	m_player1MCTSTimesVisitedWidget = new Widget( rightRightRowsBounds[2], m_AIWidget );
	m_player1MCTSTimesVisitedWidget->SetTextAlignment( ALIGN_CENTER_LEFT );
	m_player1MCTSTimesVisitedWidget->SetText( "Node Visits: 123" );
	m_player1MCTSTimesVisitedWidget->SetTextSize( 0.1f );
	m_player1MCTSTimesVisitedWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );

	m_player2MCTSTimesVisitedWidget = new Widget( rightRightRowsBounds[1], m_AIWidget );
	m_player2MCTSTimesVisitedWidget->SetTextAlignment( ALIGN_CENTER_LEFT );
	m_player2MCTSTimesVisitedWidget->SetText( "Node Visits: 123" );
	m_player2MCTSTimesVisitedWidget->SetTextSize( 0.1f );
	m_player2MCTSTimesVisitedWidget->SetTexture( m_artichokeGreenTexture, m_artichokeGreenTexture, m_artichokeGreenTexture );


	m_AddPlayer1Simulations = new Widget( farthestRightRowsBounds[2], m_AIWidget );
	//m_AddPlayer1Simulations->SetTextAlignment( ALIGN_CENTER_LEFT );
	m_AddPlayer1Simulations->SetText( "Add Sims" );
	m_AddPlayer1Simulations->SetTextSize( 0.1f );
	m_AddPlayer1Simulations->SetTexture( m_forestGreenTexture, m_darkForestGreenTexture, m_forestGreenTexture );
	Delegate<EventArgs const&>& player1AddSimsDelegate = m_AddPlayer1Simulations->m_releaseDelegate;
	player1AddSimsDelegate.SubscribeMethod( this, &Game::AddSimsForPlayer1 );

	m_AddPlayer2Simulations = new Widget( farthestRightRowsBounds[1], m_AIWidget );
	//m_AddPlayer2Simulations->SetTextAlignment( ALIGN_CENTER_LEFT );
	m_AddPlayer2Simulations->SetText( "Add Sims" );
	m_AddPlayer2Simulations->SetTextSize( 0.1f );
	m_AddPlayer2Simulations->SetTexture( m_forestGreenTexture, m_darkForestGreenTexture, m_forestGreenTexture );
	Delegate<EventArgs const&>& player2AddSimsDelegate = m_AddPlayer2Simulations->m_releaseDelegate;
	player2AddSimsDelegate.SubscribeMethod( this, &Game::AddSimsForPlayer2 );

}

void Game::InitializeAILargePanelWidget()
{
	Rgba8 darkdarkGreen = Rgba8::DarkDarkGreen;
	Rgba8 greyGreen = Rgba8::GreyGreen;
	Rgba8 black = Rgba8::BLACK;
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
	aiStrategies.push_back( "MCTS" );
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
	m_player1AITextWidget->SetTexture( darkdarkGreenTexture, darkdarkGreenTexture, darkdarkGreenTexture );
	m_player1ChooseAIWidget->SetTexture( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );
	m_player2AITextWidget->SetTexture( darkdarkGreenTexture, darkdarkGreenTexture, darkdarkGreenTexture );
	m_player2ChooseAIWidget->SetTexture( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );
	
	m_player1ChooseAIWidget->m_valueChangeDelegate.SubscribeMethod( this, &Game::ChangePlayer1Strategy );
	m_player2ChooseAIWidget->m_valueChangeDelegate.SubscribeMethod( this, &Game::ChangePlayer2Strategy );

	m_player1AITextWidget->SetIsVisible( true );
	m_player2AITextWidget->SetIsVisible( true );
	m_player2AITextWidget->SetText( "Player 2" );
	m_player1AITextWidget->SetText( "Player 1" );
	m_player2AITextWidget->SetTextAlignment( Vec2( 0.1f, 0.5f ) );
	m_player1AITextWidget->SetTextAlignment( Vec2( 0.1f, 0.5f ) );
	m_player2AITextWidget->SetTextSize( 0.1f );
	m_player1AITextWidget->SetTextSize( 0.1f );

	std::vector<std::string> rolloutIncrementers;
	rolloutIncrementers.push_back( "Random" );
	rolloutIncrementers.push_back( "Big Money" );
	rolloutIncrementers.push_back( "Single Witch" );
	rolloutIncrementers.push_back( "Double Witch" );
	rolloutIncrementers.push_back( "Sarasua1" );
	rolloutIncrementers.push_back( "Greedy" );
	rolloutIncrementers.push_back( "RandomPLUS" );

	std::vector<std::string> expansionIncrementers;
	expansionIncrementers.push_back( "All Moves" );
	expansionIncrementers.push_back( "Heuristics" );
	std::vector<float> increments;
	increments.push_back( -1.f );
	increments.push_back( 1.f );
	//Player 1 MCTS AI Parameters
	AABB2 middleColumnAIBounds = AIMoreInfoBounds.CarveBoxOffLeft( 0.5f );
	middleColumnAIBounds = middleColumnAIBounds.GetBoxAtTop( 0.5f );
	std::vector<AABB2> middleAIRows = middleColumnAIBounds.GetBoxAsRows( 9 );
	m_player1MCTSAIParametersWidget = new Widget( middleAIRows[8], m_AIMoreInfoWidget );
	m_player1SelectionTextWidget = new Widget( middleAIRows[7], m_AIMoreInfoWidget );
	m_player1UCTScoreChangerWidget = new WidgetIncrementerFloat( increments, "UCT Score:", 0.5f, 0.1f, 50.f, middleAIRows[6], 0.1f, OnPress, m_AIMoreInfoWidget );
	m_player1ExpansionTextWidget = new Widget( middleAIRows[5], m_AIMoreInfoWidget );
	m_player1ExpansionChangerWidget = new WidgetIncrementer( expansionIncrementers, middleAIRows[4], 0.3f, 0.1f, m_AIMoreInfoWidget );
	m_player1SimulationTextWidget = new Widget( middleAIRows[3], m_AIMoreInfoWidget );
	m_player1SimulationChangerWidget = new WidgetIncrementer( rolloutIncrementers, middleAIRows[2], 0.3f, 0.1f, m_AIMoreInfoWidget );
	m_player1UseChaosChanceWidget = new Widget( middleAIRows[1], m_AIMoreInfoWidget );
	m_player1ChaosChanceChangerWidget = new WidgetIncrementerFloat( increments, "Chaos Value:", 0.15f, 0.f, 1.f, middleAIRows[0], 0.1f, OnPress, m_AIMoreInfoWidget );

	m_player1MCTSAIParametersWidget->SetText( "Player 1 MCTS AI Parameters" );
	m_player1MCTSAIParametersWidget->SetTexture( darkdarkGreenTexture, darkdarkGreenTexture, darkdarkGreenTexture );
	m_player1MCTSAIParametersWidget->SetTextSize( 0.1f );
	m_player1MCTSAIParametersWidget->SetIsVisible( true );
	m_player1MCTSAIParametersWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player1SelectionTextWidget->SetText( "Selection" );
	m_player1SelectionTextWidget->SetTexture( darkdarkGreenTexture, darkdarkGreenTexture, darkdarkGreenTexture );
	m_player1SelectionTextWidget->SetTextSize( 0.1f );
	m_player1SelectionTextWidget->SetIsVisible( true );
	m_player1SelectionTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player1UCTScoreChangerWidget->SetIncrementerTextures( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );
	m_player1UCTScoreChangerWidget->m_valueChangeDelegate.SubscribeMethod( this, &Game::ChangePlayer1UCTValue );

	m_player1ExpansionTextWidget->SetText( "Expansion" );
	m_player1ExpansionTextWidget->SetTexture( darkdarkGreenTexture, darkdarkGreenTexture, darkdarkGreenTexture );
	m_player1ExpansionTextWidget->SetTextSize( 0.1f );
	m_player1ExpansionTextWidget->SetIsVisible( true );
	m_player1ExpansionTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player1ExpansionChangerWidget->SetTexture( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );
	m_player1ExpansionChangerWidget->m_valueChangeDelegate.SubscribeMethod( this, &Game::ChangePlayer1ExpansionMethod );

	m_player1SimulationTextWidget->SetText( "Simulation" );
	m_player1SimulationTextWidget->SetTexture( darkdarkGreenTexture, darkdarkGreenTexture, darkdarkGreenTexture );
	m_player1SimulationTextWidget->SetTextSize( 0.1f );
	m_player1SimulationTextWidget->SetIsVisible( true );
	m_player1SimulationTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player1SimulationChangerWidget->SetTexture( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );
	m_player1SimulationChangerWidget->m_valueChangeDelegate.SubscribeMethod( this, &Game::ChangePlayer1RolloutMethod );

	m_player1UseChaosChanceWidget->SetText( "Use Chaos Chance: [x]" );
	m_player1UseChaosChanceWidget->SetTexture( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );
	m_player1UseChaosChanceWidget->SetTextSize( 0.1f );
	m_player1UseChaosChanceWidget->SetIsVisible( true );
	m_player1UseChaosChanceWidget->SetTextAlignment( Vec2( 0.5f, 0.5f ) );
	m_player1UseChaosChanceWidget->m_releaseDelegate.SubscribeMethod( this, &Game::TogglePlayer1UseChaosChance );
	
	m_player1ChaosChanceChangerWidget->SetIncrementerTextures( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );

	//Player 2 MCTS AI Parameters
	AABB2 rightColumnAIBounds = AIMoreInfoBounds;
	rightColumnAIBounds = rightColumnAIBounds.GetBoxAtTop( 0.5f );
	std::vector<AABB2> rightAIRows = rightColumnAIBounds.GetBoxAsRows( 9 );
	m_player2MCTSAIParametersWidget = new Widget( rightAIRows[8], m_AIMoreInfoWidget );
	m_player2SelectionTextWidget = new Widget( rightAIRows[7], m_AIMoreInfoWidget );
	m_player2UCTScoreChangerWidget = new WidgetIncrementerFloat( increments, "UCT Score:", 0.5f, 0.1f, 50.f, rightAIRows[6], 0.1f, OnPress, m_AIMoreInfoWidget );
	m_player2ExpansionTextWidget = new Widget( rightAIRows[5], m_AIMoreInfoWidget );
	m_player2ExpansionChangerWidget = new WidgetIncrementer( expansionIncrementers, rightAIRows[4], 0.3f, 0.1f, m_AIMoreInfoWidget );
	m_player2SimulationTextWidget = new Widget( rightAIRows[3], m_AIMoreInfoWidget );
	m_player2SimulationChangerWidget = new WidgetIncrementer( rolloutIncrementers, rightAIRows[2], 0.3f, 0.1f, m_AIMoreInfoWidget );
	m_player2UseChaosChanceWidget = new Widget( rightAIRows[1], m_AIMoreInfoWidget );
	m_player2ChaosChanceChangerWidget = new WidgetIncrementerFloat( increments, "Chaos Value:", 0.15f, 0.f, 1.f, rightAIRows[0], 0.1f, OnPress, m_AIMoreInfoWidget );

	m_player2MCTSAIParametersWidget->SetText( "Player 2 MCTS AI Parameters" );
	m_player2MCTSAIParametersWidget->SetTexture( darkdarkGreenTexture, darkdarkGreenTexture, darkdarkGreenTexture );
	m_player2MCTSAIParametersWidget->SetTextSize( 0.1f );
	m_player2MCTSAIParametersWidget->SetIsVisible( true );
	m_player2MCTSAIParametersWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player2SelectionTextWidget->SetText( "Selection" );
	m_player2SelectionTextWidget->SetTexture( darkdarkGreenTexture, darkdarkGreenTexture, darkdarkGreenTexture );
	m_player2SelectionTextWidget->SetTextSize( 0.1f );
	m_player2SelectionTextWidget->SetIsVisible( true );
	m_player2SelectionTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player2UCTScoreChangerWidget->SetIncrementerTextures( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );
	m_player2UCTScoreChangerWidget->m_valueChangeDelegate.SubscribeMethod( this, &Game::ChangePlayer2UCTValue );

	m_player2ExpansionTextWidget->SetText( "Expansion" );
	m_player2ExpansionTextWidget->SetTexture( darkdarkGreenTexture, darkdarkGreenTexture, darkdarkGreenTexture );
	m_player2ExpansionTextWidget->SetTextSize( 0.1f );
	m_player2ExpansionTextWidget->SetIsVisible( true );
	m_player2ExpansionTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player2ExpansionChangerWidget->SetTexture( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );
	m_player2ExpansionChangerWidget->m_valueChangeDelegate.SubscribeMethod( this, &Game::ChangePlayer2ExpansionMethod );

	m_player2SimulationTextWidget->SetText( "Simulation" );
	m_player2SimulationTextWidget->SetTexture( darkdarkGreenTexture, darkdarkGreenTexture, darkdarkGreenTexture );
	m_player2SimulationTextWidget->SetTextSize( 0.1f );
	m_player2SimulationTextWidget->SetIsVisible( true );
	m_player2SimulationTextWidget->SetTextAlignment( ALIGN_CENTER_LEFT );

	m_player2SimulationChangerWidget->SetTexture( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );
	m_player2SimulationChangerWidget->m_valueChangeDelegate.SubscribeMethod( this, &Game::ChangePlayer2RolloutMethod );

	m_player2UseChaosChanceWidget->SetText( "Use Chaos Chance: [x]" );
	m_player2UseChaosChanceWidget->SetTexture( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );
	m_player2UseChaosChanceWidget->SetTextSize( 0.1f );
	m_player2UseChaosChanceWidget->SetIsVisible( true );
	m_player2UseChaosChanceWidget->SetTextAlignment( Vec2( 0.5f, 0.5f ) );
	m_player2UseChaosChanceWidget->m_releaseDelegate.SubscribeMethod( this, &Game::TogglePlayer2UseChaosChance );

	m_player2ChaosChanceChangerWidget->SetIncrementerTextures( darkdarkGreenTexture, m_artichokeGreenTexture, darkdarkGreenTexture );
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

	UpdateScoreWidgets();
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
		Texture const* cardTex = cardDef->GetCardTexture();
		Widget* cardWidget = new Widget( *m_baseCardWidget );
		cardWidget->SetTexture( cardTex, cardTex, cardTex );
		cardWidget->SetCanDrag( false );
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
	UpdateUIText();
}

void Game::UpdateUIText()
{
	UpdateDeckAndDiscardWidgets();
	UpdateAISmallPanelWidget();
	UpdateScoreWidgets();
	UpdateGameStateWidget();
}

void Game::UpdateDeckAndDiscardWidgets()
{
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

void Game::UpdateScoreWidgets()
{
	int player1Score = m_currentGameState->m_playerBoards[PLAYER_1].GetCurrentVictoryPoints();
	int player2Score = m_currentGameState->m_playerBoards[PLAYER_2].GetCurrentVictoryPoints();

	m_player1ScoreWidget->SetText( Stringf( "Player 1: %i VP", player1Score ) );
	m_player2ScoreWidget->SetText( Stringf( "Player 2: %i VP", player2Score ) );
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

	m_toggleAutoPlayWidget->SetText( Stringf( "Auto Play: %s", toggleAutoPlayStr.c_str() ) );


	int whoseMove = m_currentGameState->m_whoseMoveIsIt;
	std::string aiStrategyStr;
	inputMove_t bestMoveForCurrentAI;
	if( whoseMove == PLAYER_1 )
	{
		bestMoveForCurrentAI = GetBestMoveUsingAIStrategy( m_player1Strategy, m_player1MCTS );
		
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
		bestMoveForCurrentAI = GetBestMoveUsingAIStrategy( m_player2Strategy, m_player2MCTS );

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

	int numberOfSimsPlayer1 = m_player1MCTS->GetNumberOfSimulationsRun();
	int numberOfVisitsPlayer1 = m_player1MCTS->GetNumberOfVisitsAtCurrentNode();
	m_player1MCTSSimulationsWidget->SetText( Stringf( "Player 1 MCTS Iterations: %i", numberOfSimsPlayer1 ) );
	m_player1MCTSTimesVisitedWidget->SetText( Stringf( "Node Visits: %i", numberOfVisitsPlayer1 ) );

	int numberOfSimsPlayer2 = m_player2MCTS->GetNumberOfSimulationsRun();
	int numberOfVisitsPlayer2 = m_player2MCTS->GetNumberOfVisitsAtCurrentNode();
	m_player2MCTSSimulationsWidget->SetText( Stringf( "Player 2 MCTS Iterations: %i", numberOfSimsPlayer2 ) );
	m_player2MCTSTimesVisitedWidget->SetText( Stringf( "Node Visits: %i", numberOfVisitsPlayer2 ) );

}

void Game::RunTestCases()
{
	TestResults results = RunAIVsAITest( AIStrategy::BIGMONEY, AIStrategy::RANDOM, 100, true );
	g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Big Money vs Random", results.m_gamesPlayed) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Games played: %i", results.m_gamesPlayed) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Time taken: %.f", results.m_timeToRun ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Big Money Wins: %i", results.m_playerAWins) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Random Wins: %i", results.m_playerBWins) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Ties: %i", results.m_numberOfTies ) );

	results = RunAIVsAITest( AIStrategy::BIGMONEY, AIStrategy::SINGLEWITCH, 100, true );
	g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Big Money vs Single Witch", results.m_gamesPlayed ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Games played: %i", results.m_gamesPlayed ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Time taken: %.f", results.m_timeToRun ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Big Money Wins: %i", results.m_playerAWins ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Single Witch Wins: %i", results.m_playerBWins ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Ties: %i", results.m_numberOfTies ) );

	results = RunAIVsAITest( AIStrategy::BIGMONEY, AIStrategy::DOUBLEWITCH, 100, true );
	g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Big Money vs Double Witch", results.m_gamesPlayed ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Games played: %i", results.m_gamesPlayed ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Time taken: %.f", results.m_timeToRun ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Big Money Wins: %i", results.m_playerAWins ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Double Witch Wins: %i", results.m_playerBWins ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Ties: %i", results.m_numberOfTies ) );


	results = RunAIVsAITest( AIStrategy::SINGLEWITCH, AIStrategy::DOUBLEWITCH, 100, true );
	g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Single Witch vs Double Witch", results.m_gamesPlayed ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Games played: %i", results.m_gamesPlayed ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Time taken: %.f", results.m_timeToRun ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Single Witch Wins: %i", results.m_playerAWins ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Double Witch Wins: %i", results.m_playerBWins ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Ties: %i", results.m_numberOfTies ) );

	MonteCarlo* mcts = new MonteCarlo();
	mcts->Startup();
	

// 	mcts->SetSimMethod( SIMMETHOD::RANDOM );
// 	mcts->SetExplorationParameter( 1.f );
// 	mcts->SetEpsilonValueZeroToOne( 0.f );
// 	mcts->SetExpansionStrategy( EXPANSIONSTRATEGY::ALLMOVES );
// 	mcts->SetRolloutMethod( ROLLOUTMETHOD::HEURISTIC );
// 	mcts->SetIterationCountPerMove( 100000 );
// 	results = RunAIVsMCTSTest( AIStrategy::BIGMONEY, mcts, 10, true );
// 	g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Big Money vs MCTS Random 100k rollout", results.m_gamesPlayed ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Games played: %i", results.m_gamesPlayed ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Time taken: %.f", results.m_timeToRun ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Big Money: %i", results.m_playerAWins ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "MCTS: %i", results.m_playerBWins ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Ties: %i", results.m_numberOfTies ) );

// 	mcts->SetSimMethod( SIMMETHOD::RANDOM );
// 	results = RunAIVsMCTSTest( AIStrategy::BIGMONEY, mcts, 100, true );
// 	g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Big Money vs MCTS Random rollout", results.m_gamesPlayed ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Games played: %i", results.m_gamesPlayed ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Time taken: %.f", results.m_timeToRun ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Big Money: %i", results.m_playerAWins ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "MCTS: %i", results.m_playerBWins ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Ties: %i", results.m_numberOfTies ) );

// 	mcts->SetSimMethod( SIMMETHOD::RANDOMPLUS );
// 	mcts->SetExplorationParameter( SquareRootFloat( 2.f ) );
// 	mcts->SetEpsilonValueZeroToOne( 0.f );
// 	mcts->SetExpansionStrategy( EXPANSIONSTRATEGY::ALLMOVES );
// 	mcts->SetRolloutMethod( ROLLOUTMETHOD::HEURISTIC );
// 	mcts->SetIterationCountPerMove( 20000 );
// 	results = RunAIVsMCTSTest( AIStrategy::BIGMONEY, mcts, 50, true );
// 	g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Big Money vs MCTS RandomPLUS 20k rollout", results.m_gamesPlayed ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Games played: %i", results.m_gamesPlayed ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Time taken: %.f", results.m_timeToRun ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Big Money: %i", results.m_playerAWins ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "MCTS: %i", results.m_playerBWins ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Ties: %i", results.m_numberOfTies ) );

	mcts->SetSimMethod( SIMMETHOD::SARASUA1	 );
	mcts->SetExplorationParameter( SquareRootFloat( 2.f ) );
	mcts->SetEpsilonValueZeroToOne( 0.15f );
	mcts->SetExpansionStrategy( EXPANSIONSTRATEGY::HEURISTICS );
	mcts->SetRolloutMethod( ROLLOUTMETHOD::EPSILONHEURISTIC );
	mcts->SetIterationCountPerMove( 10000 );
	results = RunAIVsMCTSTest( AIStrategy::DOUBLEWITCH, mcts, 50, true );
	g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Double Witch vs MCTS Sarasua1 10k Everything", results.m_gamesPlayed ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Games played: %i", results.m_gamesPlayed ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Time taken: %.f", results.m_timeToRun ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Double Witch: %i", results.m_playerAWins ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "MCTS: %i", results.m_playerBWins ) );
	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Ties: %i", results.m_numberOfTies ) );

	mcts->Shutdown();
	delete mcts;


// 	MonteCarlo* mctsA = new MonteCarlo();
// 	mctsA->Startup();
// 
// 	MonteCarlo* mctsB = new MonteCarlo();
// 	mctsB->Startup();
// 
// 	mctsA->SetSimMethod( SIMMETHOD::SARASUA1 );
// 	mctsA->SetExplorationParameter( SquareRootFloat(2.f) );
// 	mctsA->SetEpsilonValueZeroToOne( 0.f );
// 	mctsA->SetExpansionStrategy( EXPANSIONSTRATEGY::ALLMOVES );
// 	mctsA->SetRolloutMethod( ROLLOUTMETHOD::HEURISTIC );
// 	mctsA->SetIterationCountPerMove( 10000 );
// 
// 	mctsB->SetSimMethod( SIMMETHOD::SARASUA1 );
// 	mctsB->SetExplorationParameter( SquareRootFloat(2.f) );
// 	mctsB->SetEpsilonValueZeroToOne( 0.15f );
// 	mctsB->SetExpansionStrategy( EXPANSIONSTRATEGY::ALLMOVES );
// 	mctsB->SetRolloutMethod( ROLLOUTMETHOD::EPSILONHEURISTIC );
// 	mctsB->SetIterationCountPerMove( 10000 );
// 
// 	results = RunMCTSVsMCTSTest( mctsA, mctsB, 25, true );
// 	g_theConsole->PrintString( Rgba8::GREEN, Stringf( "MCTS Sarasua1 0 Chaos vs MCTS Sarasua1 .15 Chaos", results.m_gamesPlayed ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Games played: %i", results.m_gamesPlayed ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Time taken: %.f", results.m_timeToRun ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "MCTS 10K: %i", results.m_playerAWins ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "MCTS 10K: %i", results.m_playerBWins ) );
// 	g_theConsole->PrintString( Rgba8::CYAN, Stringf( "Ties: %i", results.m_numberOfTies ) );
// 
// 	mctsA->Shutdown();
// 	delete mctsA;
// 
// 	mctsB->Shutdown();
// 	delete mctsB;
}

TestResults Game::RunAIVsAITest( AIStrategy player1Strategy, AIStrategy player2Strategy, int numberOfGames, bool doesRunPlayersFlipped )
{
	TestResults results;

	double startTime = GetCurrentTimeSeconds();
	
	for( int gameIndex = 0; gameIndex < numberOfGames; gameIndex++ )
	{
		gamestate_t newGamestate = GetRandomInitialGameState();

		int isGameOver = IsGameOverForGameState( newGamestate );
		while( isGameOver == GAMENOTOVER )
		{
			if( newGamestate.m_whoseMoveIsIt == PLAYER_1 )
			{
				inputMove_t playerMove = GetBestMoveUsingAIStrategyForGamestate( player1Strategy, newGamestate );
				newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
			}
			else if( newGamestate.m_whoseMoveIsIt == PLAYER_2 )
			{
				inputMove_t playerMove = GetBestMoveUsingAIStrategyForGamestate( player2Strategy, newGamestate );
				newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
			}
			else
			{
				ERROR_AND_DIE( "Invalid player" );
			}

			isGameOver = IsGameOverForGameState( newGamestate );
		}

		results.m_gamesPlayed++;
		if( isGameOver == PLAYER_1 )
		{
			results.m_playerAWins++;
		}
		else if( isGameOver == PLAYER_2 )
		{
			results.m_playerBWins++;
		}
		else if( isGameOver == TIE )
		{
			results.m_numberOfTies++;
		}
		else
		{
			ERROR_AND_DIE( "Invalid result" );
		}
	}

	if( doesRunPlayersFlipped )
	{
		for( int gameIndex = 0; gameIndex < numberOfGames; gameIndex++ )
		{
			gamestate_t newGamestate = GetRandomInitialGameState();

			int isGameOver = IsGameOverForGameState( newGamestate );
			while( isGameOver == GAMENOTOVER  )
			{
				if( newGamestate.m_whoseMoveIsIt == PLAYER_1 )
				{
					inputMove_t playerMove = GetBestMoveUsingAIStrategyForGamestate( player2Strategy, newGamestate );
					newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
				}
				else if( newGamestate.m_whoseMoveIsIt == PLAYER_2 )
				{
					inputMove_t playerMove = GetBestMoveUsingAIStrategyForGamestate( player1Strategy, newGamestate );
					newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
				}
				else
				{
					ERROR_AND_DIE( "Invalid player" );
				}

				isGameOver = IsGameOverForGameState( newGamestate );
			}

			results.m_gamesPlayed++;
			if( isGameOver == PLAYER_1 )
			{
				results.m_playerBWins++;
			}
			else if( isGameOver == PLAYER_2 )
			{
				results.m_playerAWins++;
			}
			else if( isGameOver == TIE )
			{
				results.m_numberOfTies++;
			}
			else
			{
				ERROR_AND_DIE( "Invalid result" );
			}
		}
	}

	double endTime = GetCurrentTimeSeconds();
	results.m_timeToRun = startTime - endTime;

	return results;
}

TestResults Game::RunAIVsMCTSTest( AIStrategy playerAStrategy, MonteCarlo* mcts, int numberOfGames, bool doesRunPlayersFlipped )
{
	TestResults results;
	double AIVsMCTSStartTime = GetCurrentTimeSeconds();
	for( int gameIndex = 0; gameIndex < numberOfGames; gameIndex++ )
	{
		gamestate_t newGamestate = GetRandomInitialGameState();
		mcts->ResetTree();
		mcts->SetInitialGameState( newGamestate );

		int isGameOver = IsGameOverForGameState( newGamestate );
		while( isGameOver == GAMENOTOVER )
		{
			if( newGamestate.m_whoseMoveIsIt == PLAYER_1 )
			{
				inputMove_t playerMove = GetBestMoveUsingAIStrategyForGamestate( playerAStrategy, newGamestate );
				newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
				mcts->UpdateGame( playerMove, newGamestate );
			}
			else if( newGamestate.m_whoseMoveIsIt == PLAYER_2 )
			{
				mcts->RunMCTSForCurrentMoveIterationCount();
				while( !mcts->IsMoveReady() )
				{
					std::this_thread::sleep_for( std::chrono::microseconds( 10 ) );
				}
				inputMove_t playerMove = mcts->GetBestMove();
				newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
				mcts->UpdateGame( playerMove, newGamestate );
			}
			else
			{
				ERROR_AND_DIE( "Invalid player" );
			}

			results.m_moveCount++;

			isGameOver = IsGameOverForGameState( newGamestate );
		}

		results.m_gamesPlayed++;
		if( isGameOver == PLAYER_1 )
		{
			results.m_playerAWins++;
		}
		else if( isGameOver == PLAYER_2 )
		{
			results.m_playerBWins++;
		}
		else if( isGameOver == TIE )
		{
			results.m_numberOfTies++;
		}
		else
		{
			ERROR_AND_DIE( "Invalid result" );
		}
	}

	if( doesRunPlayersFlipped )
	{
		for( int gameIndex = 0; gameIndex < numberOfGames; gameIndex++ )
		{
			gamestate_t newGamestate = GetRandomInitialGameState();
			mcts->ResetTree();
			mcts->SetInitialGameState( newGamestate );

			int isGameOver = IsGameOverForGameState( newGamestate );
			while( isGameOver == GAMENOTOVER )
			{
				if( newGamestate.m_whoseMoveIsIt == PLAYER_1 )
				{
					mcts->RunMCTSForCurrentMoveIterationCount();
					while( !mcts->IsMoveReady() )
					{
						std::this_thread::sleep_for( std::chrono::microseconds( 10 ) );
					}
					inputMove_t playerMove = mcts->GetBestMove();
					newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
					mcts->UpdateGame( playerMove, newGamestate );
				}
				else if( newGamestate.m_whoseMoveIsIt == PLAYER_2 )
				{
					inputMove_t playerMove = GetBestMoveUsingAIStrategyForGamestate( playerAStrategy, newGamestate );
					newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
					mcts->UpdateGame( playerMove, newGamestate );
				}
				else
				{
					ERROR_AND_DIE( "Invalid player" );
				}

				isGameOver = IsGameOverForGameState( newGamestate );
			}

			results.m_gamesPlayed++;
			if( isGameOver == PLAYER_1 )
			{
				results.m_playerBWins++;
			}
			else if( isGameOver == PLAYER_2 )
			{
				results.m_playerAWins++;
			}
			else if( isGameOver == TIE )
			{
				results.m_numberOfTies++;
			}
			else
			{
				ERROR_AND_DIE( "Invalid result" );
			}
		}
	}

	double AIVsMCTSEndTime = GetCurrentTimeSeconds();

	results.m_timeToRun = AIVsMCTSEndTime - AIVsMCTSStartTime;

	results.m_moveCount/= results.m_gamesPlayed;
	return results;
}

TestResults Game::RunMCTSVsMCTSTest( MonteCarlo* mctsA, MonteCarlo* mctsB, int numberOfGames, bool doesRunPlayersFlipped )
{
	TestResults results;
	double MCTSVsMCTSStartTime = GetCurrentTimeSeconds();
	for( int gameIndex = 0; gameIndex < numberOfGames; gameIndex++ )
	{
		gamestate_t newGamestate = GetRandomInitialGameState();
		mctsA->ResetTree();
		mctsA->SetInitialGameState( newGamestate );
		mctsB->ResetTree();
		mctsB->SetInitialGameState( newGamestate );

		int isGameOver = IsGameOverForGameState( newGamestate );
		while( isGameOver == GAMENOTOVER )
		{
			if( newGamestate.m_whoseMoveIsIt == PLAYER_1 )
			{
				mctsA->RunMCTSForCurrentMoveIterationCount();
				while( !mctsA->IsMoveReady() )
				{
					std::this_thread::sleep_for( std::chrono::microseconds( 10 ) );
				}
				inputMove_t playerMove = mctsA->GetBestMove();
				newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
				mctsA->UpdateGame( playerMove, newGamestate );
				mctsB->UpdateGame( playerMove, newGamestate );
			}
			else if( newGamestate.m_whoseMoveIsIt == PLAYER_2 )
			{
				mctsB->RunMCTSForCurrentMoveIterationCount();
				while( !mctsB->IsMoveReady() )
				{
					std::this_thread::sleep_for( std::chrono::microseconds( 10 ) );
				}
				inputMove_t playerMove = mctsB->GetBestMove();
				newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
				mctsA->UpdateGame( playerMove, newGamestate );
				mctsB->UpdateGame( playerMove, newGamestate );
			}
			else
			{
				ERROR_AND_DIE( "Invalid player" );
			}

			results.m_moveCount++;

			isGameOver = IsGameOverForGameState( newGamestate );
		}

		results.m_gamesPlayed++;
		if( isGameOver == PLAYER_1 )
		{
			results.m_playerAWins++;
		}
		else if( isGameOver == PLAYER_2 )
		{
			results.m_playerBWins++;
		}
		else if( isGameOver == TIE )
		{
			results.m_numberOfTies++;
		}
		else
		{
			ERROR_AND_DIE( "Invalid result" );
		}
	}

	if( doesRunPlayersFlipped )
	{
		for( int gameIndex = 0; gameIndex < numberOfGames; gameIndex++ )
		{
			gamestate_t newGamestate = GetRandomInitialGameState();
			mctsA->ResetTree();
			mctsB->ResetTree();
			mctsA->SetInitialGameState( newGamestate );
			mctsB->SetInitialGameState( newGamestate );

			int isGameOver = IsGameOverForGameState( newGamestate );
			while( isGameOver == GAMENOTOVER )
			{
				if( newGamestate.m_whoseMoveIsIt == PLAYER_1 )
				{
					mctsB->RunMCTSForCurrentMoveIterationCount();
					while( !mctsB->IsMoveReady() )
					{
						std::this_thread::sleep_for( std::chrono::microseconds( 10 ) );
					}
					inputMove_t playerMove = mctsB->GetBestMove();
					newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
					mctsB->UpdateGame( playerMove, newGamestate );
				}
				else if( newGamestate.m_whoseMoveIsIt == PLAYER_2 )
				{
					mctsA->RunMCTSForCurrentMoveIterationCount();
					while( !mctsA->IsMoveReady() )
					{
						std::this_thread::sleep_for( std::chrono::microseconds( 10 ) );
					}
					inputMove_t playerMove = mctsA->GetBestMove();
					newGamestate = GetGameStateAfterMove( newGamestate, playerMove );
					mctsA->UpdateGame( playerMove, newGamestate );
					mctsB->UpdateGame( playerMove, newGamestate );
				}
				else
				{
					ERROR_AND_DIE( "Invalid player" );
				}

				isGameOver = IsGameOverForGameState( newGamestate );
			}

			results.m_gamesPlayed++;
			if( isGameOver == PLAYER_1 )
			{
				results.m_playerBWins++;
			}
			else if( isGameOver == PLAYER_2 )
			{
				results.m_playerAWins++;
			}
			else if( isGameOver == TIE )
			{
				results.m_numberOfTies++;
			}
			else
			{
				ERROR_AND_DIE( "Invalid result" );
			}
		}
	}

	double MCTSVsMCTSEndTime = GetCurrentTimeSeconds();

	results.m_timeToRun = MCTSVsMCTSEndTime - MCTSVsMCTSStartTime;

	results.m_moveCount/= results.m_gamesPlayed;
	return results;
}

void Game::InitializeGameState()
{
	if( m_currentGameState )
	{
		delete m_currentGameState;
		m_currentGameState = nullptr;
	}


	m_currentGameState = new gamestate_t( GetRandomInitialGameState() );


 	m_player1MCTS->Shutdown();
 	m_player1MCTS->Startup();
	m_player1MCTS->SetInitialGameState( *m_currentGameState );

	m_player2MCTS->Shutdown();
	m_player2MCTS->Startup();
	m_player2MCTS->SetInitialGameState( *m_currentGameState );
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
	m_player1MCTS->SetInitialGameState( *m_currentGameState );
	m_player2MCTS->SetInitialGameState( *m_currentGameState );
	m_isUIDirty = true;
}

bool Game::RestartGame( EventArgs const& args )
{
	UNUSED( args );
	RestartGame();

	return true;
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
//	const KeyButtonState& num2Key = g_theInput->GetKeyStates( '2' );
//	const KeyButtonState& num3Key = g_theInput->GetKeyStates( '3' );
// 	const KeyButtonState& num4Key = g_theInput->GetKeyStates( '4' );
//	const KeyButtonState& num5Key = g_theInput->GetKeyStates( '5' );
// 	const KeyButtonState& num6Key = g_theInput->GetKeyStates( '6' );
// 	const KeyButtonState& num7Key = g_theInput->GetKeyStates( '7' );
// 	const KeyButtonState& num8Key = g_theInput->GetKeyStates( '8' );
//	const KeyButtonState& num9Key = g_theInput->GetKeyStates( '9' );
// 	const KeyButtonState& qKey = g_theInput->GetKeyStates( 'Q' );
// 	const KeyButtonState& wKey = g_theInput->GetKeyStates( 'W' );
// 	const KeyButtonState& eKey = g_theInput->GetKeyStates( 'E' );
// 	const KeyButtonState& rKey = g_theInput->GetKeyStates( 'R' );
// 	const KeyButtonState& tKey = g_theInput->GetKeyStates( 'T' );
// 	const KeyButtonState& yKey = g_theInput->GetKeyStates( 'Y' );
// 	const KeyButtonState& uKey = g_theInput->GetKeyStates( 'U' );
// 	const KeyButtonState& iKey = g_theInput->GetKeyStates( 'I' );
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
	if( rBracketKey.WasJustPressed() )
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
		default: ERROR_AND_DIE( "Invalid AI strategy" );
			break;
		}
	}
	if( semiColonKey.WasJustPressed() )
	{
		switch( m_player1MCTSRolloutMethod )
		{
		case ROLLOUTMETHOD::RANDOM: m_player1MCTSRolloutMethod = ROLLOUTMETHOD::EPSILONHEURISTIC;
			break;
		case ROLLOUTMETHOD::HEURISTIC: m_player1MCTSRolloutMethod = ROLLOUTMETHOD::RANDOM;
			break;
		case ROLLOUTMETHOD::EPSILONHEURISTIC: m_player1MCTSRolloutMethod = ROLLOUTMETHOD::HEURISTIC;
			break;
		default: ERROR_AND_DIE( "Invalid rollout method" );
			break;
		}

		m_player1MCTS->SetRolloutMethod( m_player1MCTSRolloutMethod );
	}
	if( singleQuoteKey.WasJustPressed() )
	{
		switch( m_player1MCTSRolloutMethod )
		{
		case ROLLOUTMETHOD::RANDOM: m_player1MCTSRolloutMethod = ROLLOUTMETHOD::HEURISTIC;
			break;
		case ROLLOUTMETHOD::HEURISTIC: m_player1MCTSRolloutMethod = ROLLOUTMETHOD::EPSILONHEURISTIC;
			break;
		case ROLLOUTMETHOD::EPSILONHEURISTIC: m_player1MCTSRolloutMethod = ROLLOUTMETHOD::RANDOM;
			break;
		default: ERROR_AND_DIE( "Invalid rollout method" );
			break;
		}
		m_player1MCTS->SetRolloutMethod( m_player1MCTSRolloutMethod );
	}
	if( plusKey.WasJustPressed() )
	{
		switch( m_player1ExpansionStrategy )
		{
		case EXPANSIONSTRATEGY::ALLMOVES: m_player1ExpansionStrategy = EXPANSIONSTRATEGY::HEURISTICS;
			break;
		case EXPANSIONSTRATEGY::HEURISTICS: m_player1ExpansionStrategy = EXPANSIONSTRATEGY::ALLMOVES;
			break;
		default: ERROR_AND_DIE( "Invalid expansion strategy" );
			break;
		}
		m_player1MCTS->SetExpansionStrategy( m_player1ExpansionStrategy );
	}
	if( minusKey.WasJustPressed() )
	{
		switch( m_player1ExpansionStrategy )
		{
		case EXPANSIONSTRATEGY::ALLMOVES: m_player1ExpansionStrategy = EXPANSIONSTRATEGY::HEURISTICS;
			break;
		case EXPANSIONSTRATEGY::HEURISTICS: m_player1ExpansionStrategy = EXPANSIONSTRATEGY::ALLMOVES;
			break;
		default: ERROR_AND_DIE( "Invalid expansion strategy" );
			break;
		}
		m_player1MCTS->SetExpansionStrategy( m_player1ExpansionStrategy );
	}
	if( zKey.IsPressed() )
	{
		m_player1MCTSExplorationParameter -= deltaSeconds;
		m_player1MCTSExplorationParameter = Max( 0.f, m_player1MCTSExplorationParameter );

		m_player1MCTS->SetExplorationParameter( m_player1MCTSExplorationParameter );
	}
	if( xKey.IsPressed() )
	{
		m_player1MCTSExplorationParameter += deltaSeconds;
		m_player1MCTSExplorationParameter = Clampf( m_player1MCTSExplorationParameter, 0.f, 30.f );

		m_player1MCTS->SetExplorationParameter( m_player1MCTSExplorationParameter );
	}
	if( nKey.IsPressed() )
	{
		m_player1MCTSChaosChance -= 0.5f * deltaSeconds;
		m_player1MCTSChaosChance = Clampf( m_player1MCTSChaosChance, 0.f, 1.f );

		m_player1MCTS->SetEpsilonValueZeroToOne( m_player1MCTSChaosChance );
	}
	if( mKey.IsPressed() )
	{
		m_player1MCTSChaosChance += 0.5f * deltaSeconds;
		m_player1MCTSChaosChance = Clampf( m_player1MCTSChaosChance, 0.f, 1.f );

		m_player1MCTS->SetEpsilonValueZeroToOne( m_player1MCTSChaosChance );
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
		switch( m_player1MCTSSimMethod )
		{
		case SIMMETHOD::RANDOM: m_player1MCTSSimMethod = SIMMETHOD::DOUBLEWITCH;
			break;
		case SIMMETHOD::BIGMONEY: m_player1MCTSSimMethod = SIMMETHOD::RANDOM;
			break;
		case SIMMETHOD::SINGLEWITCH: m_player1MCTSSimMethod = SIMMETHOD::BIGMONEY;
			break;
		case SIMMETHOD::SARASUA1: m_player1MCTSSimMethod = SIMMETHOD::SINGLEWITCH;
			break;
		case SIMMETHOD::GREEDY: m_player1MCTSSimMethod = SIMMETHOD::SINGLEWITCH;
			break;
		case SIMMETHOD::DOUBLEWITCH: m_player1MCTSSimMethod = SIMMETHOD::SARASUA1;
			break;
		default:
			break;
		}

		m_player1MCTS->SetSimMethod( m_player1MCTSSimMethod );
	}
	if( rightArrow.WasJustPressed() )
	{
		switch( m_player1MCTSSimMethod )
		{
		case SIMMETHOD::RANDOM: m_player1MCTSSimMethod = SIMMETHOD::BIGMONEY;
			break;
		case SIMMETHOD::BIGMONEY: m_player1MCTSSimMethod = SIMMETHOD::SINGLEWITCH;
			break;
		case SIMMETHOD::SINGLEWITCH: m_player1MCTSSimMethod = SIMMETHOD::SARASUA1;
			break;
		case  SIMMETHOD::SARASUA1: m_player1MCTSSimMethod = SIMMETHOD::DOUBLEWITCH;
			break;
		case SIMMETHOD::GREEDY: m_player1MCTSSimMethod = SIMMETHOD::RANDOM;
			break;
		case SIMMETHOD::DOUBLEWITCH: m_player1MCTSSimMethod = SIMMETHOD::RANDOM;
			break;
		default:
			break;
		}

		m_player1MCTS->SetSimMethod( m_player1MCTSSimMethod );
	}
	if( f1Key.WasJustPressed() )
	{
		//AppendGameStateToFile( *m_currentGameState, "test.mcts" );

		m_player1MCTS->StopThreads();
		m_player1MCTS->FlushJobSystemQueues();
		m_player1MCTS->SaveTree();

	}
	if( f2Key.WasJustPressed() )
	{
// 		//Load gamestate
// 		size_t outSize;
// 		byte* buffer = FileReadToNewBuffer( "test.mcts", &outSize );
// 		gamestate_t gameState = gamestate_t::ParseGameStateFromBuffer( buffer );
// 
// 		*m_currentGameState = gameState;
		m_player1MCTS->StopThreads();
		m_player1MCTS->FlushJobSystemQueues();
		m_player1MCTS->LoadTree();
		m_player1MCTS->StartThreads();
		m_player1MCTS->SetInitialGameState( *m_currentGameState );
	}
	if( f3Key.WasJustPressed() )
	{
		m_player1MCTS->AddSimulations( 1'000'000 );

	}
	if( f4Key.WasJustPressed() )
	{
		m_player1MCTS->AddSimulations( 1 );
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
			m_player1MCTS->AddSimulations( 400 );
			m_player1SimCount += 400;
		}
		else
		{
			inputMove_t move = GetBestMoveUsingAIStrategy( m_player1Strategy );
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
		m_player1MCTS->AddSimulations( 1'000 );
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
			
			inputMove_t move;
			move.m_moveType = PLAY_CARD;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndex = m_currentGameState->m_playerBoards[move.m_whoseMoveIsIt].GetCardIndexFromHandIndex( 0 );
				
			PlayMoveIfValid( move );


		}

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
				m_player1SimCount = m_player1MCTS->GetCurrentNumberOfSimulationsLeft();
				if( m_timer.CheckAndDecrementAll() )
				{
					m_player1MCTS->AddSimulations( 5000 );

					//inputMove_t move = m_mc->GetBestMove();
					inputMove_t move = m_player1MCTS->GetBestMove();
					if( move.m_moveType == INVALID_MOVE )
					{

					}
					else
					{
						PlayMoveIfValid( move );
					}

					DebugAddScreenPoint( Vec2( 0.5, 0.5f ), 100.f, Rgba8::YELLOW, 0.f );
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
				m_player2SimCount = m_player2MCTS->GetCurrentNumberOfSimulationsLeft();
				if( m_timer.CheckAndDecrementAll() )
				{
					m_player2MCTS->AddSimulations( 5000 );

					//inputMove_t move = m_mc->GetBestMove();
					inputMove_t move = m_player2MCTS->GetBestMove();
					if( move.m_moveType == INVALID_MOVE )
					{

					}
					else
					{
						PlayMoveIfValid( move );
					}

					DebugAddScreenPoint( Vec2( 0.5, 0.5f ), 100.f, Rgba8::YELLOW, 0.f );
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

AIStrategy Game::StringToAIStrategy( std::string const& strategyStr ) const
{
	if( strategyStr == "Random" )
	{
		return AIStrategy::RANDOM;
	}
	else if( strategyStr == "Big Money" )
	{
		return AIStrategy::BIGMONEY;
	}
	else if( strategyStr == "Single Witch" )
	{
		return AIStrategy::SINGLEWITCH;
	}
	else if( strategyStr == "Sarasua1" )
	{
		return AIStrategy::SARASUA1;
	}
	else if( strategyStr == "MCTS" )
	{
		return AIStrategy::MCTS;
	}
	else if( strategyStr == "Double Witch" )
	{
		return AIStrategy::DOUBLEWITCH;
	}
	else
	{
		ERROR_AND_DIE( "String invalid" );
	}
}

bool Game::TogglePlayer1UseChaosChance( EventArgs const& args )
{
	UNUSED( args );
	if( m_player1MCTSRolloutMethod == ROLLOUTMETHOD::HEURISTIC )
	{
		m_player1MCTSRolloutMethod = ROLLOUTMETHOD::EPSILONHEURISTIC;
		m_player1UseChaosChanceWidget->SetText( "Use Chaos Chance: [x]" );
	}
	else if( m_player1MCTSRolloutMethod == ROLLOUTMETHOD::EPSILONHEURISTIC )
	{
		m_player1MCTSRolloutMethod = ROLLOUTMETHOD::HEURISTIC;
		m_player1UseChaosChanceWidget->SetText( "Use Chaos Chance: [ ]" );
	}

	m_player1MCTS->SetRolloutMethod( m_player1MCTSRolloutMethod );

	return true;
}

bool Game::TogglePlayer2UseChaosChance( EventArgs const& args )
{
	UNUSED( args );
	if( m_player2MCTSRolloutMethod == ROLLOUTMETHOD::HEURISTIC )
	{
		m_player2MCTSRolloutMethod = ROLLOUTMETHOD::EPSILONHEURISTIC;
		m_player2UseChaosChanceWidget->SetText( "Use Chaos Chance: [x]" );
	}
	else if( m_player2MCTSRolloutMethod == ROLLOUTMETHOD::EPSILONHEURISTIC )
	{
		m_player2MCTSRolloutMethod = ROLLOUTMETHOD::HEURISTIC;
		m_player2UseChaosChanceWidget->SetText( "Use Chaos Chance: [ ]" );
	}

	m_player2MCTS->SetRolloutMethod( m_player2MCTSRolloutMethod );

	return true;
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

 				CardDefinition const* card = CardDefinition::GetCardDefinitionByType( (eCards)handIndex );

				std::string playStr = Stringf( "Player %i played %s", whoseMoveBase1, card->GetCardName().c_str() );
				g_theConsole->PrintString( Rgba8::CYAN, playStr );
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

	m_player1MCTS->UpdateGame( moveToPlay, *m_currentGameState );
	m_player2MCTS->UpdateGame( moveToPlay, *m_currentGameState );
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
		inputMove_t move = GetBestMoveUsingAIStrategy( m_player1Strategy, m_player1MCTS );
		if( move.m_moveType == INVALID_MOVE )
		{

		}
		else
		{
			PlayMoveIfValid( move );
		}

		DebugAddScreenPoint( Vec2( 0.5, 0.5f ), 100.f, Rgba8::YELLOW, 0.f );
		m_player1MCTS->AddSimulations( 400 );
		m_player1SimCount += 400;
	}
	else
	{
		inputMove_t move = GetBestMoveUsingAIStrategy( m_player2Strategy, m_player2MCTS );
		if( move.m_moveType == INVALID_MOVE )
		{

		}
		else
		{
			PlayMoveIfValid( move );
		}

		DebugAddScreenPoint( Vec2( 0.5, 0.5f ), 100.f, Rgba8::YELLOW, 0.f );
		m_player2MCTS->AddSimulations( 400 );
		m_player2SimCount += 400;
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

bool Game::AddSimsForPlayer1( EventArgs const& args )
{
	UNUSED( args );
	m_player1MCTS->AddSimulations( 1'000'000 );
	m_isUIDirty = true;

	return true;
}

bool Game::AddSimsForPlayer2( EventArgs const& args )
{
	UNUSED( args );
	m_player2MCTS->AddSimulations( 1'000'000 );
	m_isUIDirty = true;

	return true;
}

bool Game::ChangePlayer1Strategy( EventArgs const& args )
{
	std::string value = args.GetValue( "value", "Invalid" );
	AIStrategy strategy = StringToAIStrategy( value );
	m_player1Strategy = strategy;

	return true;
}

bool Game::ChangePlayer2Strategy( EventArgs const& args )
{
	std::string value = args.GetValue( "value", "Invalid" );
	AIStrategy strategy = StringToAIStrategy( value );
	m_player2Strategy = strategy;

	return true;
}

bool Game::ChangePlayer1UCTValue( EventArgs const& args )
{
	float uctValue = args.GetValue( "value", 0.f );
	m_player1MCTS->SetUCBValue( uctValue );

	return true;
}

bool Game::ChangePlayer2UCTValue( EventArgs const& args )
{
	float uctValue = args.GetValue( "value", 0.f );
	m_player2MCTS->SetUCBValue( uctValue );

	return true;
}

bool Game::ChangePlayer1ExpansionMethod( EventArgs const& args )
{
	std::string expansionMethod = args.GetValue( "value", "Invalid" );

	if( expansionMethod == "All Moves" )
	{
		m_player1ExpansionStrategy = EXPANSIONSTRATEGY::ALLMOVES;
	}
	else if( expansionMethod == "Heuristics" )
	{
		m_player1ExpansionStrategy = EXPANSIONSTRATEGY::HEURISTICS;
	}
	else
	{
		ERROR_AND_DIE( "Invalid Expansion Method");
	}

	m_player1MCTS->SetExpansionStrategy( m_player1ExpansionStrategy );
	return true;
}

bool Game::ChangePlayer2ExpansionMethod( EventArgs const& args )
{
	std::string expansionMethod = args.GetValue( "value", "Invalid" );

	if( expansionMethod == "All Moves" )
	{
		m_player2ExpansionStrategy = EXPANSIONSTRATEGY::ALLMOVES;
	}
	else if( expansionMethod == "Heuristics" )
	{
		m_player2ExpansionStrategy = EXPANSIONSTRATEGY::HEURISTICS;
	}
	else
	{
		ERROR_AND_DIE( "Invalid Expansion Method" );
	}

	m_player2MCTS->SetExpansionStrategy( m_player2ExpansionStrategy );
	return true;
}

bool Game::ChangePlayer1RolloutMethod( EventArgs const& args )
{
	std::string rolloutMethod = args.GetValue( "value", "Invalid" );
	if( rolloutMethod == "Random" )
	{
		m_player1MCTSSimMethod = SIMMETHOD::RANDOM;
	}
	else if( rolloutMethod == "Big Money" )
	{
		m_player1MCTSSimMethod = SIMMETHOD::BIGMONEY;
	}
	else if( rolloutMethod == "Single Witch" )
	{
		m_player1MCTSSimMethod = SIMMETHOD::SINGLEWITCH;
	}
	else if( rolloutMethod == "Double Witch" )
	{
		m_player1MCTSSimMethod = SIMMETHOD::DOUBLEWITCH;
	}
	else if( rolloutMethod == "Sarasua1" )
	{
		m_player1MCTSSimMethod = SIMMETHOD::SARASUA1;
	}
	else if( rolloutMethod == "Greedy" )
	{
		m_player1MCTSSimMethod = SIMMETHOD::GREEDY;
	}
	else if( rolloutMethod == "RandomPLUS" )
	{
		m_player1MCTSSimMethod = SIMMETHOD::RANDOMPLUS;
	}
	else
	{
		ERROR_AND_DIE( "Invalid rollout method" );
	}

	m_player1MCTS->SetSimMethod( m_player1MCTSSimMethod );
	return true;
}

bool Game::ChangePlayer2RolloutMethod( EventArgs const& args )
{
	std::string rolloutMethod = args.GetValue( "value", "Invalid" );
	if( rolloutMethod == "Random" )
	{
		m_player2MCTSSimMethod = SIMMETHOD::RANDOM;
	}
	else if( rolloutMethod == "Big Money" )
	{
		m_player2MCTSSimMethod = SIMMETHOD::BIGMONEY;
	}
	else if( rolloutMethod == "Single Witch" )
	{
		m_player2MCTSSimMethod = SIMMETHOD::SINGLEWITCH;
	}
	else if( rolloutMethod == "Double Witch" )
	{
		m_player2MCTSSimMethod = SIMMETHOD::DOUBLEWITCH;
	}
	else if( rolloutMethod == "Sarasua1" )
	{
		m_player2MCTSSimMethod = SIMMETHOD::SARASUA1;
	}
	else if( rolloutMethod == "Greedy" )
	{
		m_player2MCTSSimMethod = SIMMETHOD::GREEDY;
	}
	else if( rolloutMethod == "RandomPLUS" )
	{
		m_player2MCTSSimMethod = SIMMETHOD::RANDOMPLUS;
	}
	else
	{
		ERROR_AND_DIE( "Invalid rollout method" );
	}

	m_player2MCTS->SetSimMethod( m_player2MCTSSimMethod );
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
					inputMove_t newMove = move;
					newMove.m_moveType = PLAY_CARD;
					newMove.m_cardIndex = cardIndex;
					validMoves.push_back( newMove );
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

inputMove_t Game::GetBestMoveUsingAIStrategy( AIStrategy aiStrategy, MonteCarlo* mcts )
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
	case AIStrategy::MCTS: return mcts->GetBestMove();
		break;
	default: return inputMove_t();
		break;
	}
}

inputMove_t Game::GetBestMoveUsingAIStrategyForGamestate( AIStrategy aiStrategy, gamestate_t const& gameState )
{
	switch( aiStrategy )
	{
	case AIStrategy::RANDOM: return GetRandomMoveAtGameState( gameState );
		break;
	case AIStrategy::BIGMONEY: return GetMoveUsingBigMoney( gameState );
		break;
	case AIStrategy::SINGLEWITCH: return GetMoveUsingSingleWitch( gameState );
		break;
	case AIStrategy::DOUBLEWITCH: return GetMoveUsingDoubleWitch( gameState );
		break;
	case AIStrategy::SARASUA1: return GetMoveUsingSarasua1( gameState );
		break;
	default: return inputMove_t();
		break;
	}
}

inputMove_t Game::GetRandomMoveAtGameState( gamestate_t const& currentGameState )
{
	std::vector<inputMove_t> validMoves = GetValidMovesAtGameState( currentGameState );

	//Game Over
	if( validMoves.size() == 0 )
	{
		return inputMove_t();
	}
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
		else if( currentMoney >= 8 && currentGameState.m_cardPiles[PROVINCE].m_pileSize > 0 )
		{
			newMove.m_cardIndex = (int)PROVINCE;
		}
		else if( currentMoney >= 6 && currentGameState.m_cardPiles[GOLD].m_pileSize > 0 )
		{
			newMove.m_cardIndex = (int)GOLD;
		}
		else if( currentMoney >= 3 && currentGameState.m_cardPiles[SILVER].m_pileSize > 0 )
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
		else if( !playerDeck->HasCard( Witch ) && currentMoney >= 5 && currentGameState.m_cardPiles[Witch].m_pileSize > 0 )
		{
			newMove.m_cardIndex = (int)Witch;
		}
		else if( currentMoney >= 8 && currentGameState.m_cardPiles[PROVINCE].m_pileSize > 0 )
		{
			newMove.m_cardIndex = (int)PROVINCE;
		}
		else if( currentMoney >= 6 && currentGameState.m_cardPiles[GOLD].m_pileSize > 0 )
		{
			newMove.m_cardIndex = (int)GOLD;
		}
		else if( currentMoney >= 3 && currentGameState.m_cardPiles[SILVER].m_pileSize > 0 )
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
		else if( witchCount < 2 && currentMoney >= 5 && currentGameState.m_cardPiles[Witch].m_pileSize > 0 )
		{
			newMove.m_cardIndex = (int)Witch;
		}
		else if( currentMoney >= 8 && currentGameState.m_cardPiles[PROVINCE].m_pileSize > 0 )
		{
			newMove.m_cardIndex = (int)PROVINCE;
		}
		else if( currentMoney >= 6 && currentGameState.m_cardPiles[GOLD].m_pileSize > 0 )
		{
			newMove.m_cardIndex = (int)GOLD;
		}
		else if( currentMoney >= 3 && currentGameState.m_cardPiles[SILVER].m_pileSize > 0 )
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
				std::vector<inputMove_t> validMoves = GetValidMovesAtGameState( currentGameState );
				for( size_t moveIndex = 0; moveIndex < validMoves.size(); moveIndex++ )
				{
					if( validMoves[moveIndex].m_moveType != END_PHASE )
					{
						return validMoves[moveIndex];
					}
				}

				newMove.m_moveType = END_PHASE;
				return newMove;

				//return randomMove;
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
				if( witchCount < 2 && currentGameState.m_cardPiles[Witch].m_pileSize > 0 )
				{
					newMove.m_cardIndex = eCards::Witch;
				}
				else if( laboratoryCount < 1 && currentGameState.m_cardPiles[Laboratory].m_pileSize > 0 )
				{
					newMove.m_cardIndex = eCards::Laboratory;
				}
			}
			else if( currentMoney >= 8 && currentGameState.m_cardPiles[PROVINCE].m_pileSize > 0 )
			{
				newMove.m_cardIndex = eCards::PROVINCE;
			}
			else if( currentMoney >= 6 )
			{
				if( provincesLeft < 3 && currentGameState.m_cardPiles[DUCHY].m_pileSize > 0 )
				{
					newMove.m_cardIndex = eCards::DUCHY;
				}
				else if( currentGameState.m_cardPiles[GOLD].m_pileSize > 0 )
				{
					newMove.m_cardIndex = eCards::GOLD;
				}
			}
			else if( currentMoney >= 5 )
			{
				if( provincesLeft <= 5 && currentGameState.m_cardPiles[DUCHY].m_pileSize > 0 )
				{
					newMove.m_cardIndex = eCards::DUCHY;
				}
				else if( silverCount < 3 && currentGameState.m_cardPiles[SILVER].m_pileSize > 0 )
				{
					newMove.m_cardIndex = eCards::SILVER;
				}
				else if( laboratoryCount < 2 && currentGameState.m_cardPiles[Laboratory].m_pileSize > 0 )
				{
					newMove.m_cardIndex = eCards::Laboratory;
				}
				else if( villageCount < 1 && currentGameState.m_cardPiles[Village].m_pileSize > 0 )
				{
					newMove.m_cardIndex = eCards::Village;
				}
				else if( currentGameState.m_cardPiles[SILVER].m_pileSize > 0 )
				{
					newMove.m_cardIndex = eCards::SILVER;
				}
			}
			else if( currentMoney >= 3 )
			{
				if( provincesLeft <= 2 && currentGameState.m_cardPiles[ESTATE].m_pileSize > 0 )
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

inputMove_t Game::GetMoveUsingHighestVP( gamestate_t const& currentGameState )
{
	int whoseMove = currentGameState.m_whoseMoveIsIt;
	PlayerBoard const& currentPlayerBoard = currentGameState.m_playerBoards[whoseMove];

	eGamePhase currentPhase = currentGameState.m_currentPhase;

	if( currentPhase == eGamePhase::ACTION_PHASE )
	{
		inputMove_t move;
		move.m_whoseMoveIsIt = whoseMove;
		move.m_moveType = END_PHASE;
		return move;
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
			if( currentMoney >= 8 && currentGameState.m_cardPiles[PROVINCE].m_pileSize > 0 )
			{
				move.m_cardIndex = PROVINCE;
			}
			else if( currentMoney >= 5 && currentGameState.m_cardPiles[DUCHY].m_pileSize > 0 )
			{
				move.m_cardIndex = DUCHY;
			}
			else if( currentMoney >= 2 && currentGameState.m_cardPiles[ESTATE].m_pileSize > 0 )
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

inputMove_t Game::GetMoveUsingRandomPlus( gamestate_t const& currentGameState )
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
				std::vector<inputMove_t> validMoves = GetValidMovesAtGameState( currentGameState );
				if( validMoves.size() == 1 )
				{
					return validMoves[0];
				}
				else if( validMoves.size() > 1 )
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
				else
				{
					return randomMove;
				}
			}
		}

	}
	else
	{
		return randomMove;
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