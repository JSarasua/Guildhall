#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Physics2D/Physics2D.hpp"

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
}

