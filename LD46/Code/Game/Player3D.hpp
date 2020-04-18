#pragma once
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Vec3.hpp"

class GPUMesh;

class Player3D
{
public:
	Player3D();
	~Player3D();

	void Update( float deltaSeconds );
	void Render();

private:
	Transform m_transform;
	Vec3 m_velocity;

	GPUMesh* m_mesh = nullptr;
};