#pragma once
#include "Game/Game.hpp"
#include "Game/MonteCarlo.hpp"
#include "Engine/Math/AABB2.hpp"
#include "App.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Game/Player.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/World.hpp"
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


	m_gameClock = new Clock();
	m_gameClock->SetParent( Clock::GetMaster() );

	g_theRenderer->Setup( m_gameClock );

	//m_mcts = new MonteCarlo();
	//m_mcts->Startup( CIRCLEPLAYER );
	CardDefinition::InitializeCards();
	InitializeGameState();
}

void Game::Shutdown()
{
	//m_mcts->Shutdown();
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

	//Render Player 2s hand at top of screen

	std::vector<CardDefinition const*> player1Hand = m_currentGameState->m_player1Deck.GetHand();
	std::vector<CardDefinition const*> player2Hand = m_currentGameState->m_player2Deck.GetHand();

	std::vector<CardDefinition const*> player1PlayArea = m_currentGameState->m_player1Deck.GetPlayArea();
	std::vector<CardDefinition const*> player2PlayArea = m_currentGameState->m_player2Deck.GetPlayArea();

	pileData_t const* piles = m_currentGameState->m_cardPiles;

	AABB2 gameBoard = DebugGetScreenBounds();
	Vec2 gameDims = gameBoard.GetDimensions();
	//gameDims.y = gameDims.x;
	AABB2 gameDataArea = gameBoard;
	AABB2 deckDataArea = gameBoard;
	gameDims *= 0.75f;
	gameBoard.SetDimensions( gameDims );

	gameDataArea = gameDataArea.GetBoxAtLeft( 1.f / 7.f );
	Vec2 phasePos = gameDataArea.GetPointAtUV( Vec2( 0.5f, 0.9f ) );
	Vec2 coinPos = gameDataArea.GetPointAtUV( Vec2( 0.5f, 0.5f ) );
	Vec2 buysPos = gameDataArea.GetPointAtUV( Vec2( 0.5f, 0.4f ) );
	Vec2 actionsPos = gameDataArea.GetPointAtUV( Vec2( 0.5f, 0.3f ) );

	deckDataArea = deckDataArea.GetBoxAtRight( 1.f / 7.f );
	Vec2 player2DeckPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.8f ) );
	Vec2 player2DiscardPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.7f ) );
	Vec2 player2VPPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.6f ) );
	Vec2 player1DeckPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.3f ) );
	Vec2 player1DiscardPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.2f ) );
	Vec2 player1VPPos = deckDataArea.GetPointAtUV( Vec2( 0.5f, 0.1f ) );

	Deck const* playerDeck = nullptr;
	if( m_currentGameState->m_whoseMoveIsIt == PLAYER_1 )
	{
		playerDeck = &m_currentGameState->m_player1Deck;
	}
	else
	{
		playerDeck = &m_currentGameState->m_player2Deck;
	}

	int currentCoins = playerDeck->m_currentCoins;
	int buysAmount = m_currentGameState->m_numberOfBuysAvailable;
	int actionsAmount = m_currentGameState->m_numberOfActionsAvailable;

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


	int player1DeckSize = (int)m_currentGameState->m_player1Deck.GetDeckSize();
	int player1DiscardSize = (int)m_currentGameState->m_player1Deck.GetDiscardSize();
	int player1VPCount = m_currentGameState->m_player1Deck.GetCurrentVictoryPoints();
	int player2DeckSize = (int)m_currentGameState->m_player2Deck.GetDeckSize();
	int player2DiscardSize = (int)m_currentGameState->m_player2Deck.GetDiscardSize();
	int player2VPCount = m_currentGameState->m_player2Deck.GetCurrentVictoryPoints();
	std::string player2DeckStr = Stringf( "Deck Size: %i", player2DeckSize );
	std::string player2DiscardStr = Stringf( "Discard Size: %i", player2DiscardSize );
	std::string player1DeckStr = Stringf( "Deck Size: %i", player1DeckSize );
	std::string player1DiscardStr = Stringf( "Discard Size: %i", player1DiscardSize );

	std::string player1VPStr = Stringf( "VP: %i", player1VPCount );
	std::string player2VPStr = Stringf( "VP: %i", player2VPCount );

	DebugAddScreenText( Vec4( Vec2(), player2DeckPos ), Vec2( 0.5f, 0.5f ), 12.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, player2DeckStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), player2DiscardPos ), Vec2( 0.5f, 0.5f ), 12.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, player2DiscardStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), player1DeckPos ), Vec2( 0.5f, 0.5f ), 12.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, player1DeckStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), player1DiscardPos ), Vec2( 0.5f, 0.5f ), 12.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, player1DiscardStr.c_str() );

	DebugAddScreenText( Vec4( Vec2(), phasePos ), Vec2( 0.5f, 0.5f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, phaseStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), coinPos ), Vec2( 0.5f, 0.5f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, coinStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), buysPos ), Vec2( 0.5f, 0.5f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, buysStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), actionsPos ), Vec2( 0.5f, 0.5f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, actionsStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), player1VPPos ), Vec2( 0.5f, 0.5f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, player1VPStr.c_str() );
	DebugAddScreenText( Vec4( Vec2(), player2VPPos ), Vec2( 0.5f, 0.5f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, player2VPStr.c_str() );

	AABB2 carvingBoard = gameBoard;

	AABB2 player2HandAABB = carvingBoard.CarveBoxOffTop( 1.f / 5.f );
	AABB2 player2PlayAreaAABB = carvingBoard.CarveBoxOffTop( 1.f / 4.f );
	AABB2 pilesArea = carvingBoard.CarveBoxOffTop( 1.f / 3.f );
	AABB2 player1PlayAreaAABB = carvingBoard.CarveBoxOffTop( 1.f / 2.f );
	AABB2 player1HandAABB = carvingBoard;

	AABB2 carvingPlayer2Hand = player2HandAABB;
	DebugAddScreenAABB2( player2HandAABB, Rgba8::GREEN, 0.f );
	DebugAddScreenAABB2( player2PlayAreaAABB, Rgba8::RED, 0.f );
	DebugAddScreenAABB2( pilesArea, Rgba8::CYAN, 0.f );
	DebugAddScreenAABB2( player1PlayAreaAABB, Rgba8::RED, 0.f );
	DebugAddScreenAABB2(player1HandAABB, Rgba8::GREEN, 0.f );

	Vec4 debugCenter = Vec4( Vec2(), player2HandAABB.GetCenter() );
	//DebugAddScreenText( debugCenter, Vec2( 0.5f, 0.5f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, "Hello" );
	for( size_t player2HandIndex = 0; player2HandIndex < player2Hand.size(); player2HandIndex++ )
	{
		float player2HandSize = (float)player2Hand.size();
		float carvingNumber = player2HandSize - (float)player2HandIndex;
		AABB2 cardArea = carvingPlayer2Hand.CarveBoxOffLeft( 1.f / carvingNumber );
		CardDefinition const* card = player2Hand[player2HandIndex];
		Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
		std::string cardName = card->GetCardName();
		DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, cardName.c_str() );
	}

	for( size_t player2PlayAreaIndex = 0; player2PlayAreaIndex < player2PlayArea.size(); player2PlayAreaIndex++ )
	{
		float player2PlayAreaSize = (float)player2PlayArea.size();
		float carvingNumber = player2PlayAreaSize - (float)player2PlayAreaIndex;
		AABB2 cardArea = player2PlayAreaAABB.CarveBoxOffLeft( 1.f / carvingNumber );
		CardDefinition const* card = player2PlayArea[player2PlayAreaIndex];
		Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
		std::string cardName = card->GetCardName();
		DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, cardName.c_str() );
	}

	for( size_t pilesIndex = 0; pilesIndex < NUMBEROFPILES; pilesIndex++ )
	{
		float pileSize = (float)NUMBEROFPILES;
		float carvingNumber = pileSize - (float)pilesIndex;
		AABB2 cardArea = pilesArea.CarveBoxOffLeft( 1.f / carvingNumber );
		CardDefinition const* card = piles[pilesIndex].m_card;
		Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
		Vec4 cardCostPos = Vec4( Vec2(), cardArea.maxs );
		Vec4 cardCoinsPos = Vec4( Vec2(), cardArea.mins );
		Vec4 cardVPsPos = Vec4( Vec2(), Vec2( cardArea.maxs.x, cardArea.mins.y ) );
		Vec4 pileSizePos = Vec4( Vec2(), Vec2( cardArea.mins.x, cardArea.maxs.y ) );
		std::string cardName = card->GetCardName();
		std::string cardCost = Stringf("Cost: %i", card->GetCardCost() );
		std::string cardCoins = Stringf("Coins: %i", card->GetCoins() );
		std::string cardVPs = Stringf("VPs: %i", card->GetCardVPs() );
		std::string pileCount = Stringf("Count: %i", piles[pilesIndex].m_pileSize );
		DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 10.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, cardName.c_str() );
		DebugAddScreenText( cardCostPos, Vec2( 1.1f, 1.1f ), 10.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, cardCost.c_str() );
		DebugAddScreenText( cardCoinsPos, Vec2( -0.1f, -0.1f ), 10.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, cardCoins.c_str() );
		DebugAddScreenText( cardVPsPos, Vec2( 1.1f, -0.1f ), 10.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, cardVPs.c_str() );
		DebugAddScreenText( pileSizePos, Vec2( -0.1f, 1.1f ), 10.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, pileCount.c_str() );
	}

	for( size_t player1PlayAreaIndex = 0; player1PlayAreaIndex < player1PlayArea.size(); player1PlayAreaIndex++ )
	{
		float player1PlayAreaSize = (float)player1PlayArea.size();
		float carvingNumber = player1PlayAreaSize - (float)player1PlayAreaIndex;
		AABB2 cardArea = player1PlayAreaAABB.CarveBoxOffLeft( 1.f / carvingNumber );
		CardDefinition const* card = player1PlayArea[player1PlayAreaIndex];
		Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
		std::string cardName = card->GetCardName();
		DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, cardName.c_str() );
	}

	for( size_t player1HandIndex = 0; player1HandIndex < player1Hand.size(); player1HandIndex++ )
	{
		float player1HandSize = (float)player1Hand.size();
		float carvingNumber = player1HandSize - (float)player1HandIndex;
		AABB2 cardArea = player1HandAABB.CarveBoxOffLeft( 1.f / carvingNumber );
		CardDefinition const* card = player1Hand[player1HandIndex];
		Vec4 cardPos = Vec4( Vec2(), cardArea.GetCenter() );
		std::string cardName = card->GetCardName();
		DebugAddScreenText( cardPos, Vec2( 0.5f, 0.5f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, cardName.c_str() );
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


	Deck starterDeck = Deck( &m_rand );
	std::vector<CardDefinition const*> starterCards;
	starterCards.reserve( 10 );
	CardDefinition const* copper = CardDefinition::GetCardDefinitionByType( eCards::COPPER );
	CardDefinition const* silver = CardDefinition::GetCardDefinitionByType( eCards::SILVER );
	CardDefinition const* gold = CardDefinition::GetCardDefinitionByType( eCards::GOLD );
	CardDefinition const* estate = CardDefinition::GetCardDefinitionByType( eCards::ESTATE );
	CardDefinition const* duchy = CardDefinition::GetCardDefinitionByType( eCards::DUCHY );
	CardDefinition const* province = CardDefinition::GetCardDefinitionByType( eCards::PROVINCE );
	CardDefinition const* curse = CardDefinition::GetCardDefinitionByType( eCards::CURSE );
	

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
	m_currentGameState->m_player1Deck = starterDeck;
	m_currentGameState->m_player2Deck = starterDeck;
	
	m_currentGameState->m_player1Deck.ShuffleDeck();
	m_currentGameState->m_player2Deck.ShuffleDeck();

	m_currentGameState->m_player1Deck.Draw5();
	m_currentGameState->m_player2Deck.Draw5();

	m_currentGameState->m_currentPhase = ACTION_PHASE;

	pileData_t* cardPiles = m_currentGameState->m_cardPiles;
	cardPiles[(int)eCards::COPPER].m_card = copper;
	cardPiles[(int)eCards::COPPER].m_pileSize = MONEPILESIZE;
	cardPiles[(int)eCards::SILVER].m_card = silver;
	cardPiles[(int)eCards::SILVER].m_pileSize = MONEPILESIZE;
	cardPiles[(int)eCards::GOLD].m_card = gold;
	cardPiles[(int)eCards::GOLD].m_pileSize = MONEPILESIZE;
	cardPiles[(int)eCards::ESTATE].m_card = estate;
	cardPiles[(int)eCards::ESTATE].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::DUCHY].m_card = duchy;
	cardPiles[(int)eCards::DUCHY].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::PROVINCE].m_card = province;
	cardPiles[(int)eCards::PROVINCE].m_pileSize = VPPileSize;
	cardPiles[(int)eCards::CURSE].m_card = curse;
	cardPiles[(int)eCards::CURSE].m_pileSize = CURSEPILESIZE;

	m_currentGameState->m_whoseMoveIsIt = PLAYER_1;


