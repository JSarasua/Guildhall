#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Transform.hpp"

class GPUMesh;

class Companion3D
{
public:
	Companion3D();
	~Companion3D();

	void Startup();
	void Update( float deltaSeconds );
	void Render();

private:
	Transform m_transform;
	Vec3 m_velocity;

	GPUMesh* m_mesh = nullptr;
};