#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Time/Timer.hpp"

class GPUMesh;
class RandomNumberGenerator;

class Companion3D
{
public:
	Companion3D( RandomNumberGenerator* rand, GPUMesh* mesh );
	~Companion3D();

	void Startup();
	void Update( float deltaSeconds );
	void Render();


	void Respawn();
	void ActivateDissolve();
	void CheckForDissolve();
	void CheckForRespawn();

	bool IsDissolving();
	bool IsRespawning();

	float GetRadius() const;
	Vec3 const& GetPosition() const;

public:
	Vec3 m_velocity;
	Transform m_transform;

private:
	RandomNumberGenerator* m_rand = nullptr;
	Timer m_dissolveTimer;
	Timer m_respawnTimer;
	GPUMesh* m_mesh = nullptr;


};