// 	m_mcts->Shutdown();
// 	m_mcts->Startup( g_WhoStarts );
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
// 	const KeyButtonState& f5Key = g_theInput->GetKeyStates( F5_KEY );
// 	const KeyButtonState& f6Key = g_theInput->GetKeyStates( F6_KEY );
// 	const KeyButtonState& f7Key = g_theInput->GetKeyStates( F7_KEY );
// 	const KeyButtonState& f8Key = g_theInput->GetKeyStates( F8_KEY );
// 	const KeyButtonState& f11Key = g_theInput->GetKeyStates( F11_KEY );
	const KeyButtonState& num1Key = g_theInput->GetKeyStates( '1' );
	const KeyButtonState& num2Key = g_theInput->GetKeyStates( '2' );
	const KeyButtonState& num3Key = g_theInput->GetKeyStates( '3' );
 	const KeyButtonState& num4Key = g_theInput->GetKeyStates( '4' );
 	const KeyButtonState& num5Key = g_theInput->GetKeyStates( '5' );
 	const KeyButtonState& num6Key = g_theInput->GetKeyStates( '6' );
 	const KeyButtonState& num7Key = g_theInput->GetKeyStates( '7' );
 	const KeyButtonState& num8Key = g_theInput->GetKeyStates( '8' );
	const KeyButtonState& num9Key = g_theInput->GetKeyStates( '9' );
