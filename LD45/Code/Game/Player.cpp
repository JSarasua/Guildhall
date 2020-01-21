#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game/App.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"


extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;

Rgba8 g_PlayerColors[] = 
{
	Rgba8(0,0,0),
	Rgba8(20, 50,200),
	Rgba8(255,255,255),
	Rgba8(255,0,0)
};



Player::Player( const Vec2& initialPosition, int playerID, Game* game ) :
	m_playerID(playerID),
	Entity(game)
{
	m_position.x = initialPosition.x;
	m_position.y = initialPosition.y;
	m_angularVelocity = 0.f;
	m_startingPosition = m_position;
}

void Player::Startup()
{

}

void Player::Update( float deltaSeconds )
{
	UpdateFromJoystick(deltaSeconds);
	if( m_playerID == 0 )
	{
		UpdateFromKeyboard(deltaSeconds);
	}
	AABB2 bottomBoundary(Vec2(0.f,-1.f),Vec2(WORLD_SIZE_X,0.f));
	AABB2 topBoundary(Vec2(0.f,WORLD_SIZE_Y),Vec2(WORLD_SIZE_X,WORLD_SIZE_Y + 1.f));
	AABB2 leftBoundary(Vec2(-1.f,0.f),Vec2(0.f,WORLD_SIZE_Y));
	AABB2 rightBoundary(Vec2(WORLD_SIZE_X,0.f),Vec2(WORLD_SIZE_X + 1.f,WORLD_SIZE_Y));
	PushDiscOutOfAABB2D(m_position,PLAYER_SIZE, bottomBoundary );
	PushDiscOutOfAABB2D(m_position,PLAYER_SIZE, topBoundary );
	PushDiscOutOfAABB2D(m_position,PLAYER_SIZE, leftBoundary );
	PushDiscOutOfAABB2D(m_position,PLAYER_SIZE, rightBoundary );


	Entity::Update(deltaSeconds);
}

void Player::Render() const
{
	if( !m_isDead )
	{
		
		DrawRing(m_position, PLAYER_SIZE, g_PlayerColors[m_playerID],0.3f);
	}

	/*
	AABB2 playerAABB2 = AABB2(Vec2(-PLAYER_SIZE, -PLAYER_SIZE), Vec2(PLAYER_SIZE, PLAYER_SIZE));
	Vertex_PCU playerVertexes[] =
	{
		Vertex_PCU( Vec3( playerAABB2.bottomLeft),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( Vec2(playerAABB2.topRight.x, playerAABB2.bottomLeft.y )),Rgba8( 255,255,255,255 ),Vec2( 1.f,0.f ) ),
		Vertex_PCU( Vec3( playerAABB2.topRight ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),

		Vertex_PCU( Vec3( playerAABB2.bottomLeft ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( playerAABB2.topRight ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),
		Vertex_PCU( Vec3( Vec2(playerAABB2.bottomLeft.x, playerAABB2.topRight.y) ),Rgba8( 255,255,255,255 ),Vec2( 0.f,1.f ) )

	};


	//	// Draw tank base (textured)
	//	Texture* tankBaseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/TankBase.png" );
	//	std::vector<Vertex_PCU> tankBaseVerts;
	//	AppendVertsForAABB2D( tankBaseVerts, tankLocalBounds, color );
	//	g_theRenderer->BindTexture( tankBaseTexture );
	//	g_theRenderer->DrawVertexArray( tankBaseVerts.size(), &tankBaseVerts[0] );

	Texture* baseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankBase.png" );
	Vertex_PCU::TransformVertexArray( 6, playerVertexes, 1.f, m_orientationDegrees, m_position );
	g_theRenderer->BindTexture(baseTexture);
	g_theRenderer->DrawVertexArray( 6, playerVertexes );
	*/





}

void Player::ResetPosition()
{
	m_position = m_startingPosition;
}

