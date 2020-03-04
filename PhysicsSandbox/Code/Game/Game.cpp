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
#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Time/Clock.hpp"

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

	m_camera->SetColorTarget( nullptr );
	m_UICamera->SetColorTarget( nullptr );

	m_gameClock = new Clock();
	m_gameClock->SetParent( Clock::GetMaster() );

	m_physics = new Physics2D();
	m_physics->SetClock( m_gameClock );
	m_physics->Startup();

	//m_camera->SetOutputSize( Vec2( GAME_CAMERA_X, GAME_CAMERA_Y ) );
	m_camera->SetProjectionOrthographic( Vec2( GAME_CAMERA_X, GAME_CAMERA_Y ), 0.f, 1.f );
	m_defaultCameraHeight = m_camera->m_outputSize.y;
	m_camera->SetPosition( Vec2(0.f,0.f) );

	m_UICamera->SetProjectionOrthographic( Vec2( GAME_CAMERA_X, GAME_CAMERA_Y ), 0.f, 1.f );
	m_defaultCameraHeight = m_UICamera->m_outputSize.y;
	m_UICamera->SetPosition( Vec2( 0.f, 0.f ) );

	m_mouseDeltaPositions.resize( 5 );
	m_mouseDeltaTime.resize( 5 );
	


	g_theRenderer->Setup( m_gameClock );
}

void Game::Shutdown()
{
	delete m_camera;
	delete m_UICamera;
}

void Game::RunFrame(){}

void Game::Update()
{

	float dt = (float)m_gameClock->GetLastDeltaSeconds();
	UpdateCameraBounds();
	m_physics->Update();
	UpdateDebugMouse( dt );
	CheckBorderCollisions();
	UpdateGameObjects(dt);
	CheckButtonPresses( dt );

	m_mouseLastPosition = m_mousePositionOnMainCamera;
}

void Game::Render()
{
	//g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	m_camera->SetClearMode( CLEAR_COLOR_BIT, Rgba8( 0, 0, 0, 255 ), 0.f, 0 );
	g_theRenderer->BeginCamera( *m_camera );
	g_theRenderer->SetBlendMode( BlendMode::ALPHA );
	RenderDebugMouse();
	RenderGameObjects();
	g_theRenderer->EndCamera( *m_camera );


/*	m_UICamera->SetClearMode( CLEAR_COLOR_BIT, Rgba8( 0, 0, 0, 255 ), 0.f, 0 );*/
	g_theRenderer->BeginCamera( *m_UICamera );
	RenderUI();
	g_theRenderer->EndCamera( *m_UICamera );

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


// 	for( int gameObjectIndex = 0; gameObjectIndex < m_gameObjects.size(); gameObjectIndex++ )
// 	{
// 		if( nullptr == m_gameObjects[gameObjectIndex] )
// 		{
// 			continue;
// 		}
// 		m_gameObjects[gameObjectIndex]->m_fillColor = Rgba8(255,255,255,128);
// 
// 		for( int otherGameObjectIndex = 0; otherGameObjectIndex < m_gameObjects.size(); otherGameObjectIndex++ )
// 		{
// 			if( nullptr == m_gameObjects[otherGameObjectIndex] )
// 			{
// 				continue;
// 			}
// 			if( gameObjectIndex != otherGameObjectIndex )
// 			{
// 				Collider2D* collider = m_gameObjects[gameObjectIndex]->m_rigidbody->m_collider;
// 				Collider2D* otherCollider = m_gameObjects[otherGameObjectIndex]->m_rigidbody->m_collider;
// 
// 				if( collider->Intersects( otherCollider ) )
// 				{
// 					m_gameObjects[gameObjectIndex]->m_fillColor = Rgba8(255,0,0,128);
// 				}
// 			}
// 		}
// 	}

	for( int gameObjectIndex = 0; gameObjectIndex < m_gameObjects.size(); gameObjectIndex++ )
	{
		if( nullptr == m_gameObjects[gameObjectIndex] )
		{
			continue;
		}

		Vec2 bottomLeft = m_camera->GetOrthoBottomLeft();
		Vec2 bottomRight = Vec2(m_camera->GetOrthoTopRight().x, m_camera->GetOrthoBottomLeft().y);
		LineSegment2 bottomCameraLine( bottomLeft, bottomRight );
	}

}

