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

	float GetRadius() const;
	Vec3 const& GetPosition() const;

public:
	Vec3 m_velocity;
	Transform m_transform;

private:

	GPUMesh* m_mesh = nullptr;


};