#pragma once
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/World.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;

Game::Game()
{
	m_camera = Camera();
	m_UICamera = Camera();
}

Game::~Game(){}

void Game::Startup()
{
	m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(GAME_CAMERA_Y * CLIENT_ASPECT, GAME_CAMERA_Y));
	m_UICamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(GAME_CAMERA_Y * CLIENT_ASPECT, GAME_CAMERA_Y));

	m_OBB2AtMouse = OBB2(Vec2(0.f,0.f), Vec2(3.5f,7.4f), 0.f);

	SetShapePositionsAndColors();

	g_theRenderer->SetBlendMode(BlendMode::ADDITIVE);
}

void Game::Shutdown(){}

void Game::RunFrame(){}

void Game::Update( float deltaSeconds )
{
	UpdateDebugMouse();
	UpdateNearestPoints();
	CheckButtonPresses( deltaSeconds );
}

void Game::Render()
{
	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	g_theRenderer->BeginCamera( m_camera );
	RenderGame();
	RenderDebugMouse();
	g_theRenderer->EndCamera( m_camera );


// 	g_theRenderer->BeginCamera( m_UICamera );
// 	RenderUI();
// 	g_theRenderer->EndCamera( m_UICamera );

}


void Game::CheckCollisions()
{}

void Game::UpdateEntities( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Game::UpdateDebugMouse()
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedPos();
	AABB2 orthoBounds( m_camera.GetOrthoBottomLeft(), m_camera.GetOrthoTopRight() );
	Vec2 mouseDrawPosOnCamera = orthoBounds.GetPointAtUV( mouseNormalizedPos );

	m_mousePositionOnMainCamera = mouseDrawPosOnCamera;

	m_OBB2AtMouse.SetCenter(m_mousePositionOnMainCamera);

}

void Game::UpdateNearestPoints()
{
	m_aabbNearestPointToMouse			= m_aabb.GetNearestPoint(m_mousePositionOnMainCamera);
	m_obbNearestPointToMouse			= m_obb.GetNearestPoint(m_mousePositionOnMainCamera);
	m_capsuleNearestPointToMouse		= m_capsule.GetNearestPoint(m_mousePositionOnMainCamera);
	m_lineSegmentNearestPointToMouse	= m_lineSegment.GetNearestPoint(m_mousePositionOnMainCamera);
	m_discNearestPointToMouse			= GetNearestPointOnDisc2D(m_mousePositionOnMainCamera, m_discCenter, m_discRadius);
}

void Game::RenderGame()
{
	std::vector<Vertex_PCU> vertexList;

	AppendShapes(vertexList);
	AppendLines(vertexList);

	g_theRenderer->DrawVertexArray(vertexList);

	g_theRenderer->DrawRing(m_lineSegmentNearestPointToMouse, 0.2f, Rgba8::RED, 0.2f);
	if( m_aabb.IsPointInside( m_mousePositionOnMainCamera ) )
	{
		g_theRenderer->DrawRing(m_aabbNearestPointToMouse, 0.6f, Rgba8::WHITE, 0.6f);
	}
	else
	{
		g_theRenderer->DrawRing(m_aabbNearestPointToMouse, 0.2f, Rgba8::RED, 0.2f);
	}

	if( m_capsule.IsPointInside( m_mousePositionOnMainCamera ) )
	{
		g_theRenderer->DrawRing(m_capsuleNearestPointToMouse, 0.6f, Rgba8::WHITE, 0.6f);
	}
	else
	{
		g_theRenderer->DrawRing(m_capsuleNearestPointToMouse, 0.2f, Rgba8::RED, 0.2f);
	}

	if( m_obb.IsPointInside( m_mousePositionOnMainCamera ) )
	{
		g_theRenderer->DrawRing(m_obbNearestPointToMouse, 0.6f, Rgba8::WHITE, 0.6f);
	}
	else
	{
		g_theRenderer->DrawRing(m_obbNearestPointToMouse, 0.2f, Rgba8::RED, 0.2f);
	}

	if( IsPointInsideDisc2D( m_mousePositionOnMainCamera, m_discCenter, m_discRadius ) )
	{
		g_theRenderer->DrawRing(m_discNearestPointToMouse, 0.6f, Rgba8::WHITE, 0.6f);
	}
	else
	{
		g_theRenderer->DrawRing(m_discNearestPointToMouse, 0.2f, Rgba8::RED, 0.2f);
	}


}

