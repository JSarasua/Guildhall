#include "Game/Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"


Entity::Entity( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& pitchRollYawDegrees ) :
	m_entityDef( entityDef ), m_position( initialPosition ), m_pitchRollYawDegrees( pitchRollYawDegrees )
{

}

void Entity::Update( float deltaSeconds )
{
//	m_orientationDegrees += m_angularVelocity * deltaSeconds;
//	m_position = TransformPosition2D(m_position, 1.f, m_orientationDegrees, m_velocity * deltaSeconds);
}

bool Entity::IsOffScreen()
{
	return false;
}

Vec3 Entity::GetForwardVector()
{
	Mat44 mat;
	mat.RotateYawPitchRollDegress( m_pitchRollYawDegrees.z, m_pitchRollYawDegrees.x, m_pitchRollYawDegrees.y );
	Vec3 forward( 1.f, 0.f, 0.f );
	forward = mat.TransformVector3D( forward );
	
	return forward;
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

void Entity::SetPosition( const Vec2& newPosition )
{
	m_position = newPosition;
}
