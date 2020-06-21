#include "Game/Entity.hpp"
#include "Engine/Math/MathUtils.hpp"


Entity::Entity( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity ) :
	m_position(initialPosition),
	m_velocity(initialVelocity),
	m_orientationDegrees(initialOrientationDegrees),
	m_angularVelocity(initialAngularVelocity)

{}

void Entity::Update( float deltaSeconds )
{
	//m_orientationDegrees += m_angularVelocity * deltaSeconds;
	//m_position = TransformPosition2D(m_position, 1.f, 0.f, m_velocity * deltaSeconds);
	m_position += m_velocity * deltaSeconds;
}

bool Entity::IsFiring() const
{
	return m_isFiring;
}

void Entity::SetIsFiring( bool isFiring )
{
	m_isFiring = isFiring;
}

bool Entity::IsGarbage() const
{
	return !this || m_isGarbage;
}

bool Entity::IsOffScreen()
{
	return false;
}

Vec2 Entity::GetForwardVector()
{
	return m_velocity;
}

float Entity::GetOrientationDegrees()
{
	return m_orientationDegrees;
}

float Entity::GetWeaponOrientationDegrees() const
{
	float weaponOrientation = m_weaponOrientationDegrees;
// 	if( weaponOrientation > 90.f && weaponOrientation < 180.f )
// 	{
// 		weaponOrientation = -m_weaponOrientationDegrees;
// 	}
	return weaponOrientation;
}

Vec2 Entity::GetWeaponStartPosition()
{
	return m_position + m_weaponOffset;
}

Vec2 Entity::GetBulletStartPosition() const
{
	return m_position + m_weaponOffset + m_bulletOffset;
}

bool Entity::IsAlive() const
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

void Entity::LoseHealth( int damage )
{
	m_health -= damage;
	if( m_health <= 0 )
	{
		m_health = 0;
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

void Entity::SetPosition( const Vec2& newPosition )
{
	m_position = newPosition;
}
