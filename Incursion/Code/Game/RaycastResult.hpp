#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/Tile.hpp"

class Entity;

struct RaycastResult
{
public:
	RaycastResult() {}
	~RaycastResult() {}


	bool m_didImpact = false;
	Vec2 m_startPos = Vec2(0.f, 0.f);
	Vec2 m_impactPos = Vec2(0.f,0.f);
	float m_impactDist = 0.f;
	Vec2 m_Normal = Vec2(0.f,0.f);
	TileType m_impactTileType = INVALID_TILETYPE;
	Entity* m_impactEntity = nullptr;
	float m_impactFraction = 0.f;
};
