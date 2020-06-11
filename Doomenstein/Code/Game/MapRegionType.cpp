#include "Game/MapRegionType.hpp"

std::map< std::string, MapRegionType*> MapRegionType::s_definitions;

MapRegionType::MapRegionType( XmlElement const& element )
{
	m_name = ParseXMLAttribute( element, "name", "INVALID" );
	m_isSolid = ParseXMLAttribute( element, "isSolid", false );
}

bool MapRegionType::IsSolid() const
{
	return m_isSolid;
}

void MapRegionType::InitializeMapRegionDefinitions( const XmlElement& rootMapRegionElement )
{
	for( const XmlElement* element = rootMapRegionElement.FirstChildElement(); element; element=element->NextSiblingElement() ) {
		const XmlAttribute* nameAttribute = element->FindAttribute( "name" );

		if( nameAttribute )
		{
			std::string mapRegionName = nameAttribute->Value();
			MapRegionType* mapRegion = new MapRegionType( *element );
			s_definitions[mapRegionName] = mapRegion;
		}
	}
}

MapRegionType* MapRegionType::GetMapRegionTypeByString( std::string const& mapRegionName )
{
	auto mapRegionIter = s_definitions.find( mapRegionName );
	GUARANTEE_OR_DIE( mapRegionIter != s_definitions.end(), "Couldn't find map region by name" );

	MapRegionType* mapRegionType = mapRegionIter->second;
	return mapRegionType;
}

