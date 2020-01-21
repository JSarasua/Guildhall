#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <string>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"

/*
<TileDefinition
name="Grass"
spriteCoords="0,1"
spriteTint="180,180,180"
allowsSight="true"
allowsWalking="true"
allowsFlying="true"
allowsSwimming="false"
/>
*/



class TileDefinition
{
	friend class Tile;
	friend class Map;
	friend class MapGenStep;
	friend class MapGenStep_Mutate;
public:
	TileDefinition() = delete;
	TileDefinition(const XMLElement& element);
	~TileDefinition(){}

	static void InitializeTileDefinitions(const XMLElement& rootTileDefelement);
	static TileDefinition* GetTileDefFromColor(const Rgba8& colorValue);
	static std::map< std::string, TileDefinition*> s_definitions;



protected:
	std::string m_name;
	AABB2 m_spriteUVs;
	Rgba8 m_tint;
	Rgba8 m_tileGlyphColor;
	bool m_allowsSight = false;
	bool m_allowsWalking = false;
	bool m_allowsFlying = false;
	bool m_allowsSwimming = false;


};

typedef std::map< std::string, TileDefinition*>::iterator TileDefIterator;