void Game::UpdateDebugMouse( float deltaSeconds )
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedPos();
	//m_mousePositionOnMainCamera = m_camera->GetClientToWorldPosition(mouseNormalizedPos);
	Vec3 clientToWorld = m_camera->ClientToWorld( mouseNormalizedPos, 0.f );
	m_mousePositionOnMainCamera = Vec2( clientToWorld.x, clientToWorld.y );

	if( m_draggingGameObject )
	{
		Rigidbody2D* rb = m_draggingGameObject->m_rigidbody;
		Vec2 updatedDraggedPos = Vec2(m_mousePositionOnMainCamera + m_draggingOffset);
		rb->SetPosition( updatedDraggedPos );
		rb->SetVelocity( m_currentMouseVelocity );
	}

	std::vector<Vec2> polyPoints;
	polyPoints = m_polygonPoints;
	polyPoints.push_back( m_mousePositionOnMainCamera );
	Polygon2D poly( polyPoints );

	if( poly.IsConvex() || !poly.IsValid() )
	{
		m_isPolyValid = true;
	}
	else
	{
		m_isPolyValid = false;
	}

	Vec2 mouseDeltaPosition = m_mousePositionOnMainCamera - m_mouseLastPosition;
	m_mouseDeltaPositions[m_mouseDeltaIndex] = mouseDeltaPosition;
	m_mouseDeltaTime[m_mouseDeltaIndex] = deltaSeconds;

	m_mouseDeltaIndex++;
	if( m_mouseDeltaIndex >= m_mouseDeltaPositions.size() )
	{
		m_mouseDeltaIndex = 0;
	}

	SetCurrentMouseVelocity();

	
}

void Game::RenderDebugMouse() const
{
	if( !m_polygonPoints.empty() )
	{
		if( m_isPolyValid )
		{
			g_theRenderer->DrawLine( m_polygonPoints.back(), m_mousePositionOnMainCamera, Rgba8::BLUE, 1.f );
		}
		else
		{
			g_theRenderer->DrawLine( m_polygonPoints.back(), m_mousePositionOnMainCamera, Rgba8::RED, 1.f );
		}
	}
}

