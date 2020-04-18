#pragma once
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Vec3.hpp"

class GPUMesh;
struct Mat44;
class Golem;

class Player3D
{
public:
	Player3D();
	~Player3D();

	void Update( float deltaSeconds );
	void Render();

	float GetRadius() const;
	Vec3 const& GetPosition() const;
	void CheckButtonPresses(float deltaSeconds);

	Transform const& GetPlayerTransform() const;
	Mat44 GetPlayerHeadMatrix() const;
	void SetPosition( Vec3 const& position );

public:
	Vec3 m_velocity;
	Transform m_transform;

	Golem* m_golem = nullptr;

private:

	GPUMesh* m_mesh = nullptr;
};