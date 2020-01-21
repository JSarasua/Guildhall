#pragma once
#include <vector>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"

class Texture;

class TileDefinition
{
public:
	TileDefinition();
	~TileDefinition(){}

	void InitializeTileDefinitions(const Texture& texture);

	bool IsSolid() { return m_isSolid; }

	static std::vector<TileDefinition> s_definitions;

	Vec2 m_uvMins;
	Vec2 m_uvMaxs;
	Rgba8 tint;

protected:
	bool m_isSolid = false;
};

