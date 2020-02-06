#pragma once
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/World.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Physics2D/Physics2D.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Math/Polygon2D.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;

Game::Game()
{

}

Game::~Game(){}

void Game::Startup()
{
	m_camera = new Camera();
	m_UICamera = new Camera();

	m_physics = new Physics2D();

	m_camera->SetOutputSize( Vec2( GAME_CAMERA_X, GAME_CAMERA_Y ) );
	m_defaultCameraHeight = m_camera->m_outputSize.y;
	m_camera->SetPosition( Vec2(0.f,0.f) );
	
	//g_theRenderer->SetBlendMode(BlendMode::ADDITIVE);
}

void Game::Shutdown(){}

void Game::RunFrame(){}

void Game::Update( float deltaSeconds )
{
	UpdateDebugMouse();
	UpdateGameObjects(deltaSeconds);
	CheckButtonPresses( deltaSeconds );
}

void Game::Render()
{
	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	g_theRenderer->BeginCamera( *m_camera );
	RenderDebugMouse();
	RenderGameObjects();
	g_theRenderer->EndCamera( *m_camera );


// 	g_theRenderer->BeginCamera( m_UICamera );
// 	RenderUI();
// 	g_theRenderer->EndCamera( m_UICamera );

}


void Game::CheckCollisions()
{}

void Game::UpdateGameObjects( float deltaSeconds )
{
	UNUSED(deltaSeconds);

	for( int gameObjectIndex = 0; gameObjectIndex < m_gameObjects.size(); gameObjectIndex++ )
	{
		if( nullptr == m_gameObjects[gameObjectIndex] )
		{
			continue;
		}

		if( m_draggingGameObject != m_gameObjects[gameObjectIndex] )
		{
			m_gameObjects[gameObjectIndex]->m_borderColor = Rgba8::BLUE;
		}

		if( m_draggingGameObject != m_gameObjects[gameObjectIndex] )
		{
			if( m_gameObjects[gameObjectIndex]->m_rigidbody->m_collider->Contains( m_mousePositionOnMainCamera ) )
			{
					m_gameObjects[gameObjectIndex]->m_borderColor = Rgba8::GREEN;
			}
		}

	}


	for( int gameObjectIndex = 0; gameObjectIndex < m_gameObjects.size(); gameObjectIndex++ )
	{
		if( nullptr == m_gameObjects[gameObjectIndex] )
		{
			continue;
		}
		m_gameObjects[gameObjectIndex]->m_fillColor = Rgba8(255,255,255,128);

		for( int otherGameObjectIndex = 0; otherGameObjectIndex < m_gameObjects.size(); otherGameObjectIndex++ )
		{
			if( nullptr == m_gameObjects[otherGameObjectIndex] )
			{
				continue;
			}
			if( gameObjectIndex != otherGameObjectIndex )
			{
				Collider2D* collider = m_gameObjects[gameObjectIndex]->m_rigidbody->m_collider;
				Collider2D* otherCollider = m_gameObjects[otherGameObjectIndex]->m_rigidbody->m_collider;

				if( collider->Intersects( otherCollider ) )
				{
					m_gameObjects[gameObjectIndex]->m_fillColor = Rgba8(255,0,0,128);
				}
			}
		}
	}

}

void Game::UpdateDebugMouse()
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedPos();
	m_mousePositionOnMainCamera = m_camera->GetClientToWorldPosition(mouseNormalizedPos);

	if( m_draggingGameObject )
	{
		Rigidbody2D* rb = m_draggingGameObject->m_rigidbody;
		Vec2 updatedDraggedPos = Vec2(m_mousePositionOnMainCamera + m_draggingOffset);
		rb->SetPosition( updatedDraggedPos );
	}
}

void Game::RenderDebugMouse() const
{
	//m_mouseGameObject->DebugRender(g_theRenderer, Rgba8::BLUE, Rgba8(255,255,255,128) );

}

