#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <string>
#include "Engine/Math/IntVec2.hpp"

class MapDefinition
{
	friend class Map;
public:
	MapDefinition() = delete;
	MapDefinition( const XMLElement& element);
	~MapDefinition(){}

	static void InitializeMapDefinitions( const XMLElement& rootMapDefElement);

	static std::map< std::string, MapDefinition*> s_definitions;

protected:
	std::string m_name;
	IntVec2 m_mapDimensions;
	std::string m_fillTile;
	std::string m_edgeTile;
};