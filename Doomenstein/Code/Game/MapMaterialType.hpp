#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include <map>
#include <string>

class MapMaterialType
{
public:
	MapMaterialType() = delete;
	MapMaterialType( XmlElement const& element );
	~MapMaterialType() {}

	static void InitializeMapMaterialDefinitions( const XmlElement& rootMapMaterialElement );

	static std::map< std::string, MapMaterialType*> s_definitions;

protected:
	std::string m_name;
	
	//Floor
	IntVec2 m_floorSpriteUVs;
	//Ceiling
	IntVec2 m_ceilingSpriteUVs;
	//Side
	IntVec2 m_sideSpriteUVs;
};