#include "Game/Entity.hpp"

Entity::Entity( Game* game ): m_game( game ), m_position( 0.f, 0.f ), m_velocity( 0.f, 0.f ), m_orientationDegrees( 0.f )
{}

Entity::Entity( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, Game* game ) :
	m_position(initialPosition),
	m_velocity(initialVelocity),
	m_orientationDegrees(initialOrientationDegrees),
	m_angularVelocity(initialAngularVelocity),
	m_game(game)

{}

bool Entity::IsOffScreen()
{
	return false;
}

Vec2 Entity::GetForwardVector()
{
	return m_velocity;
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
	}

}

void Entity::LoseAllHealth()
{
	m_health = 0;
	m_isDead = true;
}

const Rgba8& Entity::GetColor()
{
	return m_color;
}
