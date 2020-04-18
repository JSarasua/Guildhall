#pragma once
#include "Engine/Math/Transform.hpp"

class Player3D
{
public:
	Player3D();
	~Player3D();

private:
	Transform m_transform;
};