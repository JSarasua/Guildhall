#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"



extern App* g_theApp;
extern RenderContext* g_theRenderer;

Bullet::Bullet( const Vec2& initialPosition, Game* game ) :
	Entity(game)
{
	m_position = initialPosition;
}

Bullet::Bullet( const Vec2& initialPosition, float orientationDegrees, EntityType type, EntityFaction faction, Game* game ) :
		Entity(type, faction, game)
{
	m_position = initialPosition;
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	m_orientationDegrees = orientationDegrees;
	m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, BULLET_SPEED);
}

void Bullet::Startup()
{

}

void Bullet::Update( float deltaSeconds )
{
	Entity::Update(deltaSeconds);
}

void Bullet::Render() const
{
	AABB2 bulletAABB2 = AABB2( Vec2( -m_cosmeticRadius, -m_cosmeticRadius ), Vec2( m_cosmeticRadius, m_cosmeticRadius ) );
	Vertex_PCU bulletVertexes[] =
	{
		Vertex_PCU( Vec3( bulletAABB2.mins ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( Vec2( bulletAABB2.maxs.x, bulletAABB2.mins.y ) ),Rgba8( 255,255,255,255 ),Vec2( 1.f,0.f ) ),
		Vertex_PCU( Vec3( bulletAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),

		Vertex_PCU( Vec3( bulletAABB2.mins ),Rgba8( 255,255,255,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( bulletAABB2.maxs ),Rgba8( 255,255,255,255 ),Vec2( 1.f,1.f ) ),
		Vertex_PCU( Vec3( Vec2( bulletAABB2.mins.x, bulletAABB2.maxs.y ) ),Rgba8( 255,255,255,255 ),Vec2( 0.f,1.f ) )

	};


	Texture* baseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Bullet.png" );
	Vertex_PCU::TransformVertexArray( 6, bulletVertexes, 1.f, m_orientationDegrees, m_position );
	g_theRenderer->BindTexture( baseTexture );
	g_theRenderer->DrawVertexArray( 6, bulletVertexes );



	Entity::DebugRender();
}

