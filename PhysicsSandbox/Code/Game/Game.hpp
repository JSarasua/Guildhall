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

#include <queue>
#include <vector>


class Physics2D;
class GameObject;
class Clock;

class Game
{
public:
	Game();
	~Game();
	void Startup();
	void Shutdown();
	void RunFrame();

	void Update();
	void Render();

public:
	RandomNumberGenerator m_rand;

private:
	void CheckCollisions();
	void UpdateGameObjects( float deltaSeconds );
	void UpdateDebugMouse(float deltaSeconds );
	void UpdateCameraBounds();

	void RenderDebugMouse() const;
	void RenderUI() const;
	void CheckButtonPresses( float deltaSeconds );



	void RenderGameObjects();
	void GrabDiscIfOverlap();
	void ReleaseDisc();
	int GetGameObjectIndex( GameObject* gameObject );
	void CheckBorderCollisions();
	void SetCurrentMouseVelocity();
private:
	Camera* m_UICamera;
	Camera* m_camera;

	Physics2D* m_physics = nullptr;


	Vec2 m_mousePositionOnMainCamera;


	Clock* m_gameClock = nullptr;

	GameObject*	m_draggingGameObject = nullptr;
	GameObject* m_hoveringOverGameObject = nullptr;
	float m_defaultCameraHeight = 0.f;
	Vec2 m_draggingOffset;
	std::vector<GameObject*> m_gameObjects;
	std::vector<Vec2> m_polygonPoints;

	Vec2 m_currentMouseVelocity;
	Vec2 m_mouseLastPosition;
	std::vector<Vec2> m_mouseDeltaPositions;
	std::vector<float> m_mouseDeltaTime;
	size_t m_mouseDeltaIndex = 0;

	bool m_isPolyValid = false;
	bool m_isPolyDrawing = false;

	LineSegment2 m_leftCameraBound;
	LineSegment2 m_rightCameraBound;
	LineSegment2 m_bottomCameraBound;
};
