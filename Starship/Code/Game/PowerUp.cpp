#include "PowerUp.hpp"
#include "App.hpp"
#include "Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "GameCommon.hpp"
//#include "CommonColors.hpp"


extern App* g_theApp;
extern RenderContext* g_theRenderer;

PowerUp::PowerUp( Game* game ): Entity(game)
{
	m_physicsRadius = POWERUP_PHYSICALRADIUS;
}

void PowerUp::Startup()
{

}

void PowerUp::Update( float deltaSeconds )
{
	m_position = TransformPosition2D( m_position, 1.f, 0.f, m_velocity * deltaSeconds );
	m_orientationDegrees += m_angularVelocity * deltaSeconds;

}

void PowerUp::Render() const
{
	DrawRing(m_position, POWERUP_PHYSICALRADIUS, m_color, 1.f);
}

void PowerUp::ActivatePowerUp( Vec2 initialPosition, int powerUp )
{
	SetAlive();
	m_position = initialPosition;
	m_WhichPowerUp = powerUp;
	m_orientationDegrees = 0.f;
	m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees, POWERUP_SPEED );

	switch( m_WhichPowerUp )
	{
	case RAPID_FIRE:
		m_color = Rgba8( 250, 128, 114 );
		break;
	case SCATTER_SHOT:
		//Scattershot;
		m_color = Rgba8( 255, 0, 255 );

		break;
	case BOUNCING_BULLET:
		//Bouncing Bullets
		m_color = Rgba8( 148, 0, 211 );
		break;
	case 4:
		//Nuke
		m_color = Rgba8( 46, 139, 87 );
		break;
	default:
		m_color = Rgba8( 0, 191, 255 );
		break;
	}
}

int PowerUp::GetPowerUp()
{
	return m_WhichPowerUp;
}

