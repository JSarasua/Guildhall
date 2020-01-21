#include "Asteroid.hpp"
#include "App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;

Asteroid::Asteroid(Game* game): Entity(game)
{
	RandomNumberGenerator rand;
	//1.6 inner radius
	//2.0 outer radius
	for( int asteroidIndex = 0; asteroidIndex < 16; asteroidIndex++ )
	{
		float randNum = ((float)rand.RollRandomIntInRange((int)(ASTEROID_PHYSICS_RADIUS*10),(int)(ASTEROID_COSMETIC_RADIUS*10)))/10.f;
		m_CompleteAsteroid[asteroidIndex] = Vec3::MakeFromPolarDegrees(asteroidIndex * 360.f/16.f, randNum);
	}
}

Asteroid::Asteroid( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, Game* game ) :
	Entity(initialPosition, initialVelocity,initialOrientationDegrees, initialAngularVelocity, game)
{

}

Asteroid::~Asteroid()
{

}

void Asteroid::Startup()
{
}

void Asteroid::Update( float deltaSeconds )
{
	if( m_game->CollidingWithWall( m_position, ASTEROID_COSMETIC_RADIUS, false ) != 0 )
	{
		LoseAllHealth();
	}
	m_position = TransformPosition2D( m_position, 1.f, 0.f, m_velocity * deltaSeconds );
	m_orientationDegrees += m_angularVelocity * deltaSeconds;
}

void Asteroid::Render() const
{

	Vertex_PCU asteroidVertexes[48];


	for( int asteroidIndex = 0; asteroidIndex < 16; asteroidIndex++ )
	{
		asteroidVertexes[3 * asteroidIndex].position = Vec3(0.f, 0.f, 0.f);
		asteroidVertexes[(3*asteroidIndex) + 1].position = m_CompleteAsteroid[asteroidIndex];
		if( asteroidIndex == 15 )
		{
			asteroidVertexes[(3*asteroidIndex) + 2].position = m_CompleteAsteroid[0];
		}
		else
		{
			asteroidVertexes[(3*asteroidIndex) + 2].position = m_CompleteAsteroid[asteroidIndex + 1];
		}
		asteroidVertexes[3 * asteroidIndex].tint = m_color;
		asteroidVertexes[(3*asteroidIndex) + 1].tint = m_color;
		asteroidVertexes[(3*asteroidIndex) + 2].tint = m_color;
	}
	Vertex_PCU::TransformVertexArray( 48, asteroidVertexes, 1.f, m_orientationDegrees, m_position );

	g_theRenderer->DrawVertexArray(48, asteroidVertexes);
}

void Asteroid::ActivateAsteroid()
{
	RandomNumberGenerator randNum;
	m_position.x = (float)randNum.RollRandomIntInRange(0,(int)WORLD_SIZE_X);
	m_position.y = (float)randNum.RollRandomIntInRange(0,(int)WORLD_SIZE_Y);

	m_velocity = Vec2::MakeFromPolarDegrees( (float)randNum.RollRandomIntInRange(0,360), ASTEROID_SPEED);
	m_angularVelocity = (float)randNum.RollRandomIntInRange(-200,200);
	SetAlive();
	m_health = ASTEROID_HEALTH;

	m_color = Rgba8(100,100,100,255);
}
