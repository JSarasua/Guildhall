#pragma once
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/MonteCarlo.hpp"
#include "Game/MonteCarloNoTree.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/World.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Audio/AudioSystem.hpp"
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

	//m_mcts = new MonteCarlo();
	//m_mcts->Startup( CIRCLEPLAYER );
	m_mc = new MonteCarloNoTree();
	m_mcts = new MonteCarlo();
	CardDefinition::InitializeCards();
	InitializeGameState();

	m_timer.SetSeconds( 0 );
}

void Game::Shutdown()
{
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
}

void Game::InitializeGameState()
{
	if( m_currentGameState )
	{
		delete m_currentGameState;
		m_currentGameState = nullptr;
	}


	PlayerBoard starterDeck = PlayerBoard( &m_rand );
	std::vector<CardDefinition const*> starterCards;
	starterCards.reserve( 10 );
	CardDefinition const* copper = CardDefinition::GetCardDefinitionByType( eCards::COPPER );
	CardDefinition const* silver = CardDefinition::GetCardDefinitionByType( eCards::SILVER );
	CardDefinition const* gold = CardDefinition::GetCardDefinitionByType( eCards::GOLD );
	CardDefinition const* estate = CardDefinition::GetCardDefinitionByType( eCards::ESTATE );
	CardDefinition const* duchy = CardDefinition::GetCardDefinitionByType( eCards::DUCHY );
	CardDefinition const* province = CardDefinition::GetCardDefinitionByType( eCards::PROVINCE );
	CardDefinition const* curse = CardDefinition::GetCardDefinitionByType( eCards::CURSE );
	
	CardDefinition const* village = CardDefinition::GetCardDefinitionByType( eCards::Village );
	CardDefinition const* smithy = CardDefinition::GetCardDefinitionByType( eCards::Smithy );
	CardDefinition const* festival = CardDefinition::GetCardDefinitionByType( eCards::Festival );
	CardDefinition const* laboratory = CardDefinition::GetCardDefinitionByType( eCards::Laboratory );
	CardDefinition const* market = CardDefinition::GetCardDefinitionByType( eCards::Market );
	CardDefinition const* councilRoom = CardDefinition::GetCardDefinitionByType( eCards::CouncilRoom );
	CardDefinition const* witch = CardDefinition::GetCardDefinitionByType( eCards::Witch );




	//7 copper
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	//3 estate
	starterCards.push_back( estate );
	starterCards.push_back( estate );
	starterCards.push_back( estate );
	starterDeck.InitializeDeck( starterCards );

	m_currentGameState = new gamestate_t();
	m_currentGameState->m_isFirstMove = false;
	m_currentGameState->m_playerBoards[0] = starterDeck;
	m_currentGameState->m_playerBoards[1] = starterDeck;
	
	m_currentGameState->m_playerBoards[0].ShuffleDeck();
	m_currentGameState->m_playerBoards[1].ShuffleDeck();

	m_currentGameState->m_playerBoards[0].Draw5();
	m_currentGameState->m_playerBoards[1].Draw5();

	m_currentGameState->m_currentPhase = ACTION_PHASE;

	//std::array<pileData_t>& cardPiles = m_currentGameState->m_cardPiles;
	pileData_t* cardPiles = &m_currentGameState->m_cardPiles[0];
	cardPiles[(int)eCards::COPPER].m_card = copper;
	cardPiles[(int)eCards::COPPER].m_pileSize = MONEYPILESIZE;
	cardPiles[(int)eCards::SILVER].m_card = silver;
	cardPiles[(int)eCards::SILVER].m_pileSize = MONEYPILESIZE;
	cardPiles[(int)eCards::GOLD].m_card = gold;
	cardPiles[(int)eCards::GOLD].m_pileSize = MONEYPILESIZE;
	cardPiles[(int)eCards::ESTATE].m_card = estate;
	cardPiles[(int)eCards::ESTATE].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::DUCHY].m_card = duchy;
	cardPiles[(int)eCards::DUCHY].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::PROVINCE].m_card = province;
	cardPiles[(int)eCards::PROVINCE].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::CURSE].m_card = curse;
	cardPiles[(int)eCards::CURSE].m_pileSize = CURSEPILESIZE;
	cardPiles[(int)eCards::Village].m_card = village;
	cardPiles[(int)eCards::Village].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Smithy].m_card = smithy;
	cardPiles[(int)eCards::Smithy].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Festival].m_card = festival;
	cardPiles[(int)eCards::Festival].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Laboratory].m_card = laboratory;
	cardPiles[(int)eCards::Laboratory].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Market].m_card = market;
	cardPiles[(int)eCards::Market].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::CouncilRoom].m_card = councilRoom;
	cardPiles[(int)eCards::CouncilRoom].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Witch].m_card = witch;
	cardPiles[(int)eCards::Witch].m_pileSize = ACTIONPILESIZE;


	m_currentGameState->m_whoseMoveIsIt = PLAYER_1;

	m_mc->SetCurrentGameState( *m_currentGameState );
	m_mc->ResetPossibleMoves();
 	m_mcts->Shutdown();
 	m_mcts->Startup( *m_currentGameState );
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