void Player::UpdateFromJoystick( float deltaSeconds )
{
	UNUSED(deltaSeconds);

	const XboxController& controller = g_theInput->GetXboxController( m_playerID );
	if( controller.IsConnected() )
	{
		m_isDead = false;
		if( controller.GetLeftJoystick().GetMagnitude() != 0 )
		{
			m_velocity = Vec2::MakeFromPolarDegrees( controller.GetLeftJoystick().GetAngleDegrees(), controller.GetLeftJoystick().GetMagnitude() * PLAYER_SPEED );
		}
		else if( controller.GetLeftJoystick().GetMagnitude() == 0 )
		{
			m_velocity *= 0.f;
		}
		if( controller.GetButtonState( XBOX_BUTTON_ID_START ).WasJustPressed() )
		{
			g_theApp->RestartGame();
		}
		if( controller.GetButtonState( XBOX_BUTTON_ID_BACK ).WasJustPressed() )
		{
			m_game->toggleDebugMode();
		}
	}
	else
	{
		m_isDead = true;
		ResetPosition();
	}

}

void Player::UpdateFromKeyboard( float deltaSeconds )
{
	UNUSED(deltaSeconds);






	const KeyButtonState& leftArrow = g_theInput->GetKeyStates(0x25);
	const KeyButtonState& upArrow = g_theInput->GetKeyStates(0x26);
	const KeyButtonState& rightArrow = g_theInput->GetKeyStates(0x27);
	const KeyButtonState& downArrow = g_theInput->GetKeyStates(0x28);

	const KeyButtonState& sKey = g_theInput->GetKeyStates( 'S' );
	const KeyButtonState& eKey = g_theInput->GetKeyStates( 'E' );
	const KeyButtonState& fKey = g_theInput->GetKeyStates( 'F' );
	const KeyButtonState& dKey = g_theInput->GetKeyStates( 'D' );

	if( leftArrow.IsPressed() && !rightArrow.IsPressed() )
	{
		m_velocity.x = -PLAYER_SPEED;
	}
	else if( rightArrow.IsPressed() && !leftArrow.IsPressed() )
	{
		m_velocity.x = PLAYER_SPEED;
	}
	else if( leftArrow.IsPressed() && rightArrow.IsPressed() )
	{
		m_velocity.x = 0.f;
	}


	if( downArrow.IsPressed() && !upArrow.IsPressed() )
	{
		m_velocity.y = -PLAYER_SPEED;
	}
	else if( upArrow.IsPressed() && !downArrow.IsPressed() )
	{
		m_velocity.y = PLAYER_SPEED;
	}
	else if( upArrow.IsPressed() && downArrow.IsPressed() )
	{
		m_velocity.y = 0.f;
	}
	
	if( leftArrow.WasJustReleased() || rightArrow.WasJustReleased() )
	{
		m_velocity.x = 0.f;
	}
	if( upArrow.WasJustReleased() || downArrow.WasJustReleased() )
	{
		m_velocity.y = 0.f;
	}



	if( sKey.IsPressed() && !fKey.IsPressed() )
	{
		m_velocity.x = -PLAYER_SPEED;
	}
	else if( fKey.IsPressed() && !sKey.IsPressed() )
	{
		m_velocity.x = PLAYER_SPEED;
	}
	else if( sKey.IsPressed() && fKey.IsPressed() )
	{
		m_velocity.x = 0.f;
	}

	if( dKey.IsPressed() && !eKey.IsPressed() )
	{
		m_velocity.y = -PLAYER_SPEED;
	}
	else if( eKey.IsPressed() && !dKey.IsPressed() )
	{
		m_velocity.y = PLAYER_SPEED;
	}
	else if( eKey.IsPressed() && dKey.IsPressed() )
	{
		m_velocity.y = 0.f;
	}

	if( sKey.WasJustReleased() || fKey.WasJustReleased() )
	{
		m_velocity.x = 0.f;
	}
	if( eKey.WasJustReleased() || dKey.WasJustReleased() )
	{
		m_velocity.y = 0.f;
	}






}