// 	const KeyButtonState& num0Key = g_theInput->GetKeyStates( '0' );
// 	const KeyButtonState& lBracketKey = g_theInput->GetKeyStates( LBRACKET_KEY );
// 	const KeyButtonState& rBracketKey = g_theInput->GetKeyStates( RBRACKET_KEY );
 	const KeyButtonState& rKey = g_theInput->GetKeyStates( 'R' );
// 	const KeyButtonState& fKey = g_theInput->GetKeyStates( 'F' );
// 	const KeyButtonState& tKey = g_theInput->GetKeyStates( 'T' );
// 	const KeyButtonState& gKey = g_theInput->GetKeyStates( 'G' );
// 	const KeyButtonState& hKey = g_theInput->GetKeyStates( 'H' );
// 	//const KeyButtonState& yKey = g_theInput->GetKeyStates( 'Y' );
// 	const KeyButtonState& qKey = g_theInput->GetKeyStates( 'Q' );
// 	const KeyButtonState& vKey = g_theInput->GetKeyStates( 'V' );
// 	const KeyButtonState& bKey = g_theInput->GetKeyStates( 'B' );
// 	const KeyButtonState& nKey = g_theInput->GetKeyStates( 'N' );
// 	const KeyButtonState& mKey = g_theInput->GetKeyStates( 'M' );
// 	const KeyButtonState& uKey = g_theInput->GetKeyStates( 'U' );
// 	const KeyButtonState& iKey = g_theInput->GetKeyStates( 'I' );
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
/*		m_mcts->RunSimulations( 1000 );*/

	}
	if( f2Key.WasJustPressed() )
	{
		//inputMove_t move = m_mcts->GetBestMove();

/*		PlayMoveIfValid( move.m_move );*/
	}
	if( f3Key.WasJustPressed() )
	{
		/*m_mcts->RunSimulations( 1 );*/
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
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 0;
		PlayMoveIfValid( move );
	}
	if( num2Key.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 1;
		PlayMoveIfValid( move );
	}
	if( num3Key.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 2;
		PlayMoveIfValid( move );
	}
	if( num4Key.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 3;
		PlayMoveIfValid( move );
	}
	if( num5Key.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 4;
		PlayMoveIfValid( move );
	}
	if( num6Key.WasJustPressed() )
	{
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 5;
		PlayMoveIfValid( move );
	}
	if( num7Key.WasJustPressed() )
	{		
		inputMove_t move;
		move.m_moveType = BUY_MOVE;
		move.m_whoseMoveIsIt = m_currentGameState->m_whoseMoveIsIt;
		move.m_cardIndexToBuy = 6;
		PlayMoveIfValid( move );
	}
	if( num8Key.WasJustPressed() )
	{
		//PlayMoveIfValid( BOTTOMMIDDLE );
	}
	if( num9Key.WasJustPressed() )
	{
		//PlayMoveIfValid( BOTTOMRIGHT );
	}
	if( rKey.WasJustPressed() )
	{
		InitializeGameState();
		//reset ai
	}
}



