#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game/App.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;

Player::Player( const Vec2& initialPosition, Game* game ) :
	Entity(game)
{
	m_position.x = initialPosition.x;
	m_position.y = initialPosition.y;
	m_angularVelocity = 0.f;
	m_physicsRadius = PLAYER_SIZE;
}

void Player::Startup()
{

}

void Player::Update( float deltaSeconds )
{
	CapOrientationDegrees();
	UpdateFromJoystick( deltaSeconds );
	UpdateFromKeyboard( deltaSeconds );

	m_position = TransformPosition2D(m_position, 1.f, 0.f, m_velocity * deltaSeconds);
}

void Player::Render() const
{
	AABB2 playerAABB2 = AABB2(Vec2(-PLAYER_SIZE, -PLAYER_SIZE), Vec2(PLAYER_SIZE, PLAYER_SIZE));
	Vertex_PCU playerVertexes[] =
	{
	Vertex_PCU( Vec3( playerAABB2.mins),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
	Vertex_PCU( Vec3( Vec2(playerAABB2.maxs.x, playerAABB2.mins.y )),Rgba8( 255,255,255,255 ),Vec2( 1.f,0.f ) ),
	Vertex_PCU( Vec3( playerAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),

	Vertex_PCU( Vec3( playerAABB2.mins ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
	Vertex_PCU( Vec3( playerAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),
	Vertex_PCU( Vec3( Vec2(playerAABB2.mins.x, playerAABB2.maxs.y) ),Rgba8( 255,255,255,255 ),Vec2( 0.f,1.f ) )

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


	AABB2 playerCannonAABB2 = AABB2( Vec2( -PLAYER_SIZE, -PLAYER_SIZE ), Vec2( PLAYER_SIZE, PLAYER_SIZE ) );
	Vertex_PCU playerCannonVertexes[] =
	{
		Vertex_PCU( Vec3( playerCannonAABB2.mins ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( Vec2( playerCannonAABB2.maxs.x, playerCannonAABB2.mins.y ) ),Rgba8( 255,255,255,255 ),Vec2( 1.f,0.f ) ),
		Vertex_PCU( Vec3( playerCannonAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),

		Vertex_PCU( Vec3( playerCannonAABB2.mins ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( playerCannonAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),
		Vertex_PCU( Vec3( Vec2( playerCannonAABB2.mins.x, playerCannonAABB2.maxs.y ) ),Rgba8( 255,255,255,255 ),Vec2( 0.f,1.f ) )
	};
	
	Texture* cannonTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankTop.png" );
	Vertex_PCU::TransformVertexArray( 6, playerCannonVertexes, 1.f, m_cannonOrientation, m_position );
	g_theRenderer->BindTexture( cannonTexture );
	g_theRenderer->DrawVertexArray( 6, playerCannonVertexes );

	if( g_theApp->GetDebugGameMode() )
	{
		g_theRenderer->BindTexture(nullptr);
		DrawRing(m_position, PLAYER_SIZE, Rgba8(255,0,255), 0.01f);
		DrawRing(m_position, PLAYER_PHYSICS_RADIUS, Rgba8(0,255,255), 0.01f);
	}
}



void Player::UpdateFromJoystick( float deltaSeconds )
{
	const XboxController& controller = g_theInput->GetXboxController( 0 );
	if( controller.IsConnected() )
	{
		if( controller.GetLeftJoystick().GetMagnitude() != 0 )
		{
			UpdatePlayerOrientationAndVelocity( controller.GetLeftJoystick().GetAngleDegrees(), controller.GetLeftJoystick().GetMagnitude(), deltaSeconds);
		}
		else if( controller.GetLeftJoystick().GetMagnitude() == 0 )
		{
			m_velocity *= 0.f;
		}
		if( controller.GetRightJoystick().GetMagnitude() != 0 )
		{
			m_cannonOrientation = GetTurnedToward( m_cannonOrientation, controller.GetRightJoystick().GetAngleDegrees(), PLAYER_TURN_SPEED * deltaSeconds);

		}
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

void Player::UpdatePlayerOrientationAndVelocity( float joystickOrientation, float joystickMagnitude, float deltaSeconds )
{
	m_orientationDegrees = GetTurnedToward( m_orientationDegrees, joystickOrientation, PLAYER_TURN_SPEED * deltaSeconds);
	m_velocity  = Vec2::MakeFromPolarDegrees(m_orientationDegrees, joystickMagnitude * PLAYER_SPEED);
}

void Player::CapOrientationDegrees()
{
	if( m_orientationDegrees > 360.f )
	{
		m_orientationDegrees -= 360.f;
	}
	if( m_orientationDegrees < -360.f )
	{
		m_orientationDegrees += 360.f;
	}

	if( m_cannonOrientation > 360.f )
	{
		m_cannonOrientation -= 360.f;
	}
	if( m_cannonOrientation < -360.f )
	{
		m_cannonOrientation += 360.f;
	}
}

