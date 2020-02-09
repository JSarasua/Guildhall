#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Physics2D/Physics2D.hpp"
#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"


void Rigidbody2D::Update( float deltaSeconds )
{
	if( m_simulationMode != STATIC )
	{
		m_worldPosition += m_velocity * deltaSeconds;
		if( nullptr != m_collider )
		{
			m_collider->UpdateWorldShape();
		}
	}

}

void Rigidbody2D::AddForce( float deltaSeconds, Vec2 const& forceValue )
{
	if( m_isEnabled && m_simulationMode == DYNAMIC )
	{
		m_velocity += forceValue * deltaSeconds;
	}
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
}

void Rigidbody2D::SetPosition( Vec2 const& position )
{
	m_worldPosition = position;
	if( nullptr != m_collider )
	{
		m_collider->UpdateWorldShape();
	}
}

void Rigidbody2D::SetVelocity( Vec2 const& newVelocity )
{
	m_velocity = newVelocity;
}

void Rigidbody2D::SetSimulationMode( eSimulationMode simulationMode )
{
	m_simulationMode = simulationMode;
}

Vec2 Rigidbody2D::GetVelocity()
{
	return m_velocity;
}

Vec2 Rigidbody2D::GetPosition()
{
	return m_worldPosition;
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

Rigidbody2D::~Rigidbody2D()
{
	
}

