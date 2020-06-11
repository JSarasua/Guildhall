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

	void InitializeDefinitions();

	void Update();
	void Render();

	static bool SetAmbientColor(const EventArgs* args);
	static bool SetAttenuation(const EventArgs* args);
	static bool SetLightColor(const EventArgs* args);

	bool TestEventSystem( EventArgs const& args );
	bool TestEventSystem2( EventArgs const& args );

private:
	void IncrementCurrentLight();
	void DecrementCurrentLight();

	void AppendIndexedVertsTestCube( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList );

	void ToggleAttenuation();
	void ToggleBloom();
	void SetAttenuation( Vec3 const& newAttenuation );

	void UpdateLightPosition( float deltaSeconds );
	void EnableLights();

	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);
	IntVec2 GetCurrentMapBounds() const;
	void SetLightPosition( Vec3 const& pos );

	void FPSStartup();
	void FPSCounterUpdate();
	void FPSRender();

private:
	Mat44 m_frontCubeModelMatrix;
	Mat44 m_leftCubeModelMatrix;
	Mat44 m_frontleftCubeModelMatrix;

	//Mat44 m_loadedMeshModelMatrix;
	//Mat44 m_circleOfSpheresModelMatrix;
	//Mat44 m_quadModelMatrix;
	//Mat44 m_sphereModelMatrix;
	//Mat44 m_triPlanarSphereModelMatrix;
	//int m_numberOfCirclingCubes = 0;

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

	Shader* m_invertShader = nullptr;

	//std::vector<Shader*> m_shaders;

	size_t m_currentShaderIndex = 0;
	size_t m_currentRenderTextureIndex = 0;
	size_t m_currentNormalTextureIndex = 0;


	bool m_isLightFollowingCamera = false;
	bool m_isLightAnimated = false;

	float m_lightAnimatedTheta = 0.f;
	float m_lightAnimatedPhi = 0.f;
	Vec3 m_lightAnimatedPosition;

	float m_numTilesInViewVertically = 0.f;
	float m_numTilesInViewHorizontally = 0.f;

	float m_dissolveAmount = 0.f;
	float m_greyscaleAmount = 0.f;
	float m_tintAmount = 0.f;

	float m_fogDistance = 20.f;
	float m_fogRange = 10.f;
	Rgba8 m_fogRed = Rgba8( 50, 0, 0 );
	Rgba8 m_fogBlue = Rgba8( 0, 0, 50 );
	float m_fogColorLerp = 0.5f;

	bool m_isBloomActive = false;

	bool m_isDebugRenderingEnabled = true;

	double m_deltaSecondsFromLastFrame[10];
	int m_fpsCounterIndex = 0;
	float m_fps = 0.f;
public:
	//static light_t m_pointLight;
	static std::vector<light_t> m_lights;
	static uint m_currentLightIndex;
	
	Rgba8 m_clearColor = Rgba8::BLACK;
	float m_currentTime = 0.f;
	RandomNumberGenerator m_rand;

private:
	void RenderDevConsole();
};