void Game::RenderUI() const
{
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	const XboxController& controller = g_theInput->GetXboxController(0);

	const KeyButtonState& num1Key = g_theInput->GetKeyStates('1');
	const KeyButtonState& num2Key = g_theInput->GetKeyStates('2');

	const KeyButtonState& leftMouseButton = g_theInput->GetMouseButton(LeftMouseButton);
	const KeyButtonState& rightMouseButton = g_theInput->GetMouseButton(RightMouseButton);
	//const KeyButtonState& rightMouseButton = g_theInput->GetMouseButton(RightMouseButton);
	float mouseWheelScroll = g_theInput->GetDeltaMouseWheelScroll();


	const KeyButtonState& wKey = g_theInput->GetKeyStates('W');
	const KeyButtonState& aKey = g_theInput->GetKeyStates('A');
	const KeyButtonState& sKey = g_theInput->GetKeyStates('S');
	const KeyButtonState& dKey = g_theInput->GetKeyStates('D');
	const KeyButtonState& oKey = g_theInput->GetKeyStates('O');
	const KeyButtonState& delKey = g_theInput->GetKeyStates( 0x2E );
	const KeyButtonState& bSpaceKey = g_theInput->GetKeyStates( 0x08 );

	if( wKey.IsPressed() )
	{
		Vec2 upVector = Vec2( 0.f, 10.f );
		m_camera->Translate2D( upVector * deltaSeconds );
	}

	if( sKey.IsPressed() )
	{
		Vec2 downVector = Vec2( 0.f, -10.f );
		m_camera->Translate2D( downVector * deltaSeconds );
	}

	if( aKey.IsPressed() )
	{
		Vec2 leftVector = Vec2( 10.f, 0.f );
		m_camera->Translate2D( leftVector * deltaSeconds );
	}

	if( dKey.IsPressed() )
	{
		Vec2 rightVector = Vec2( -10.f, 0.f );
		m_camera->Translate2D( rightVector * deltaSeconds );
	}

	if( oKey.WasJustPressed() )
	{
		m_camera->m_position = Vec2( 0.f, 0.f );
		m_camera->SetProjectionOrthographic( m_defaultCameraHeight );
	}

	if( num1Key.WasJustPressed() )
	{
		Rigidbody2D* rb = m_physics->CreateRigidBody();
		float randNum = m_rand.RollRandomFloatInRange(5.f, 15.f);
		DiscCollider2D* dc = m_physics->CreateDiscCollider( Vec2( 0.f, 0.f ), randNum );
		rb->TakeCollider( dc );
		rb->SetPosition(m_mousePositionOnMainCamera);
		GameObject* gameObject = new GameObject( rb );

		m_gameObjects.push_back( gameObject );
	}

	if( num2Key.WasJustPressed() )
	{
		m_polygonPoints.push_back( m_mousePositionOnMainCamera );
	}

	if( bSpaceKey.WasJustPressed() || delKey.WasJustPressed() )
	{
		if( m_draggingGameObject )
		{
			int gameObjectsIndex = GetGameObjectIndex( m_draggingGameObject );

			delete m_draggingGameObject;

			m_draggingGameObject = nullptr;
			m_gameObjects[gameObjectsIndex] = nullptr;
		}

	}

	if( leftMouseButton.WasJustPressed() )
	{
		GrabDiscIfOverlap();
	}

	if( leftMouseButton.WasJustReleased() )
	{
		ReleaseDisc();
	}

	if( rightMouseButton.WasJustPressed() )
	{
		Polygon2D newPoly(m_polygonPoints);
	}

	if( mouseWheelScroll != 0.f )
	{
		float cameraHeight = m_camera->m_outputSize.y;

		cameraHeight  += (mouseWheelScroll * 10.f);
		cameraHeight = Clampf(cameraHeight, 10.f, 1000.f);
		//float newOrienation = GetTurnedToward( orientation, orientationIncremented, 2.f );
		m_camera->SetProjectionOrthographic(cameraHeight);
 	}

// 	if( rKey.WasJustPressed() )
// 	{
// 		SetShapePositionsAndColors();
// 	}
// 
// 	if( tKey.WasJustPressed() )
// 	{
// 		m_UseOBB2AtMouse = !m_UseOBB2AtMouse;
// 	}
// 
// 	if( yKey.IsPressed() )
// 	{
// 		float orientation = m_OBB2AtMouse.GetOrientationDegrees();
// 		float orientationIncremented = orientation + 30.f;
// 		float newOrienation = GetTurnedToward(orientation, orientationIncremented, 2.f );
// 		m_OBB2AtMouse.SetOrientationDegrees(newOrienation);
// 	}
// 
// 	if( leftMouseButton.WasJustPressed() )
// 	{
// 		m_UseOBB2AtMouse = !m_UseOBB2AtMouse;
// 	}
// 	else if( rightMouseButton.WasJustPressed() )
// 	{
// 		SetShapePositionsAndColors();
// 	}
// 
// 
// 	if( mouseWheelScroll != 0.f )
// 	{
// 		float orientation = m_OBB2AtMouse.GetOrientationDegrees();
// 		float orientationIncremented = orientation + (mouseWheelScroll * 10.f);
// 		//float newOrienation = GetTurnedToward( orientation, orientationIncremented, 2.f );
// 		m_OBB2AtMouse.SetOrientationDegrees( orientationIncremented );
// 	}

	UNUSED( deltaSeconds );
	UNUSED( controller );
}

void Game::RenderGameObjects()
{
	for( int gameObjectIndex = 0; gameObjectIndex < m_gameObjects.size(); gameObjectIndex++ )
	{
		if( nullptr != m_gameObjects[gameObjectIndex] )
		{
			m_gameObjects[gameObjectIndex]->DebugRender(g_theRenderer);
		}

	}
}

void Game::GrabDiscIfOverlap()
{
	for( int gameObjectIndex = (int)m_gameObjects.size() - 1; gameObjectIndex >= 0; gameObjectIndex-- )
	{
		if( nullptr == m_gameObjects[gameObjectIndex] )
		{
			continue;
		}
		Collider2D* collider = m_gameObjects[gameObjectIndex]->m_rigidbody->m_collider;
		if( collider->Contains( m_mousePositionOnMainCamera ) )
		{
			Vec2 colliderPos = m_gameObjects[gameObjectIndex]->m_rigidbody->GetPosition();
			m_draggingGameObject = m_gameObjects[gameObjectIndex];
			m_draggingOffset = colliderPos - m_mousePositionOnMainCamera;
			m_draggingGameObject->m_borderColor = Rgba8(0,100,0,255);
			break;
		}
	}
}

void Game::ReleaseDisc()
{
	if( nullptr != m_draggingGameObject )
	{
		m_draggingGameObject->m_borderColor = Rgba8::BLUE;
		m_draggingGameObject = nullptr;
		m_draggingOffset = Vec2( 0.f, 0.f );
	}

}

int Game::GetGameObjectIndex( GameObject* gameObject )
{
	for( int gameObjectsIndex = 0; gameObjectsIndex < m_gameObjects.size(); gameObjectsIndex++ )
	{
		if( gameObject == m_gameObjects[gameObjectsIndex] )
		{
			return gameObjectsIndex;
		}
	}

	return -1;
}