void Game::AppendShapes(std::vector<Vertex_PCU>& masterVertexList)
{
	if( m_UseOBB2AtMouse && DoOBBAndCapsuleOverlap2D( m_OBB2AtMouse, m_capsule ) )
	{
		Rgba8 capsuleColor = m_capsuleColor;
		capsuleColor.r += 50;
		capsuleColor.g += 50;
		capsuleColor.b += 50;
		Vertex_PCU::AppendVertsCapsule2D( masterVertexList, m_capsule, capsuleColor );
	}
	else
	{
		Vertex_PCU::AppendVertsCapsule2D( masterVertexList, m_capsule, m_capsuleColor );
	}


	//#TODO FIX DOOBBANDLINESEGMENTOVERLAP2D
	if( m_UseOBB2AtMouse && DoOBBAndLineSegmentOverlap2D( m_OBB2AtMouse, m_lineSegment ) )
	{
		Rgba8 lineSegmentColor = m_lineSegmentColor;
		lineSegmentColor.r += 50;
		lineSegmentColor.g += 50;
		lineSegmentColor.b += 50;
		Vertex_PCU::AppendVertsLine2D( masterVertexList, m_lineSegment, 0.5f, lineSegmentColor );
	}
	else
	{
		Vertex_PCU::AppendVertsLine2D( masterVertexList, m_lineSegment, 0.5f, m_lineSegmentColor );
	}

	if( m_UseOBB2AtMouse && DoOBBAndAABBOverlap2D( m_OBB2AtMouse, m_aabb ) )
	{
		Rgba8 abbColor = m_aabbColor;
		abbColor.r += 50;
		abbColor.g += 50;
		abbColor.b += 50;
		Vertex_PCU::AppendVertsAABB2D( masterVertexList, m_aabb, abbColor );
	}
	else
	{
		Vertex_PCU::AppendVertsAABB2D( masterVertexList, m_aabb, m_aabbColor );
	}

	if( m_UseOBB2AtMouse && DoOBBAndOBBOverlap2D( m_OBB2AtMouse, m_obb ) )
	{
		Rgba8 obbColor = m_obbColor;
		obbColor.r += 50;
		obbColor.g += 50;
		obbColor.b += 50;
		Vertex_PCU::AppendVertsOBB2D( masterVertexList, m_obb, obbColor );
	}
	else
	{
		Vertex_PCU::AppendVertsOBB2D( masterVertexList, m_obb, m_obbColor );
	}

	if( m_UseOBB2AtMouse && DoOBBAndDiscOverlap2D( m_OBB2AtMouse, m_discCenter, m_discRadius ) )
	{
		Rgba8 discColor = m_discColor;
		discColor.r += 50;
		discColor.g += 50;
		discColor.b += 50;
		Vertex_PCU::AppendVertsDisc2D( masterVertexList, m_discCenter, m_discRadius, discColor );
	}
	else
	{
		Vertex_PCU::AppendVertsDisc2D( masterVertexList, m_discCenter, m_discRadius, m_discColor );
	}

	//TODO ADD DISC
}

void Game::AppendLines( std::vector<Vertex_PCU>& masterVertexList )
{
	LineSegment2 lineToMouse	= LineSegment2( m_lineSegmentNearestPointToMouse, m_mousePositionOnMainCamera );
	LineSegment2 aabbToMouse	= LineSegment2( m_aabbNearestPointToMouse, m_mousePositionOnMainCamera );
	LineSegment2 capsuleToMouse = LineSegment2( m_capsuleNearestPointToMouse, m_mousePositionOnMainCamera );
	LineSegment2 obbToMouse		= LineSegment2( m_obbNearestPointToMouse, m_mousePositionOnMainCamera );
	LineSegment2 discToMouse	= LineSegment2( m_discNearestPointToMouse, m_mousePositionOnMainCamera );

	Vertex_PCU::AppendVertsLine2D( masterVertexList, lineToMouse, 0.1f, Rgba8::GREY );
	Vertex_PCU::AppendVertsLine2D( masterVertexList, aabbToMouse, 0.1f, Rgba8::GREY );
	Vertex_PCU::AppendVertsLine2D( masterVertexList, capsuleToMouse, 0.1f, Rgba8::GREY );
	Vertex_PCU::AppendVertsLine2D( masterVertexList, obbToMouse, 0.1f, Rgba8::GREY );
	Vertex_PCU::AppendVertsLine2D( masterVertexList, discToMouse, 0.1f, Rgba8::GREY );

	//TODO ADD DISC
}

