#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

class Rigidbody2D;
class RenderContext;

class GameObject
{
public:
	GameObject(Rigidbody2D* rigidbody);
	~GameObject();
	
	void SetPosition( Vec2 const& position );
	Vec2 GetPosition();

	void DebugRender( RenderContext* context );

public:
	Rigidbody2D* m_rigidbody;
	Rgba8 m_fillColor = Rgba8(255,255,255,128);
	Rgba8 m_borderColor = Rgba8::BLUE;
};

