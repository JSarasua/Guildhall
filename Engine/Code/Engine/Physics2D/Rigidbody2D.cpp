#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Physics2D/Physics2D.hpp"
#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"


void Rigidbody2D::Update( float deltaSeconds )
{
	m_frameStartPosition = m_worldPosition;

	if( m_isEnabled && m_simulationMode == DYNAMIC )
	{
		Vec2 acceleration = m_forcePerFrame/m_mass;
		m_velocity += acceleration * deltaSeconds;
	}


	if( m_simulationMode != STATIC )
	{
		m_worldPosition += m_velocity * deltaSeconds;
		UpdateRotation( deltaSeconds );
		if( nullptr != m_collider )
		{
			m_collider->UpdateWorldShape();
		}
	}



	m_frameTorque = 0.f;
	m_forcePerFrame = Vec2(0.f, 0.f);
}

void Rigidbody2D::UpdateRotation( float deltaSeconds )
{
	CalculateMoment();
	if( m_isEnabled && m_simulationMode == DYNAMIC )
	{
		float angularAcceleration = m_frameTorque / m_moment;
		m_angularVelocity += angularAcceleration * deltaSeconds;
	}


	if( m_simulationMode != STATIC )
	{
		m_orientationInRadians += m_angularVelocity * deltaSeconds;
	}
}

void Rigidbody2D::AddForce( Vec2 const& forceValue )
{
	m_forcePerFrame += forceValue;
}

float Rigidbody2D::GetMass() const
{
	return m_mass;
}

float Rigidbody2D::GetDrag() const
{
	return m_drag;
}

float Rigidbody2D::CalculateMoment()
{
	if( nullptr != m_collider )
	{
		m_moment = m_collider->CalculateMoment( m_mass );
		return m_moment;
	}

	//Safe value before deleting
	return 1.f;
}

void Rigidbody2D::Destroy()
{
	m_system->DestroyRigidBody(this);
}

void Rigidbody2D::TakeCollider( Collider2D* collider )
{
	if( nullptr != m_collider )
	{
		m_system->DestroyCollider(m_collider);
	}

	m_collider = collider;
	m_collider->m_rigidbody = this;
	CalculateMoment();
}

void Rigidbody2D::SetPosition( Vec2 const& position )
{
	m_worldPosition = position;
	m_frameStartPosition = m_worldPosition;

	if( nullptr != m_collider )
	{
		m_collider->UpdateWorldShape();
	}
}

void Rigidbody2D::SetMass( float newMass )
{
	m_mass = newMass;
	CalculateMoment();
}

void Rigidbody2D::SetOrientationRadians( float newOrientationRadians )
{
	m_orientationInRadians = newOrientationRadians;
	m_collider->UpdateWorldShape();
}

void Rigidbody2D::SetAngularVelocityRandians( float newAngularVelocity )
{
	m_angularVelocity = newAngularVelocity;
}

void Rigidbody2D::Translate( Vec2 const& translator )
{
	m_worldPosition += translator;
	m_frameStartPosition = m_worldPosition;
}

void Rigidbody2D::SetVelocity( Vec2 const& newVelocity )
{
	m_velocity = newVelocity;
}

void Rigidbody2D::SetSimulationMode( eSimulationMode simulationMode )
{
	m_simulationMode = simulationMode;
}

eSimulationMode Rigidbody2D::GetSimulationMode() const
{
	return m_simulationMode;
}

void Rigidbody2D::ApplyImpulseAt( Vec2 const& worldPos, Vec2 const& impulse )
{
	if( m_simulationMode == DYNAMIC )
	{
		float inverseMass = 1.f/m_mass;
		Vec2 deltaVelocity = impulse * inverseMass;

		m_velocity += deltaVelocity;


		Vec2 center = m_collider->GetCenterOfMass();
		Vec2 centerToWorldPos = worldPos - center;

		Vec2 centerToWorldPerp = centerToWorldPos.GetRotated90Degrees();

		float deltaAngularVelocity = DotProduct2D( centerToWorldPerp, impulse ) / m_moment;
		m_angularVelocity += deltaAngularVelocity;

	}
}

void Rigidbody2D::ApplyDragForce()
{
	Vec2 velocity = GetVerletVelocity();
	Vec2 dragForce = -velocity * m_drag;
	AddForce( dragForce );
}

void Rigidbody2D::SetDragCoefficient( float dragCoefficient )
{
	m_drag = dragCoefficient;
}

Vec2 Rigidbody2D::GetVelocity()
{
	return m_velocity;
}

Vec2 Rigidbody2D::GetVerletVelocity()
{
	Vec2 verletVelocity = (m_worldPosition - m_frameStartPosition) / m_system->GetFixedDeltaTime();
	return verletVelocity;
}

Vec2 Rigidbody2D::GetPosition()
{
	return m_worldPosition;
}

Vec2 Rigidbody2D::GetImpactVelocityAtPoint( Vec2 const& point )
{

	Vec2 centerToPoint = point - m_collider->GetCenterOfMass();
	float radius = centerToPoint.GetLength();
	float linearSpeedFromRotation = radius * m_angularVelocity;
	Vec2 centerToPointTangent = centerToPoint.GetRotated90Degrees();
	centerToPointTangent.Normalize();

	Vec2 linearVelocityFromRotation = centerToPointTangent * linearSpeedFromRotation;

	Vec2 currentVelocity = GetVerletVelocity() + linearVelocityFromRotation;

	return currentVelocity;
}

void Rigidbody2D::DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor )
{
	m_collider->DebugRender( context, borderColor, fillColor, 1.f );

	Vec2 positiveOffset = Vec2( 0.5f, 0.5f );
	Vec2 negativeOffset = Vec2( 0.5f, -0.5f );
	if( !m_isEnabled )
	{
		context->DrawLine(m_worldPosition - positiveOffset, m_worldPosition + positiveOffset, Rgba8::RED, 0.5f);
		context->DrawLine(m_worldPosition - negativeOffset, m_worldPosition + negativeOffset, Rgba8::RED, 0.5f);
	}
	else
	{
		context->DrawLine( m_worldPosition - positiveOffset, m_worldPosition + positiveOffset, Rgba8::BLUE, 0.5f );
		context->DrawLine( m_worldPosition - negativeOffset, m_worldPosition + negativeOffset, Rgba8::BLUE, 0.5f );
	}
}

void Rigidbody2D::EnableRigidbody()
{
	m_isEnabled = true;
}

void Rigidbody2D::DisableRigidbody()
{
	m_isEnabled = false;
}

bool Rigidbody2D::IsEnabled()
{
	return m_isEnabled;
}

float Rigidbody2D::GetOrientationRadians() const
{
	return m_orientationInRadians;
}

float Rigidbody2D::GetAngularVelocityRadians() const
{
	return m_angularVelocity;
}

Rigidbody2D::~Rigidbody2D()
{
	
}

float Rigidbody2D::GetMoment() const
{
	return m_moment;
}

