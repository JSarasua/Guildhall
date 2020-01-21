#include "Debris.hpp"
#include "App.hpp"
#include "Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;

Debris::Debris( Rgba8 color, Vec2 initialVelocity, Game* game ): 
	Entity(game), m_color(color)
{

}

Debris::Debris( Game* game ) : 
	Entity(game)
{

}

void Debris::Startup()
{

}

void Debris::Update( float deltaSeconds )
{
	static float sixtyFPSDecrement = 0.f;
	sixtyFPSDecrement += deltaSeconds;


	m_position = TransformPosition2D(m_position,1.f,0.f,m_velocity * deltaSeconds);
	m_orientationDegrees += m_angularVelocity*deltaSeconds;
	if( m_color.a > 0 && sixtyFPSDecrement > 1.f/60.f)
	{
		m_color.a -= (unsigned char)(60.f * sixtyFPSDecrement);
		sixtyFPSDecrement = 0.f;
	}

	if( m_color.a <= 0 )
	{
		LoseAllHealth();
	}

	if( m_game->CollidingWithWall( m_position, DEBRIS_COSMETIC_RADIUS, false ) != 0 )
	{
		LoseAllHealth();
	}

}

void Debris::Render() const
{
	Vertex_PCU debrisVertexes[] =
	{
		Vertex_PCU( Vec3( 0.0f,-0.5f,0.f ),m_color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.f,0.f ),m_color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,0.5f,0.f ),m_color,Vec2( 0.f,0.f ) )
	};
	Vertex_PCU::TransformVertexArray( 3, debrisVertexes, 1.f, m_orientationDegrees, m_position );

	g_theRenderer->DrawVertexArray( 3, debrisVertexes );
}

void Debris::ActivateDebris( Rgba8 color, Vec2 initialVelocity, Vec2 initialPosition )
{
	SetAlive();
	m_angularVelocity = m_game->m_rand.RollRandomFloatInRange(-300.f,300.f);
	Vec2 randVelocity = Vec2(m_game->m_rand.RollRandomFloatInRange(-10.f,10.f), m_game->m_rand.RollRandomFloatInRange(-10.f,10.f));
	m_velocity = (initialVelocity * 0.5) + randVelocity;
	m_position = initialPosition;
	m_color = color;
	m_color.a = 128;
	m_cosmeticRadius = DEBRIS_COSMETIC_RADIUS;
}

