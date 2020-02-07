#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Physics2D/Physics2D.hpp"
#include "Engine/Physics2D/Collider2D.hpp"

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
	m_collider->UpdateWorldShape();
}

Vec2 Rigidbody2D::GetPosition()
{
	return m_worldPosition;
}

void Rigidbody2D::DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor )
{
	m_collider->DebugRender( context, borderColor, fillColor );
}

Rigidbody2D::~Rigidbody2D()
{
	
}
