#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <vector>


class Clock;
class GPUMesh;
class Material;
class Player;
class Shader;
class ShaderState;
class World;

struct light_t;
struct Vertex_PCUTBN;

struct fresnel_t
{
	Vec3 color;
	float power;
	float factor;

	Vec3 pad00;
};

struct dissolve_t
{
	Vec3 edgeNearColor;
	float amount;
	Vec3 edgeFarColor;
	float edgeWidth;
};

struct projection_t
{
	Mat44 projection;

	Vec3 position;
	//float pad00;

	//Vec3 normal;
	float strength;
};

struct transformColor_t
{
	Mat44 transformColor;

	Vec3 tint;
	float tintPower = 0.f;

	Vec3 pad00;
	float transformPower = 0.f;
};

enum PowerUps
{
	RAPID_FIRE,
	BOUNCING_BULLET,
	SCATTER_SHOT,


	NUM_POWERUPS
};

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


private:
	void AppendIndexedVertsTestCube( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList );

	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);
	IntVec2 GetCurrentMapBounds() const;

private:
	Mat44 m_frontCubeModelMatrix;
	Mat44 m_leftCubeModelMatrix;
	Mat44 m_frontleftCubeModelMatrix;


	Clock* m_gameClock = nullptr;

	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	Camera m_camera;
	Vec2 m_cameraPosition;

	int m_currentLevel = 0;

	World* m_world = nullptr;
	Player* m_player = nullptr;

	GPUMesh* m_cubeMesh = nullptr;
	Texture* m_screenTexture = nullptr;

	size_t m_currentShaderIndex = 0;
	size_t m_currentRenderTextureIndex = 0;
	size_t m_currentNormalTextureIndex = 0;


	float m_numTilesInViewVertically = 0.f;
	float m_numTilesInViewHorizontally = 0.f;


	ShaderState* m_testShaderState = nullptr;
	Material* m_testMaterial = nullptr;

	bool m_isBloomActive = false;

public:
	Rgba8 m_clearColor = Rgba8::BLACK;
	float m_currentTime = 0.f;
	RandomNumberGenerator m_rand;

private:
};
