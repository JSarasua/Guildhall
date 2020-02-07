#include "Engine/Physics2D/Physics2D.hpp"
#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Physics2D/PolygonCollider2D.hpp"

void Physics2D::BeginFrame()
{

}

void Physics2D::Update()
{
}

void Physics2D::EndFrame()
{
	for( int colliderIndex = 0; colliderIndex < m_colliders.size(); colliderIndex++ )
	{
		bool isGarbage = m_colliders[colliderIndex]->m_isGarbage;
		if( isGarbage )
		{
			delete m_colliders[colliderIndex];
			m_colliders[colliderIndex] = nullptr;
		}
	}

	for( int rigidBodyIndex = 0; rigidBodyIndex < m_rigidBodies.size(); rigidBodyIndex++ )
	{
		bool isGarbage = m_rigidBodies[rigidBodyIndex]->m_isGarbage;
		if( isGarbage )
		{
			delete m_rigidBodies[rigidBodyIndex];
			m_rigidBodies[rigidBodyIndex] = nullptr;
		}
	}
}


Rigidbody2D* Physics2D::CreateRigidBody()
{
	Rigidbody2D* rb = new Rigidbody2D();
	m_rigidBodies.push_back(rb);
	return rb;
}

void Physics2D::DestroyRigidBody( Rigidbody2D* rb )
{
	rb->m_isGarbage = true;

	rb->m_collider->m_isGarbage = true;;
	rb->m_collider = nullptr;
	
}

DiscCollider2D* Physics2D::CreateDiscCollider( Vec2 localPosition, float radius )
{
	DiscCollider2D* dc = new DiscCollider2D(localPosition, Vec2(0.f, 0.f), radius );
	m_colliders.push_back(dc);
	return dc;
}

PolygonCollider2D* Physics2D::CreatePolygonCollider( Polygon2D const& poly, Vec2 localPosition )
{
	Vec2 worldPosition = poly.GetCenterOfMass();
	PolygonCollider2D* pc = new PolygonCollider2D(localPosition, worldPosition, poly );
	return pc;
}

void Physics2D::DestroyCollider( Collider2D* collider )
{
	collider->m_isGarbage = true;
}