void Game::RenderUI() const
{
	std::string gravityUIString = Stringf("Gravity: %.2f",m_physics->GetSceneGravity());

	g_theRenderer->DrawTextAtPosition(gravityUIString.c_str(), Vec2(60.f, 43.f) , 1.f);
	g_theRenderer->DrawTextAtPosition("Adjust: +/-", Vec2(60.f, 42.f) , 1.f);
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	const XboxController& controller = g_theInput->GetXboxController(0);

	const KeyButtonState& num1Key = g_theInput->GetKeyStates('1');
	const KeyButtonState& num2Key = g_theInput->GetKeyStates('2');
	const KeyButtonState& num3Key = g_theInput->GetKeyStates('3');

	const KeyButtonState& leftMouseButton = g_theInput->GetMouseButton(LeftMouseButton);
	const KeyButtonState& rightMouseButton = g_theInput->GetMouseButton(RightMouseButton);
	//const KeyButtonState& rightMouseButton = g_theInput->GetMouseButton(RightMouseButton);
	float mouseWheelScroll = g_theInput->GetDeltaMouseWheelScroll();


	const KeyButtonState& wKey				= g_theInput->GetKeyStates('W');
	const KeyButtonState& aKey				= g_theInput->GetKeyStates('A');
	const KeyButtonState& sKey				= g_theInput->GetKeyStates('S');
	const KeyButtonState& dKey				= g_theInput->GetKeyStates('D');
	const KeyButtonState& oKey				= g_theInput->GetKeyStates('O');
	const KeyButtonState& delKey			= g_theInput->GetKeyStates( 0x2E );
	const KeyButtonState& bSpaceKey			= g_theInput->GetKeyStates( 0x08 ); //backspace
	const KeyButtonState& escKey			= g_theInput->GetKeyStates( ESC_KEY );	//ESC
	const KeyButtonState& plusKey			= g_theInput->GetKeyStates( 0xBB ); //+
	const KeyButtonState& minusKey			= g_theInput->GetKeyStates( 0xBD ); //-
	const KeyButtonState& lBracketKey		= g_theInput->GetKeyStates( LBRACKET_KEY );
	const KeyButtonState& rBracketKey		= g_theInput->GetKeyStates( RBRACKET_KEY );
	const KeyButtonState& commaKey			= g_theInput->GetKeyStates( COMMA_KEY );
	const KeyButtonState& periodKey			= g_theInput->GetKeyStates( PERIOD_KEY );
	const KeyButtonState& singleQuoteKey	= g_theInput->GetKeyStates( SINGLEQUOTE_KEY );
	const KeyButtonState& backSlashKey		= g_theInput->GetKeyStates( BACKSLASH_KEY );


	if( !m_isPolyDrawing )
	{
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
			m_camera->SetPosition( Vec3(0.f, 0.f, 0.f ) );
			//m_camera->m_position = Vec2( 0.f, 0.f );
			m_camera->SetProjectionOrthographic( Vec2( GAME_CAMERA_X, GAME_CAMERA_Y ), 0.f, 1.f );
			//m_camera->SetProjectionOrthographic( m_defaultCameraHeight );
		}

		if( plusKey.IsPressed() )
		{
			if( nullptr == m_draggingGameObject )
			{
				float gravity = m_physics->GetSceneGravity();
				gravity += 10.f * deltaSeconds;
				m_physics->SetSceneGravity( gravity );
			}
			else
			{
				Collider2D* draggingCollider = m_draggingGameObject->m_rigidbody->m_collider;
				float restitution = draggingCollider->m_physicsMaterial.m_restitution;
				restitution += deltaSeconds;
				draggingCollider->SetRestitution( restitution );
				float currentRestitution = draggingCollider->GetPhysicsMaterial().m_restitution;
				m_draggingGameObject->m_fillColor.a = (unsigned char)Interpolate( 0, 128, currentRestitution );
			}
		}

		if( minusKey.IsPressed() )
		{

			if( nullptr == m_draggingGameObject )
			{
				float gravity = m_physics->GetSceneGravity();
				gravity -= 10.f * deltaSeconds;
				m_physics->SetSceneGravity( gravity );
			}
			else
			{
				Collider2D* draggingCollider = m_draggingGameObject->m_rigidbody->m_collider;
				float restitution = draggingCollider->m_physicsMaterial.m_restitution;
				restitution -= deltaSeconds;
				draggingCollider->SetRestitution( restitution );
				float currentRestitution = draggingCollider->GetPhysicsMaterial().m_restitution;
				m_draggingGameObject->m_fillColor.a = (unsigned char)Interpolate(0, 128, currentRestitution );
			}
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
	}
	
	if( escKey.WasJustPressed() )
	{
		if( m_isPolyDrawing )
		{
			m_isPolyDrawing = false;
			m_polygonPoints.clear();
		}
		else
		{
			g_theApp->HandleQuitRequested();
		}
	}

	if( num1Key.WasJustPressed() )
	{
		if( !m_isPolyDrawing )
		{
			if( nullptr == m_draggingGameObject )
			{
				Rigidbody2D* rb = m_physics->CreateRigidBody();
				float randNum = m_rand.RollRandomFloatInRange( 5.f, 15.f );
				DiscCollider2D* dc = m_physics->CreateDiscCollider( Vec2( 0.f, 0.f ), randNum );
				rb->TakeCollider( dc );
				rb->SetPosition( m_mousePositionOnMainCamera );
				rb->SetSimulationMode( DYNAMIC );
				GameObject* gameObject = new GameObject( rb );

				m_gameObjects.push_back( gameObject );
			}
			else
			{
				m_draggingGameObject->m_rigidbody->SetSimulationMode( STATIC );
			}
		}
	}

	if( num2Key.WasJustPressed() )
	{
		if( nullptr == m_draggingGameObject )
		{
			if( m_isPolyValid && !m_isPolyDrawing )
			{
				m_polygonPoints.push_back( m_mousePositionOnMainCamera );
				m_isPolyDrawing = true;
			}
		}
		else
		{
			m_draggingGameObject->m_rigidbody->SetSimulationMode( KINEMATIC );
		}

	}

	if( num3Key.WasJustPressed() )
	{
		if( nullptr == m_draggingGameObject )
		{

		}
		else
		{
			m_draggingGameObject->m_rigidbody->SetSimulationMode( DYNAMIC );
		}
	}

	if( leftMouseButton.WasJustPressed() )
	{


		if( m_isPolyDrawing && m_isPolyValid )
		{
			m_polygonPoints.push_back( m_mousePositionOnMainCamera );
		}
		else
		{
			GrabDiscIfOverlap();
		}
	}

	if( leftMouseButton.WasJustReleased() )
	{
		ReleaseDisc();
	}

	if( rightMouseButton.WasJustPressed() )
	{
		Polygon2D newPoly(m_polygonPoints);
		if( newPoly.IsValid() && newPoly.IsConvex() )
		{
			Vec2 worldCenter = newPoly.GetCenterOfMass();
			Rigidbody2D* rb = m_physics->CreateRigidBody();
			PolygonCollider2D* pc = m_physics->CreatePolygonCollider( newPoly, Vec2(0.f, 0.f) );
			rb->TakeCollider( pc );
			rb->SetPosition( newPoly.GetCenterOfMass() );
			rb->SetSimulationMode( STATIC );
			GameObject* gameObject = new GameObject( rb );
			m_gameObjects.push_back( gameObject );

			m_isPolyDrawing = false;
			m_polygonPoints.clear();
		}
	}

	if( mouseWheelScroll != 0.f )
	{
		float cameraHeight = m_camera->m_outputSize.y;

		cameraHeight  += (mouseWheelScroll * 10.f);
		cameraHeight = Clampf(cameraHeight, 10.f, 1000.f);
		//float newOrienation = GetTurnedToward( orientation, orientationIncremented, 2.f );
		//m_camera->SetProjectionOrthographic(cameraHeight);
		Vec2 outputSize = Vec2( cameraHeight * m_camera->GetAspectRatio(), cameraHeight );
		m_camera->SetProjectionOrthographic( outputSize, 0.f, 1.f );
 	}

	//Change mass minimum of 0.001
	if( lBracketKey.IsPressed() )
	{
		if( nullptr != m_draggingGameObject )
		{
			Rigidbody2D* rb = m_draggingGameObject->m_rigidbody;
			float currentMass = rb->GetMass();
			float newMass = currentMass - 10.f * deltaSeconds;
			newMass = Clampf( newMass, 0.001f, 999999.f);
			
			rb->SetMass( newMass );
		}
	}
	if( rBracketKey.IsPressed() )
	{
		if( nullptr != m_draggingGameObject )
		{
			Rigidbody2D* rb = m_draggingGameObject->m_rigidbody;
			float currentMass = rb->GetMass();
			float newMass = currentMass + 10.f * deltaSeconds;
			newMass = Clampf( newMass, 0.001f, 999999.f );

			rb->SetMass( newMass );
		}
	}

	//Change friction between 0, 1
	if( commaKey.IsPressed() )
	{
		if( nullptr != m_draggingGameObject )
		{
			Rigidbody2D* rb = m_draggingGameObject->m_rigidbody;
			Collider2D* col = rb->m_collider;
			float currentFriction = col->GetFriction();
			float newFriction = currentFriction - 10.f * deltaSeconds;
			newFriction = Clampf( newFriction, 0.f, 1.f );

			col->SetFriction( newFriction );
		}
	}
	if( periodKey.IsPressed() )
	{
		if( nullptr != m_draggingGameObject )
		{
			Rigidbody2D* rb = m_draggingGameObject->m_rigidbody;
			Collider2D* col = rb->m_collider;
			float currentFriction = col->GetFriction();
			float newFriction = currentFriction + 10.f * deltaSeconds;
			newFriction = Clampf( newFriction, 0.f, 1.f );

			col->SetFriction( newFriction );
		}
	}

	//Change drag minimum of 0
	if( singleQuoteKey.IsPressed() )
	{
		if( nullptr != m_draggingGameObject )
		{
			Rigidbody2D* rb = m_draggingGameObject->m_rigidbody;
			float currentDrag = rb->GetDrag();
			float newDrag = currentDrag - 10.f * deltaSeconds;
			newDrag = Clampf( newDrag, 0.f, 999999.f );

			rb->SetDragCoefficient( newDrag );
		}
	}
	if( backSlashKey.IsPressed() )
	{
		if( nullptr != m_draggingGameObject )
		{
			Rigidbody2D* rb = m_draggingGameObject->m_rigidbody;
			float currentDrag = rb->GetDrag();
			float newDrag = currentDrag + 10.f * deltaSeconds;
			newDrag = Clampf( newDrag, 0.f, 999999.f );

			rb->SetDragCoefficient( newDrag );
		}
	}



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
			m_gameObjects[gameObjectIndex]->m_rigidbody->DisableRigidbody();
			
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
		m_draggingGameObject->m_rigidbody->EnableRigidbody();
		m_draggingGameObject->m_rigidbody->SetVelocity(m_currentMouseVelocity);
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

void Game::CheckBorderCollisions()
{
	AABB2 cameraBounds( m_camera->GetOrthoBottomLeft(), m_camera->GetOrthoTopRight() );
	Vec2 cameraIRange = Vec2( cameraBounds.maxs.x - cameraBounds.mins.x, 0.f );

	for( size_t objectIndex = 0; objectIndex < m_gameObjects.size(); objectIndex++ )
	{
		GameObject& gameObject = *m_gameObjects[objectIndex];
		if( nullptr == &gameObject )
		{
			continue;
		}
		Rigidbody2D& rigidbody = *gameObject.m_rigidbody;
		Collider2D& collider = *rigidbody.m_collider;

		Vec2 velocity = rigidbody.GetVelocity();

		if( collider.IsCollidingWithWall( m_bottomCameraBound ) )
		{

			velocity = Vec2( velocity.x, absFloat(velocity.y ) );
			rigidbody.SetVelocity( velocity );
		}

		eOffScreenDirection direction = gameObject.m_rigidbody->m_collider->IsOffScreen( cameraBounds );
		Vec2 currentPosition = rigidbody.GetPosition();
		AABB2 currentColliderBounds = collider.GetBounds();
		Vec2 colliderIRange = Vec2( currentColliderBounds.maxs.x - currentColliderBounds.mins.x, 0.f );
		Vec2 offset = cameraIRange + colliderIRange;

		if( direction == LEFTOFSCREEN && velocity.x <= 0.f )
		{
			rigidbody.SetPosition( currentPosition + offset );
		}
		else if( direction == RIGHTOFSCREEN && velocity.x >= 0.f )
		{
			rigidbody.SetPosition( currentPosition - offset );
		}
	}
}

void Game::SetCurrentMouseVelocity()
{
	Vec2 sumOfDeltaPositions;
	float sumOfDeltaTime = 0.f;
	for( size_t deltaIndex = 0; deltaIndex < m_mouseDeltaPositions.size(); deltaIndex++ )
	{
		sumOfDeltaPositions += m_mouseDeltaPositions[deltaIndex];
		sumOfDeltaTime += m_mouseDeltaTime[deltaIndex];
	}

	if( sumOfDeltaTime == 0.f )
	{
		m_currentMouseVelocity = Vec2(0.f, 0.f);
	}
	else
	{
		m_currentMouseVelocity = sumOfDeltaPositions/sumOfDeltaTime;
	}
}

void Game::UpdateCameraBounds()
{
	Vec2 bLeft = m_camera->GetOrthoBottomLeft();
	Vec2 tRight = m_camera->GetOrthoTopRight();
	Vec2 bRight( tRight.x, bLeft.y );
	Vec2 tLeft( bLeft.x, tRight.y );

	m_leftCameraBound.startPosition = bLeft;
	m_leftCameraBound.endPosition = tLeft;

	m_rightCameraBound.startPosition = bRight;
	m_rightCameraBound.endPosition = tRight;

	m_bottomCameraBound.startPosition = bLeft;
	m_bottomCameraBound.endPosition = bRight;
}
