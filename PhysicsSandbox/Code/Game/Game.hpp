#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>



class Game
{
public:
	Game();
	~Game();
	void Startup();
	void Shutdown();
	void RunFrame();

	void Update( float deltaSeconds );
	void Render();

public:
	RandomNumberGenerator m_rand;

private:
	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateDebugMouse();
	void UpdateNearestPoints();
	void RenderGame();
	void AppendShapes( std::vector<Vertex_PCU>& masterVertexList );
	void AppendLines( std::vector<Vertex_PCU>& masterVertexList );
	void RenderDebugMouse() const;
	void RenderUI() const;
	void CheckButtonPresses( float deltaSeconds );

	void SetShapePositionsAndColors();
	void SetLineSegment();
	void SetCapsule();
	void SetAABB2();
	void SetOBB2();
	void SetDisc();

private:
	Camera m_UICamera;
	Camera m_camera;

	Vec2 m_mousePositionOnMainCamera;

	AABB2			m_aabb;
	OBB2			m_obb;
	Capsule2		m_capsule;
	LineSegment2	m_lineSegment;
	Vec2			m_discCenter;
	float			m_discRadius;

	Rgba8			m_aabbColor;
	Rgba8			m_obbColor;
	Rgba8			m_capsuleColor;
	Rgba8			m_lineSegmentColor;
	Rgba8			m_discColor;

	Vec2			m_aabbNearestPointToMouse;
	Vec2			m_obbNearestPointToMouse;
	Vec2			m_capsuleNearestPointToMouse;
	Vec2			m_lineSegmentNearestPointToMouse;
	Vec2			m_discNearestPointToMouse;

	bool			m_UseOBB2AtMouse = false;
	OBB2			m_OBB2AtMouse;

	// 	Vec2			m_OBB2NearestPointToAABB;
	// 	Vec2			m_OBB2NearestPointToOBB;
	// 	Vec2			m_OBB2NearestPointToCapsule;
	// 	Vec2			m_OBB2NearestPointToLineSegment;
};