void Game::RenderDebugMouse() const
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedPos();

	AABB2 orthoBounds( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
	//Should pass in camera
	//Use UV because our value is between 0 to 1
	Vec2 mouseDrawPos = orthoBounds.GetPointAtUV( mouseNormalizedPos );

	if( m_UseOBB2AtMouse )
	{
		std::vector<Vertex_PCU> vertexList;
		Vertex_PCU::AppendVertsOBB2D(vertexList, m_OBB2AtMouse, Rgba8::GREEN);
		g_theRenderer->DrawVertexArray(vertexList);
	}
	else
	{
		g_theRenderer->DrawRing( mouseDrawPos, 0.1f, Rgba8::GREEN, 0.1f );
	}

}

void Game::RenderUI() const
{
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	const XboxController& controller = g_theInput->GetXboxController(0);

	const KeyButtonState& rKey = g_theInput->GetKeyStates('R');
	const KeyButtonState& tKey = g_theInput->GetKeyStates('T');
	const KeyButtonState& yKey = g_theInput->GetKeyStates('Y');
	const KeyButtonState& leftMouseButton = g_theInput->GetMouseButton(LeftMouseButton);
	const KeyButtonState& rightMouseButton = g_theInput->GetMouseButton(RightMouseButton);
	float mouseWheelScroll = g_theInput->GetDeltaMouseWheelScroll();

	if( rKey.WasJustPressed() )
	{
		SetShapePositionsAndColors();
	}

	if( tKey.WasJustPressed() )
	{
		m_UseOBB2AtMouse = !m_UseOBB2AtMouse;
	}

	if( yKey.IsPressed() )
	{
		float orientation = m_OBB2AtMouse.GetOrientationDegrees();
		float orientationIncremented = orientation + 30.f;
		float newOrienation = GetTurnedToward(orientation, orientationIncremented, 2.f );
		m_OBB2AtMouse.SetOrientationDegrees(newOrienation);
	}

	if( leftMouseButton.WasJustPressed() )
	{
		m_UseOBB2AtMouse = !m_UseOBB2AtMouse;
	}
	else if( rightMouseButton.WasJustPressed() )
	{
		SetShapePositionsAndColors();
	}


	if( mouseWheelScroll != 0.f )
	{
		float orientation = m_OBB2AtMouse.GetOrientationDegrees();
		float orientationIncremented = orientation + (mouseWheelScroll * 10.f);
		//float newOrienation = GetTurnedToward( orientation, orientationIncremented, 2.f );
		m_OBB2AtMouse.SetOrientationDegrees( orientationIncremented );
	}

	UNUSED( deltaSeconds );
	UNUSED( controller );
}

void Game::SetShapePositionsAndColors()
{
	SetLineSegment();
	SetCapsule();
	SetAABB2();
	SetOBB2();
	SetDisc();
}

void Game::SetLineSegment()
{
	AABB2 gameCameraAABB = AABB2( m_camera.GetOrthoBottomLeft(), m_camera.GetOrthoTopRight() );
	FloatRange gameCameraXRange = FloatRange( gameCameraAABB.mins.x, gameCameraAABB.maxs.x );
	FloatRange gameCameraYRange = FloatRange( gameCameraAABB.mins.y, gameCameraAABB.maxs.y );

	Vec2 lineSegmentStart = Vec2( gameCameraXRange.GetRandomInRange( m_rand ), gameCameraYRange.GetRandomInRange( m_rand ) );
	float lineSegmentAngle = m_rand.RollRandomFloatInRange(0.f, 360.f);
	float lineSegmentLength = m_rand.RollRandomFloatInRange(GAME_CAMERA_Y/15.f, GAME_CAMERA_Y/10.f);
	Vec2 lineSegmentEnd = Vec2::MakeFromPolarDegrees(lineSegmentAngle, lineSegmentLength) + lineSegmentStart;

	m_lineSegment = LineSegment2(lineSegmentStart, lineSegmentEnd);

	m_lineSegmentColor.r = (unsigned char)m_rand.RollRandomIntInRange(70,128);
	m_lineSegmentColor.g = (unsigned char)m_rand.RollRandomIntInRange(70,128);
	m_lineSegmentColor.b = (unsigned char)m_rand.RollRandomIntInRange(70,128);
}

