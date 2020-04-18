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

	float GetRadius() const;
	Vec3 const& GetPosition() const;

public:
	Vec3 m_velocity;
	Transform m_transform;

private:

	GPUMesh* m_mesh = nullptr;
};