#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <vector>


class Player;
class World;
class Shader;
class GPUMesh;
class Clock;

struct light_t;

struct fresnel_t
{
	Vec3 color;
	float power;
	float factor;

	Vec3 pad00;
};

struct dissolve_t
{
	float amount;
	float edgeWidth;
	Vec3 edgeColor;

	Vec3 pad00;
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
	void RenderCircleOfSpheres();

	static bool SetAmbientColor(const EventArgs* args);
	static bool SetAttenuation(const EventArgs* args);
	static bool SetLightColor(const EventArgs* args);

private:

	void IncrementShader();
	void DecrementShader();
	void IncrementRenderTexture();
	void DecrementRenderTexture();
	void IncrementNormalTexture();
	void DecrementNormalTexture();
	void IncrementCurrentLight();
	void DecrementCurrentLight();

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

private:
	Mat44 m_cubeModelMatrix;
	Mat44 m_circleOfSpheresModelMatrix;
	Mat44 m_quadModelMatrix;
	Mat44 m_sphereModelMatrix;
	int m_numberOfCirclingCubes = 0;

	Clock* m_gameClock = nullptr;

	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	Camera m_camera;
	Vec2 m_cameraPosition;

	int m_currentLevel = 0;

	World* m_world = nullptr;
	Player* m_player = nullptr;
	GPUMesh* m_cubeMesh = nullptr;
	GPUMesh* m_sphereMesh = nullptr;
	GPUMesh* m_quadMesh = nullptr;
	Texture* m_screenTexture = nullptr;

	Shader* m_invertShader = nullptr;

	std::vector<Shader*> m_shaders;
	std::vector<Texture*> m_renderTextures;
	std::vector<Texture*> m_normalTextures;

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

	float m_dissolveAmount = 0.5;
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
