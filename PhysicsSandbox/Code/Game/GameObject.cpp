#include "Game/GameObject.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"

GameObject::GameObject( Rigidbody2D* rigidbody )
	: m_rigidbody(rigidbody)
{

}

GameObject::~GameObject()
{
	// delete m_rigidbody; // this should be an error if setup right
	m_rigidbody->Destroy(); // destroys through the system;  
	m_rigidbody = nullptr;
}

void GameObject::SetPosition( Vec2 const& position )
{
	m_rigidbody->SetPosition(position);
}

Vec2 GameObject::GetPosition()
{
	return m_rigidbody->GetPosition();
}

void GameObject::DebugRender( RenderContext* context )
{
	if( nullptr != m_rigidbody )
	{
		m_rigidbody->DebugRender( context, m_borderColor, m_fillColor );
	}

}

