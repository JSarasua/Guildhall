#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <map>
#include <string>

class MapDefinition
{
	friend class Map;
public:
	MapDefinition() = delete;
	MapDefinition( const XmlElement& element );
	~MapDefinition() {}

	static void InitializeMapDefinitions( const XmlElement& rootMapDefElement );

	static std::map< std::string, MapDefinition*> s_definitions;

protected:
	std::string m_name;
	IntVec2 m_mapDimensions;
};