void Game::SetCapsule()
{
	AABB2 gameCameraAABB = AABB2( m_camera.GetOrthoBottomLeft(), m_camera.GetOrthoTopRight() );
	FloatRange gameCameraXRange = FloatRange( gameCameraAABB.mins.x, gameCameraAABB.maxs.x );
	FloatRange gameCameraYRange = FloatRange( gameCameraAABB.mins.y, gameCameraAABB.maxs.y );

	Vec2 lineSegmentStart = Vec2( gameCameraXRange.GetRandomInRange( m_rand ), gameCameraYRange.GetRandomInRange( m_rand ) );
	float lineSegmentAngle = m_rand.RollRandomFloatInRange( 0.f, 360.f );
	float lineSegmentLength = m_rand.RollRandomFloatInRange( GAME_CAMERA_Y/15.f, GAME_CAMERA_Y/10.f );
	Vec2 lineSegmentEnd = Vec2::MakeFromPolarDegrees( lineSegmentAngle, lineSegmentLength ) + lineSegmentStart;
	float lineSegmentRadius = m_rand.RollRandomFloatInRange(1.f, 15.f);

	m_capsule = Capsule2(lineSegmentStart, lineSegmentEnd, lineSegmentRadius);

	m_capsuleColor.r = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
	m_capsuleColor.g = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
	m_capsuleColor.b = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
}

void Game::SetAABB2()
{
	AABB2 gameCameraAABB = AABB2( m_camera.GetOrthoBottomLeft(), m_camera.GetOrthoTopRight() );
	FloatRange gameCameraXRange = FloatRange( gameCameraAABB.mins.x, gameCameraAABB.maxs.x );
	FloatRange gameCameraYRange = FloatRange( gameCameraAABB.mins.y, gameCameraAABB.maxs.y );

	Vec2 aabbCenter = Vec2( gameCameraXRange.GetRandomInRange( m_rand ), gameCameraYRange.GetRandomInRange( m_rand ) );
	Vec2 aabbMaxOffset = Vec2( m_rand.RollRandomFloatInRange(5.f, 15.f), m_rand.RollRandomFloatInRange(5.f, 15.f));
	Vec2 aabbMinOffset = Vec2( m_rand.RollRandomFloatInRange(-15.f, -5.f), m_rand.RollRandomFloatInRange(-15.f, -5.f));
	m_aabb.maxs = aabbMaxOffset;
	m_aabb.mins = aabbMinOffset;
	m_aabb.SetCenter( aabbCenter );

	m_aabbColor.r = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
	m_aabbColor.g = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
	m_aabbColor.b = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
}

void Game::SetOBB2()
{
	AABB2 gameCameraAABB = AABB2( m_camera.GetOrthoBottomLeft(), m_camera.GetOrthoTopRight() );
	FloatRange gameCameraXRange = FloatRange( gameCameraAABB.mins.x, gameCameraAABB.maxs.x );
	FloatRange gameCameraYRange = FloatRange( gameCameraAABB.mins.y, gameCameraAABB.maxs.y );

	Vec2 aabbCenter = Vec2( gameCameraXRange.GetRandomInRange( m_rand ), gameCameraYRange.GetRandomInRange( m_rand ) );
	Vec2 dimension = Vec2( m_rand.RollRandomFloatInRange(5.f, 15.f), m_rand.RollRandomFloatInRange(5.f, 15.f) );
	float orientationDegrees = m_rand.RollRandomFloatInRange(0.f, 360.f);
	//float orientationDegrees = 0.f;

	m_obb = OBB2(aabbCenter, dimension, orientationDegrees);

	m_obbColor.r = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
	m_obbColor.g = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
	m_obbColor.b = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
}

void Game::SetDisc()
{
	AABB2 gameCameraAABB = AABB2( m_camera.GetOrthoBottomLeft(), m_camera.GetOrthoTopRight() );
	FloatRange gameCameraXRange = FloatRange( gameCameraAABB.mins.x, gameCameraAABB.maxs.x );
	FloatRange gameCameraYRange = FloatRange( gameCameraAABB.mins.y, gameCameraAABB.maxs.y );

	m_discCenter = Vec2( gameCameraXRange.GetRandomInRange( m_rand ), gameCameraYRange.GetRandomInRange( m_rand ) );
	m_discRadius = m_rand.RollRandomFloatInRange( 1.f, 15.f );


	m_discColor.r = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
	m_discColor.g = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
	m_discColor.b = (unsigned char)m_rand.RollRandomIntInRange( 70, 128 );
}
