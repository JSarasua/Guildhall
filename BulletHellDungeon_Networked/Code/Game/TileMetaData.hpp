#pragma once
#include "Engine/Core/Tags.hpp"
#include "Engine/Math/FloatRange.hpp"

struct TileMetaData
{
public:
	Tags m_tags;
	float m_heat = 0.f;

public:
	TileMetaData() = default;

	bool IsHeatInRange(FloatRange heatRange);
};