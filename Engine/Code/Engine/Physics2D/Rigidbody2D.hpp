#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

class Physics2D;
class Collider2D;
class RenderContext;

class Rigidbody2D
{
	friend class Physics2D;

public:
	void Destroy();
	void TakeCollider( Collider2D* collider );

	void SetPosition( Vec2 const& position );
	Vec2 GetPosition();

	void DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor );

public:
	Physics2D* m_system;
	Collider2D* m_collider;


protected:
	~Rigidbody2D(); //Destroys the collider

protected:
	bool m_isGarbage = false;
	Vec2 m_worldPosition;
};