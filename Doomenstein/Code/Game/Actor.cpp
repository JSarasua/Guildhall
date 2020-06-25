#include "Game/Actor.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/LineSegment3.hpp"

Actor::Actor( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& pitchRollYawDegrees ) :
	Entity( entityDef, initialPosition, pitchRollYawDegrees )
{

}

void Actor::Update( float deltaSeconds )
{

}

void Actor::Render() const
{
	float eyeHeight = m_entityDef->GetEyeHeight();
	float physicsRadius = m_entityDef->GetPhysicsRadius();
	float height = m_entityDef->GetHeight();
	Vec3 position = Vec3( m_position, eyeHeight );
	DebugAddWorldWireCylinder( m_position, physicsRadius, height, Rgba8::WHITE, Rgba8::WHITE, 0.f );

	LineSegment3 forwardVec = LineSegment3( Vec3(m_position, eyeHeight), Vec3(m_position, eyeHeight) + GetForwardVector() );
	DebugAddWorldArrow( forwardVec, Rgba8::RED, 0.f );
}