// 	const KeyButtonState& leftArrow = g_theInput->GetKeyStates( 0x25 );
// 	const KeyButtonState& upArrow = g_theInput->GetKeyStates( 0x26 );
// 	const KeyButtonState& rightArrow = g_theInput->GetKeyStates( 0x27 );
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
 	const KeyButtonState& f5Key = g_theInput->GetKeyStates( F5_KEY );
 	const KeyButtonState& f6Key = g_theInput->GetKeyStates( F6_KEY );
 	const KeyButtonState& f7Key = g_theInput->GetKeyStates( F7_KEY );
 	const KeyButtonState& f8Key = g_theInput->GetKeyStates( F8_KEY );
	const KeyButtonState& f9Key = g_theInput->GetKeyStates( F9_KEY );
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
// 	const KeyButtonState& lBracketKey = g_theInput->GetKeyStates( LBRACKET_KEY );
// 	const KeyButtonState& rBracketKey = g_theInput->GetKeyStates( RBRACKET_KEY );
// 	const KeyButtonState& fKey = g_theInput->GetKeyStates( 'F' );
// 	const KeyButtonState& tKey = g_theInput->GetKeyStates( 'T' );
// 	const KeyButtonState& gKey = g_theInput->GetKeyStates( 'G' );
// 	const KeyButtonState& hKey = g_theInput->GetKeyStates( 'H' );
// 	//const KeyButtonState& yKey = g_theInput->GetKeyStates( 'Y' );
// 	const KeyButtonState& vKey = g_theInput->GetKeyStates( 'V' );
// 	const KeyButtonState& bKey = g_theInput->GetKeyStates( 'B' );
// 	const KeyButtonState& nKey = g_theInput->GetKeyStates( 'N' );
// 	const KeyButtonState& mKey = g_theInput->GetKeyStates( 'M' );
// 	const KeyButtonState& jKey = g_theInput->GetKeyStates( 'J' );
// 	const KeyButtonState& kKey = g_theInput->GetKeyStates( 'K' );
// 	const KeyButtonState& zKey = g_theInput->GetKeyStates( 'Z' );
// 	const KeyButtonState& xKey = g_theInput->GetKeyStates( 'X' );
// 	const KeyButtonState& plusKey = g_theInput->GetKeyStates( PLUS_KEY );
// 	const KeyButtonState& minusKey = g_theInput->GetKeyStates( MINUS_KEY );
// 	const KeyButtonState& semiColonKey = g_theInput->GetKeyStates( SEMICOLON_KEY );
// 	const KeyButtonState& singleQuoteKey = g_theInput->GetKeyStates( SINGLEQUOTE_KEY );
// 	const KeyButtonState& commaKey = g_theInput->GetKeyStates( COMMA_KEY );
// 	const KeyButtonState& periodKey = g_theInput->GetKeyStates( PERIOD_KEY );
	const KeyButtonState& enterKey = g_theInput->GetKeyStates( ENTER_KEY );

	if( f1Key.WasJustPressed() )
	{
		DebugAddScreenPoint( Vec2( 0.5, 0.5f ), 100.f, Rgba8::YELLOW, 0.f );
		m_mc->RunSimulations( 400 );
/*		m_mcts->RunSimulations( 1000 );*/

	}
	if( f2Key.WasJustPressed() )
	{
		inputMove_t move = m_mc->GetBestMove();
		PlayMoveIfValid( move );
		//inputMove_t move = m_mcts->GetBestMove();

/*		PlayMoveIfValid( move.m_move );*/
	}
	if( f3Key.WasJustPressed() )
	{
		inputMove_t bigMoneyAI = GetMoveUsingBigMoney( *m_currentGameState );
		PlayMoveIfValid( bigMoneyAI );
	}
	if( f5Key.WasJustPressed() )
	{
		if( m_currentGameState->m_whoseMoveIsIt == PLAYER_1 )
		{
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
			//m_mc->RunSimulations( 400 );
			m_mcts->RunSimulations( 400 );
		}
		else
		{
			inputMove_t bigMoneyAI = GetMoveUsingBigMoney( *m_currentGameState );
			PlayMoveIfValid( bigMoneyAI );
		}
	}
	if( f6Key.WasJustPressed() )
	{
		m_isAutoPlayEnabled = !m_isAutoPlayEnabled;
	}
	if( f7Key.WasJustPressed() )
	{
		InitializeGameState();
	}
	if( f8Key.WasJustPressed() )
	{
		m_mcts->RunSimulations( 500 );
	}
	if( f9Key.WasJustPressed() )
	{
		inputMove_t move = m_mcts->GetBestMove();
		PlayMoveIfValid( move );
	}
	if( f11Key.WasJustPressed() )
	{
		m_mcts->RunSimulations( 10000 );
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
			move.m_cardIndexToBuy = 0;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			inputMove_t move;
			move.m_moveType = PLAY_CARD;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardHandIndexToPlay = 0;
			PlayMoveIfValid( move );
		}

	}
	if( num2Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndexToBuy = 1;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			inputMove_t move;
			move.m_moveType = PLAY_CARD;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardHandIndexToPlay = 1;
			PlayMoveIfValid( move );
		}
	}
	if( num3Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndexToBuy = 2;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			inputMove_t move;
			move.m_moveType = PLAY_CARD;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardHandIndexToPlay = 2;
			PlayMoveIfValid( move );
		}
	}
	if( num4Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndexToBuy = 3;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			inputMove_t move;
			move.m_moveType = PLAY_CARD;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardHandIndexToPlay = 3;
			PlayMoveIfValid( move );
		}
	}
	if( num5Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndexToBuy = 4;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			inputMove_t move;
			move.m_moveType = PLAY_CARD;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardHandIndexToPlay = 4;
			PlayMoveIfValid( move );
		}
	}
	if( num6Key.WasJustPressed() )
	{

		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndexToBuy = 5;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			inputMove_t move;
			move.m_moveType = PLAY_CARD;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardHandIndexToPlay = 5;
			PlayMoveIfValid( move );
		}
	}
	if( num7Key.WasJustPressed() )
	{		

		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndexToBuy = 6;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			inputMove_t move;
			move.m_moveType = PLAY_CARD;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardHandIndexToPlay = 6;
			PlayMoveIfValid( move );
		}
	}
	if( num8Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndexToBuy = 7;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			inputMove_t move;
			move.m_moveType = PLAY_CARD;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardHandIndexToPlay = 7;
			PlayMoveIfValid( move );
		}
	}
	if( num9Key.WasJustPressed() )
	{
		if( m_currentGameState->m_currentPhase == BUY_PHASE )
		{
			inputMove_t move;
			move.m_moveType = BUY_MOVE;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardIndexToBuy = 8;
			PlayMoveIfValid( move );
		}
		else if( m_currentGameState->m_currentPhase == ACTION_PHASE )
		{
			inputMove_t move;
			move.m_moveType = PLAY_CARD;
			move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
			move.m_cardHandIndexToPlay = 8;
			PlayMoveIfValid( move );
		}
	}
	if( qKey.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 8;
		PlayMoveIfValid( move );
	}
	if( wKey.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 9;
		PlayMoveIfValid( move );
	}
	if( eKey.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 10;
		PlayMoveIfValid( move );
	}
	if( rKey.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 11;
		PlayMoveIfValid( move );
	}
	if( tKey.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 12;
		PlayMoveIfValid( move );
	}
	if( yKey.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 13;
		PlayMoveIfValid( move );
	}
	if( uKey.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 14;
		PlayMoveIfValid( move );
	}
	if( iKey.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 15;
		PlayMoveIfValid( move );
	}
}



