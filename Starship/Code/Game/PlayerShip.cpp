#include "Game/PlayerShip.hpp"
#include "App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;
PlayerShip::PlayerShip(Game* game): Entity(game)
{
	m_health = PLAYER_SHIP_HEALTH;
}

PlayerShip::PlayerShip( Vec2 initialPosition, Game* game ): Entity(game)
{
	m_position.x = initialPosition.x;
	m_position.y = initialPosition.y;
}

PlayerShip::~PlayerShip()
{

}

void PlayerShip::Startup()
{
	m_angularVelocity = PLAYER_SHIP_TURN_SPEED;
	m_color = Rgba8( 132,156,169,255 );

}


void PlayerShip::Update( float deltaSeconds )
{

	UpdateFromJoystick(deltaSeconds);
	CheckCollidingWithWall();


	//Moves center position
	m_position = TransformPosition2D(m_position,1.f,0.f,m_velocity * deltaSeconds);

	if( g_theApp->IsUpArrowPressed() )
	{
		IncreaseVelocity(PLAYER_SHIP_ACCELERATION* deltaSeconds);
	}
	

	if( g_theApp->IsLeftArrowPressed() )
	{
		m_orientationDegrees += m_angularVelocity * deltaSeconds;
	}
	else if( g_theApp->IsRightArrowPressed() )
	{
		m_orientationDegrees -= m_angularVelocity * deltaSeconds;
	}
}

void PlayerShip::Render() const
{
	
	Vertex_PCU playerShipVertexes[] = 
	{
		Vertex_PCU( Vec3( 0.0f,-1.0f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,0.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( -2.0f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( -2.0f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -2.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( -2.0f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 2.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( -2.0f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( 0.f,-2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 2.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) )
	};
	Vertex_PCU::TransformVertexArray(21, playerShipVertexes, 1.f, m_orientationDegrees, m_position);
	
	g_theRenderer->DrawVertexArray(21, playerShipVertexes);

}

void PlayerShip::Lose1Health()
{
	Entity::Lose1Health();
	if( !IsAlive() )
	{
		m_lives -= 1;
	}
}

void PlayerShip::LoseAllHealth()
{
	Entity::LoseAllHealth();
	m_lives -= 1;
}

float PlayerShip::getOrientationDegrees()
{
	return m_orientationDegrees;
}

void PlayerShip::ResurrectPlayer()
{
	if( m_lives != 0 )
	{
		m_position = Vec2( WORLD_CENTER_X, WORLD_CENTER_Y );
		SetAlive();
		m_health = PLAYER_SHIP_HEALTH;
		m_velocity = Vec2( 0.f, 0.f );
		m_orientationDegrees = 0.f;
	}

}

Vec2 PlayerShip::GetNosePosition()
{
	return m_position + Vec2::MakeFromPolarDegrees(m_orientationDegrees, 1.f);
}

int PlayerShip::GetLivesCount()
{
	return m_lives;
}

void PlayerShip::IncreaseVelocity( float deltaSpeedUp )
{
		m_velocity = TransformPosition2D(m_velocity,1.f,0.f,Vec2::MakeFromPolarDegrees(m_orientationDegrees, deltaSpeedUp));
}

void PlayerShip::UpdateFromJoystick( float deltaSeconds )
{
	UNUSED(deltaSeconds);

	const XboxController& controller = g_theInput->GetXboxController(0);
	if( !controller.m_isConnected )
	{
		return;
	}
	const AnalogJoystick& joystick = controller.GetLeftJoystick();
	if( joystick.GetMagnitude() > 0 )
	{
		m_velocity = TransformPosition2D(m_velocity, 1.f, 0.f, Vec2::MakeFromPolarDegrees(joystick.GetAngleDegrees(), PLAYER_SHIP_ACCELERATION * deltaSeconds* joystick.GetMagnitude()));
		//m_velocity = Vec2::MakeFromPolarDegrees(joystick.GetAngleDegrees(), joystick.GetMagnitude() * 10.f);
		m_orientationDegrees = joystick.GetAngleDegrees();
	}



}

void PlayerShip::CheckCollidingWithWall()
{
	switch( m_game->CollidingWithWall( m_position, PLAYER_SHIP_PHYSICS_RADIUS, true ) )
	{
	case 0:
		break;
	case 1:
		//Collided with Right Wall
		if( m_velocity.x > 0.f )
		{
			m_velocity.x = -m_velocity.x;
		}

		break;
	case 2:
		//Collided with Top Wall
		if( m_velocity.y > 0.f )
		{
			m_velocity.y = -m_velocity.y;
		}
		break;
	case 3:
		//Collided with Left Wall
		if( m_velocity.x < 0.f )
		{
			m_velocity.x = -m_velocity.x;
		}
		break;
	case 4:
		//Collided with Bottom Wall
		if( m_velocity.y < 0.f )
		{
			m_velocity.y = -m_velocity.y;
		}
		break;
	}
}
