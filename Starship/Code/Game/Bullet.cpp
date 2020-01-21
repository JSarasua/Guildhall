#include "Bullet.hpp"
#include "App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
Bullet::Bullet(Game* game): Entity(game)
{}

Bullet::Bullet( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, Game* game ):
	Entity(initialPosition, initialVelocity,initialOrientationDegrees, initialAngularVelocity, game)
{
}

Bullet::~Bullet()
{

}

void Bullet::Startup()
{
}

void Bullet::Update( float deltaSeconds )
{
	if (m_game->CollidingWithWall(m_position, BULLET_COSMETIC_RADIUS, false) != 0)
	{
		LoseAllHealth();	}
	m_position = TransformPosition2D(m_position,1.f,0.f,m_velocity * deltaSeconds);

}

void Bullet::Render() const
{
	Vertex_PCU bulletVertexes[] =
	{
		Vertex_PCU( Vec3( 0.0f,-0.5f,0.f ),m_color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.5f,0.f,0.f ),Rgba8( 255,255,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,0.5f,0.f ),m_color,Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( -2.0f,0.f,0.f ),Rgba8( 255,0,0,0 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-0.5f,0.f ),m_color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,0.5f,0.f ),m_color,Vec2( 0.f,0.f ) )

	};
	Vertex_PCU::TransformVertexArray(6,bulletVertexes,1.f,m_orientationDegrees,m_position);

	g_theRenderer->DrawVertexArray(6, bulletVertexes);
}

void Bullet::ActivateBullet( Vec2 initialPosition, float initialOrientationDegrees, bool isABouncingBullet )
{
	SetAlive();
	m_position = initialPosition;
	m_orientationDegrees = initialOrientationDegrees;
	m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, BULLET_SPEED);
	m_health = BULLET_HEALTH;
	m_color = Rgba8( 255,0,0,255 );
	m_isABouncingBullet = isABouncingBullet;
	if( m_isABouncingBullet )
	{
		m_health = BOUNCING_BULLET_HEALTH;
	}
}

void Bullet::Bounce()
{
	m_orientationDegrees += m_game->m_rand.RollRandomFloatInRange(-45.f,45.f);
	m_color = Rgba8(255,255,255);
}

void Bullet::setColor( Rgba8 color )
{
	m_color = color;
}