void Game::DebugDrawGame()
{
	//Render Player 2s hand at top of screen

	std::vector<CardDefinition const*> player1Hand = m_currentGameState->m_playerBoards[0].GetHand();
	std::vector<CardDefinition const*> player2Hand = m_currentGameState->m_playerBoards[1].GetHand();

	std::vector<CardDefinition const*> player1PlayArea = m_currentGameState->m_playerBoards[0].GetPlayArea();
	std::vector<CardDefinition const*> player2PlayArea = m_currentGameState->m_playerBoards[1].GetPlayArea();

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
	Vec2 phasePos = gameDataArea.GetPointAtUV( Vec2( 0.1f, 0.9f ) );
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
	std::string phaseStr;

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

	DebugAddScreenText( Vec4( Vec2(), phasePos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, phaseStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), coinPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, coinStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), buysPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, buysStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), actionsPos ), Vec2( 0.f, 0.5f ), 12.f, textColor, textColor, 0.f, actionsStr.c_str() );

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

	AABB2 carvingPlayer2Hand = player2HandAABB;

	Rgba8 handColor = Rgba8::WindsorTan;
	Rgba8 playAreaColor = Rgba8::Tan;
	Rgba8 pileAreaColor = Rgba8::SandyTan;

	DebugAddScreenAABB2( player2HandAABB, handColor, 0.f );
	DebugAddScreenAABB2( player2PlayAreaAABB, playAreaColor, 0.f );
	DebugAddScreenAABB2( pilesArea, pileAreaColor, 0.f );
	DebugAddScreenAABB2( player1PlayAreaAABB, playAreaColor, 0.f );
	DebugAddScreenAABB2( player1HandAABB, handColor, 0.f );

	Vec4 debugCenter = Vec4( Vec2(), player2HandAABB.GetCenter() );

	for( size_t player2HandIndex = 0; player2HandIndex < player2Hand.size(); player2HandIndex++ )
	{
		float player2HandSize = (float)player2Hand.size();
		float carvingNumber = player2HandSize - (float)player2HandIndex;
		AABB2 cardArea = carvingPlayer2Hand.CarveBoxOffLeft( 1.f / carvingNumber );
		CardDefinition const* card = player2Hand[player2HandIndex];
		Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
		std::string cardName = card->GetCardName();
		DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, textColor, textColor, 0.f, cardName.c_str() );
	}

	for( size_t player2PlayAreaIndex = 0; player2PlayAreaIndex < player2PlayArea.size(); player2PlayAreaIndex++ )
	{
		float player2PlayAreaSize = (float)player2PlayArea.size();
		float carvingNumber = player2PlayAreaSize - (float)player2PlayAreaIndex;
		AABB2 cardArea = player2PlayAreaAABB.CarveBoxOffLeft( 1.f / carvingNumber );
		CardDefinition const* card = player2PlayArea[player2PlayAreaIndex];
		Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
		std::string cardName = card->GetCardName();
		DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, textColor, textColor, 0.f, cardName.c_str() );
	}

	std::vector<int> aiValidMoves = m_mc->GetCurrentBuyIndexes();
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

		CardDefinition const* card = piles[pilesIndex].m_card;
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

	for( size_t player1PlayAreaIndex = 0; player1PlayAreaIndex < player1PlayArea.size(); player1PlayAreaIndex++ )
	{
		float player1PlayAreaSize = (float)player1PlayArea.size();
		float carvingNumber = player1PlayAreaSize - (float)player1PlayAreaIndex;
		AABB2 cardArea = player1PlayAreaAABB.CarveBoxOffLeft( 1.f / carvingNumber );
		CardDefinition const* card = player1PlayArea[player1PlayAreaIndex];
		Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
		std::string cardName = card->GetCardName();
		DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, textColor, textColor, 0.f, cardName.c_str() );
	}

	for( size_t player1HandIndex = 0; player1HandIndex < player1Hand.size(); player1HandIndex++ )
	{
		float player1HandSize = (float)player1Hand.size();
		float carvingNumber = player1HandSize - (float)player1HandIndex;
		AABB2 cardArea = player1HandAABB.CarveBoxOffLeft( 1.f / carvingNumber );
		CardDefinition const* card = player1Hand[player1HandIndex];
		Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
		std::string cardName = card->GetCardName();
		DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, textColor, textColor, 0.f, cardName.c_str() );
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
		//inputMove_t currentBestMove = m_mc->GetBestMove();
		inputMove_t currentBestMove = m_mcts->GetBestMove();
		if( currentBestMove.m_moveType != INVALID_MOVE )
		{
			std::string moveStr;

			if( currentBestMove.m_moveType == BUY_MOVE )
			{
				CardDefinition const* card = m_currentGameState->m_cardPiles[currentBestMove.m_cardIndexToBuy].m_card;
				moveStr = Stringf( " Best Move: Buy %s", card->GetCardName().c_str() );
			}
			else if( currentBestMove.m_moveType == PLAY_CARD )
			{
				CardDefinition const* card = playerDeck->GetHand()[currentBestMove.m_cardHandIndexToPlay];
				moveStr = Stringf( "Best Move: Play %s", card->GetCardName().c_str() );
			}
			else if( currentBestMove.m_moveType == END_PHASE )
			{
				moveStr = Stringf( "Best move: End Phase" );
			}
			DebugAddScreenText( Vec4( 0.5f, 0.95f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::RED, Rgba8::RED, 0.f, moveStr.c_str() );
		}
	}
	else if( m_currentGameState->m_whoseMoveIsIt == PLAYER_2 )
	{
		inputMove_t bigMoneyMove = GetMoveUsingBigMoney( *m_currentGameState );
		std::string bigMoneyStr;
		if( bigMoneyMove.m_moveType == BUY_MOVE )
		{
			CardDefinition const* card = m_currentGameState->m_cardPiles[bigMoneyMove.m_cardIndexToBuy].m_card;
			bigMoneyStr = Stringf( "Big Money Best Move: Buy %s", card->GetCardName().c_str() );
		}
		else if( bigMoneyMove.m_moveType == PLAY_CARD )
		{
			CardDefinition const* card = playerDeck->GetHand()[bigMoneyMove.m_cardHandIndexToPlay];
			bigMoneyStr = Stringf( "Big Money Best Move: Play %s", card->GetCardName().c_str() );
		}
		else if( bigMoneyMove.m_moveType == END_PHASE )
		{
			bigMoneyStr = Stringf( "Big Money Best move: End Phase" );
		}
		DebugAddScreenText( Vec4( 0.5f, 0.95f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::RED, Rgba8::RED, 0.f, bigMoneyStr.c_str() );
	}
	DebugAddScreenText( Vec4( 0.5f, 0.90f, 0.f, 0.f ), Vec2(), 15.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Current sim count: %i", m_totalSimCount ).c_str() );



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
	if( m_currentGameState->m_whoseMoveIsIt == PLAYER_1 )
	{

		if( m_timer.CheckAndDecrement() )
		{

			if( m_simCount < 10'000 )
			{
				m_mcts->RunSimulations( 100 );
				m_simCount += 100;
				m_totalSimCount += 100;

			}
			else
			{
				if( m_simCount >= 10'000 )
				{
					m_simCount = 0;
				}
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
		}
		else
		{

		}


// 		m_mcts->RunSimulations( 1000 );
// 		//m_mc->RunSimulations( 1000 );
	}
	else
	{
		inputMove_t bigMoneyAI = GetMoveUsingBigMoney( *m_currentGameState );
		PlayMoveIfValid( bigMoneyAI );
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
				int handIndex = moveToPlay.m_cardHandIndexToPlay;
				playerDeck->PlayCard( handIndex, m_currentGameState );
			}
		}
		else if( moveToPlay.m_moveType == BUY_MOVE )
		{
			int pileIndex = moveToPlay.m_cardIndexToBuy;
			pileData_t& pileData = m_currentGameState->m_cardPiles[pileIndex];
			pileData.m_pileSize -= 1;
			int cardCost = pileData.m_card->GetCardCost();
			//Decrement coins
			//Put card in discard pile
			playerDeck->DecrementCoins( cardCost );
			playerDeck->AddCardToDiscardPile( pileData.m_card );
			playerDeck->m_numberOfBuysAvailable--;
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
		}
		else
		{
			ERROR_AND_DIE( "INVALID PHASE" );
			return;
		}

	}

	m_mc->SetCurrentGameState( *m_currentGameState );
	m_mc->ResetPossibleMoves();

	m_mcts->UpdateGame( moveToPlay, *m_currentGameState );
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
			int cardIndex = moveToPlay.m_cardIndexToBuy;
			if( cardIndex >= 0 && cardIndex < NUMBEROFPILES )
			{
				pileData_t const& pileData = gameState.m_cardPiles[cardIndex];
				if( pileData.m_pileSize > 0 )
				{
					if( pileData.m_card->GetCardCost() <= playerDeck->GetCurrentMoney() )
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
		std::vector<CardDefinition const*> hand = playerDeck->m_hand;

		if( playerDeck->m_numberOfActionsAvailable > 0 )
		{
			if( hand.size() > 0 )
			{
				int cardIndex = moveToPlay.m_cardHandIndexToPlay;
				if( cardIndex >= 0 && cardIndex < hand.size() )
				{
					if( hand[cardIndex]->GetCardType() == ACTION_TYPE )
					{
						return true;
					}
				}
			}
		}

		return false;
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

int Game::IsGameOverForGameState( gamestate_t const& gameState )
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
			if( pileData.m_card && pileData.m_pileSize <= 0 )
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



std::vector<inputMove_t> Game::GetValidMovesAtGameState( gamestate_t const& gameState )
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
			std::vector<CardDefinition const*> const& hand = playerDeck->GetHand();
			for( size_t handIndex = 0; handIndex < hand.size(); handIndex++ )
			{
				CardDefinition const* card = hand[handIndex];
				if( card->GetCardType() == ACTION_TYPE )
				{
					inputMove_t newMove = move;
					newMove.m_moveType = PLAY_CARD;
					newMove.m_cardHandIndexToPlay = (int)handIndex;
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
				CardDefinition const* card = piles[pileIndex].m_card;
				int pileSize = piles[pileIndex].m_pileSize;
				int cardCost = card->GetCardCost();

				if( pileSize > 0 && currentMoney >= cardCost )
				{
					inputMove_t newMove = move;
					newMove.m_moveType = BUY_MOVE;
					newMove.m_cardIndexToBuy = (int)pileIndex;
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
		std::vector<CardDefinition const*> const& hand = playerDeck->GetHand();
		for( size_t handIndex = 0; handIndex < hand.size(); handIndex++ )
		{
			CardDefinition const* card = hand[handIndex];
			if( card->GetCardType() == ACTION_TYPE )
			{
				numberOfValidMoves++;
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
				CardDefinition const* card = piles[pileIndex].m_card;
				int pileSize = piles[pileIndex].m_pileSize;
				int cardCost = card->GetCardCost();

				if( pileSize > 0 && currentMoney > cardCost )
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
	gamestate_t newGameState = currentGameState;

	if( IsGameOverForGameState( currentGameState ) != GAMENOTOVER )
	{
		return newGameState;
	}
	if( IsMoveValidForGameState( move, newGameState ) )
	{
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


		if( move.m_moveType == PLAY_CARD )
		{
			if( playerDeck->m_numberOfActionsAvailable > 0 )
			{
				playerDeck->m_numberOfActionsAvailable--;
				int handIndex = move.m_cardHandIndexToPlay;
				playerDeck->PlayCard( handIndex, &newGameState );
			}
			return newGameState;
		}
		else if( move.m_moveType == BUY_MOVE )
		{
			int pileIndex = move.m_cardIndexToBuy;
			pileData_t& pileData = newGameState.m_cardPiles[pileIndex];
			pileData.m_pileSize -= 1;
			int cardCost = pileData.m_card->GetCardCost();
			//Decrement coins
			//Put card in discard pile
			playerDeck->DecrementCoins( cardCost );
			playerDeck->AddCardToDiscardPile( pileData.m_card );
			playerDeck->m_numberOfBuysAvailable--;
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
		}
		else
		{
			ERROR_AND_DIE( "INVALID PHASE" );
		}

	}

	return newGameState;
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
			newMove.m_cardIndexToBuy = (int)PROVINCE;
		}
		else if( currentMoney >= 6 )
		{
			newMove.m_cardIndexToBuy = (int)GOLD;
		}
		else if( currentMoney >= 3 )
		{
			newMove.m_cardIndexToBuy = (int)SILVER;
		}
		else
		{
			newMove.m_moveType = END_PHASE;
		}

		return newMove;
	}

}

gamestate_t Game::GetRandomInitialGameState()
{
	PlayerBoard starterDeck = PlayerBoard( &m_rand );
	std::vector<CardDefinition const*> starterCards;
	starterCards.reserve( 10 );
	CardDefinition const* copper = CardDefinition::GetCardDefinitionByType( eCards::COPPER );
	CardDefinition const* silver = CardDefinition::GetCardDefinitionByType( eCards::SILVER );
	CardDefinition const* gold = CardDefinition::GetCardDefinitionByType( eCards::GOLD );
	CardDefinition const* estate = CardDefinition::GetCardDefinitionByType( eCards::ESTATE );
	CardDefinition const* duchy = CardDefinition::GetCardDefinitionByType( eCards::DUCHY );
	CardDefinition const* province = CardDefinition::GetCardDefinitionByType( eCards::PROVINCE );
	CardDefinition const* curse = CardDefinition::GetCardDefinitionByType( eCards::CURSE );

	CardDefinition const* village = CardDefinition::GetCardDefinitionByType( eCards::Village );
	CardDefinition const* smithy = CardDefinition::GetCardDefinitionByType( eCards::Smithy );
	CardDefinition const* festival = CardDefinition::GetCardDefinitionByType( eCards::Festival );
	CardDefinition const* laboratory = CardDefinition::GetCardDefinitionByType( eCards::Laboratory );
	CardDefinition const* market = CardDefinition::GetCardDefinitionByType( eCards::Market );
	CardDefinition const* councilRoom = CardDefinition::GetCardDefinitionByType( eCards::CouncilRoom );
	CardDefinition const* witch = CardDefinition::GetCardDefinitionByType( eCards::Witch );

	//7 copper
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	starterCards.push_back( copper );
	//3 estate
	starterCards.push_back( estate );
	starterCards.push_back( estate );
	starterCards.push_back( estate );
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
	cardPiles[(int)eCards::COPPER].m_card = copper;
	cardPiles[(int)eCards::COPPER].m_pileSize = MONEYPILESIZE;
	cardPiles[(int)eCards::SILVER].m_card = silver;
	cardPiles[(int)eCards::SILVER].m_pileSize = MONEYPILESIZE;
	cardPiles[(int)eCards::GOLD].m_card = gold;
	cardPiles[(int)eCards::GOLD].m_pileSize = MONEYPILESIZE;
	cardPiles[(int)eCards::ESTATE].m_card = estate;
	cardPiles[(int)eCards::ESTATE].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::DUCHY].m_card = duchy;
	cardPiles[(int)eCards::DUCHY].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::PROVINCE].m_card = province;
	cardPiles[(int)eCards::PROVINCE].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::CURSE].m_card = curse;
	cardPiles[(int)eCards::CURSE].m_pileSize = CURSEPILESIZE;
	cardPiles[(int)eCards::Village].m_card = village;
	cardPiles[(int)eCards::Village].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Smithy].m_card = smithy;
	cardPiles[(int)eCards::Smithy].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Festival].m_card = festival;
	cardPiles[(int)eCards::Festival].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Laboratory].m_card = laboratory;
	cardPiles[(int)eCards::Laboratory].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Market].m_card = market;
	cardPiles[(int)eCards::Market].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::CouncilRoom].m_card = councilRoom;
	cardPiles[(int)eCards::CouncilRoom].m_pileSize = ACTIONPILESIZE;
	cardPiles[(int)eCards::Witch].m_card = witch;
	cardPiles[(int)eCards::Witch].m_pileSize = ACTIONPILESIZE;


	newGameState.m_whoseMoveIsIt = PLAYER_1;
	newGameState.m_isFirstMove = false;

	return newGameState;
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