void Game::PlayMoveIfValid( inputMove_t const& moveToPlay )
{
	if( IsGameOver() != 0 )
	{
		return;
	}
	if( IsMoveValid( moveToPlay ) )
	{
		Deck* playerDeck = nullptr;
		int nextPlayersTurn = 0;
		if( moveToPlay.m_whoseMoveIsIt == PLAYER_1 )
		{
			playerDeck = &m_currentGameState->m_player1Deck;
			nextPlayersTurn = PLAYER_2;
		}
		else
		{
			playerDeck = &m_currentGameState->m_player2Deck;
			nextPlayersTurn = PLAYER_1;
		}


		if( moveToPlay.m_moveType == PLAY_CARD )
		{
			//IMPLEMENT LATER
			m_currentGameState->m_numberOfActionsAvailable--;
			return;
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
			m_currentGameState->m_numberOfBuysAvailable--;
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
				m_currentGameState->m_numberOfActionsAvailable = 1;
				m_currentGameState->m_numberOfBuysAvailable = 1;
				playerDeck->m_currentCoins = 0;
			}
		}
		else
		{
			ERROR_AND_DIE( "INVALID PHASE" );
			return;
		}

	}
// 
// 	m_mcts->UpdateGame( move, m_currentGameState );
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

	Deck const* playerDeck = nullptr;
	if( whoseMove == PLAYER_1 )
	{
		playerDeck = &gameState.m_player1Deck;
	}
	else
	{
		playerDeck = &gameState.m_player2Deck;
	}

	if( moveType == BUY_MOVE )
	{
		if( gameState.m_numberOfBuysAvailable > 0 )
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

		if( gameState.m_numberOfActionsAvailable > 0 )
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
	if( provinceData.m_pileSize == 0 )
	{
		isGameOver = true;
	}
	else
	{
		int emptyPileCount = 0;
		for( size_t pileIndex = 0; pileIndex < NUMBEROFPILES; pileIndex++ )
		{
			pileData_t const& pileData = gameState.m_cardPiles[pileIndex];
			if( pileData.m_card && pileData.m_pileSize == 0 )
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
		int player1Score = gameState.m_player1Deck.GetCurrentVictoryPoints();
		int player2Score = gameState.m_player2Deck.GetCurrentVictoryPoints();
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
	return 0;
}

int Game::IsGameOver()
{
	return IsGameOverForGameState( *m_currentGameState );
}



std::vector<inputMove_t> Game::GetValidMovesAtGameState( gamestate_t const& gameState )
{
	UNIMPLEMENTED();
	std::vector<inputMove_t> validMoves;
// 
// 	int const* gameArray = gameState.gameArray;
// 	int stateIndex = 0;
// 	while ( stateIndex < 9 )
// 	{
// 		if( gameArray[stateIndex] == 0 )
// 		{
// 			validMoves.push_back( stateIndex );
// 		}
// 		stateIndex++;
// 	}

	return validMoves;
}

int Game::GetNumberOfValidMovesAtGameState( gamestate_t const& gameState )
{
	UNIMPLEMENTED();
	int numberOfValidMoves = 0;
// 
// 	int const* gameArray = gameState.gameArray;
// 	int stateIndex = 0;
// 	while( stateIndex < 9 )
// 	{
// 		if( gameArray[stateIndex] == 0 )
// 		{
// 			numberOfValidMoves++;
// 		}
// 		stateIndex++;
// 	}

	return numberOfValidMoves;
}

gamestate_t Game::GetGameStateAfterMove( gamestate_t const& currentGameState, inputMove_t const& move )
{
	UNIMPLEMENTED();
	gamestate_t newGameState = currentGameState;
// 	int whoseMoveIsIt = newGameState.m_whoseMoveIsIt;
// 	int moveToMake = move.m_move;
// 	if( IsMoveValidForGameState( move.m_move, newGameState ) )
// 	{
// 		newGameState.gameArray[moveToMake] = whoseMoveIsIt;
// 
// 		if( whoseMoveIsIt == PLAYER_1 )
// 		{
// 			newGameState.m_whoseMoveIsIt = PLAYER_2;
// 		}
// 		else
// 		{
// 			newGameState.m_whoseMoveIsIt = PLAYER_1;
// 		}
// 	}

	return newGameState;
}

inputMove_t Game::GetRandomMoveAtGameState( gamestate_t const& currentGameState )
{
	UNIMPLEMENTED();

// 	std::vector<int> validMoves = GetValidMovesAtGameState( currentGameState );
// 
// 	int randIndex = m_rand.RollRandomIntInRange( 0, (int)validMoves.size() - 1 );
// 	int randMove = validMoves[randIndex];
// 	inputMove_t randMoveStruct = inputMove_t(randMove);
// 
// 	return randMoveStruct;
	return inputMove_t();
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
