#include "Game/Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/App.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;

Entity::Entity( Game* game ): 
	m_game( game ), 
	m_position( 0.f, 0.f ), 
	m_velocity( 0.f, 0.f ), 
	m_orientationDegrees( 0.f )
{}

Entity::Entity( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, Game* game ) :
	m_position(initialPosition),
	m_velocity(initialVelocity),
	m_orientationDegrees(initialOrientationDegrees),
	m_angularVelocity(initialAngularVelocity),
	m_game(game)

{}

Entity::Entity( EntityType type, EntityFaction faction, Game* game ):
	m_EntityType(type),
	m_EntityFaction(faction),
	m_game(game),
	m_position(0.f, 0.f),
	m_velocity(0.f, 0.f),
	m_orientationDegrees(0.f)
{}

void Entity::Update( float deltaSeconds )
{
	m_orientationDegrees += m_angularVelocity * deltaSeconds;
	m_position = TransformPosition2D(m_position, 1.f, 0.f, m_velocity * deltaSeconds);
}

void Entity::DebugRender() const
{
	if( g_theApp->GetDebugGameMode() )
	{
		g_theRenderer->BindTexture( nullptr );
		DrawRing( m_position, m_cosmeticRadius, Rgba8( 255, 0, 255 ), 0.01f );
		DrawRing( m_position, m_physicsRadius, Rgba8( 0, 255, 255 ), 0.01f );
	}
}

bool Entity::IsOffScreen()
{
	return false;
}

Vec2 Entity::GetForwardVelocityVector()
{
	return m_velocity;
}

Vec2 Entity::GetForwardVector() const
{
	return Vec2::MakeFromPolarDegrees(m_orientationDegrees);
}

float Entity::GetOrientationDegrees() const
{
	return m_orientationDegrees;
}

bool Entity::IsAlive()
{
	return !m_isDead;
}

void Entity::SetAlive()
{
	m_isDead = false;
}

const Vec2 Entity::GetPosition()
{
	return m_position;
}

void Entity::Lose1Health()
{
	if( m_health != 0 )
	{
		m_health -= 1;
	}
	if( m_health == 0 )
	{
		m_isDead = true;
		m_isGarbage = true;
	}

}

void Entity::LoseAllHealth()
{
	m_health = 0;
	m_isDead = true;
	m_isGarbage = true;
}

const Rgba8& Entity::GetColor()
{
	return m_color;
}

bool Entity::IsPushedByEntities() const
{
	return m_isPushedByEntities;
}

bool Entity::IsPushedByWalls() const
{
	return m_isPushedByWalls;
}

bool Entity::DoesPushEntities() const
{
	return m_doesPushEntities;
}

bool Entity::IsHitByBullets() const
{
	return m_isHitByBullets;
}

void Entity::SetPosition( const Vec2& newPosition )
{
	m_position = newPosition;
}

void Entity::SetVelocity( const Vec2& newVelocity )
{
	m_velocity = newVelocity;
}

void Entity::MarkGarbage()
{
	m_isGarbage = true;
}

bool Entity::IsGarbage() const
{
	return  !this || m_isGarbage;
}
