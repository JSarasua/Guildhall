#include "Engine/Physics2D/Physics2D.hpp"
#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"

void Physics2D::BeginFrame()
{

}

void Physics2D::Update()
{

}

void Physics2D::EndFrame()
{

}

Rigidbody2D* Physics2D::CreateRigidBody()
{
	Rigidbody2D* rb = new Rigidbody2D();
	return rb;
}

void Physics2D::DestroyRigidBody( Rigidbody2D* rb )
{
	delete rb;
	rb = nullptr;
}

DiscCollider2D* Physics2D::CreateDiscCollider( Vec2 localPosition, float radius )
{
	DiscCollider2D* dc = new DiscCollider2D(localPosition, Vec2(0.f, 0.f), radius );
	return dc;
}

void Physics2D::DestroyCollider( Collider2D* collider )
{
	//delete collider;
	collider = nullptr;
}

