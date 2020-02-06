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


class Physics2D;
class GameObject;

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
	void UpdateGameObjects( float deltaSeconds );
	void UpdateDebugMouse();

	void RenderDebugMouse() const;
	void RenderUI() const;
	void CheckButtonPresses( float deltaSeconds );



	void RenderGameObjects();
	void GrabDiscIfOverlap();
	void ReleaseDisc();
	int GetGameObjectIndex( GameObject* gameObject );
private:
	Camera* m_UICamera;
	Camera* m_camera;

	Physics2D* m_physics = nullptr;


	Vec2 m_mousePositionOnMainCamera;




	GameObject*	m_draggingGameObject = nullptr;
	float m_defaultCameraHeight = 0.f;
	Vec2 m_draggingOffset;
	std::vector<GameObject*> m_gameObjects;
	std::vector<Vec2> m_polygonPoints;
};
