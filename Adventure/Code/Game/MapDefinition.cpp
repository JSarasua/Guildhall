#include "Game/MapDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


std::map< std::string, MapDefinition*> MapDefinition::s_definitions;

MapDefinition::MapDefinition( const XMLElement& element )
{
	m_name				= ParseXMLAttribute( element, "name", "INVALID" );
	m_mapDimensions.x	= ParseXMLAttribute( element, "width", 0 );
	m_mapDimensions.y	= ParseXMLAttribute( element, "height", 0 );
	m_fillTile			= ParseXMLAttribute( element, "fillTile", "INVALID" );
	m_edgeTile			= ParseXMLAttribute( element, "edgeTile", "INVALID" );

	GUARANTEE_OR_DIE(m_name != "INVALID", "NAME IS INVALID");
	GUARANTEE_OR_DIE(m_fillTile != "INVALID", "FILLTILE IS INVALID");
	GUARANTEE_OR_DIE(m_edgeTile != "INVALID", "EDGETILE IS INVALID");


}

void MapDefinition::InitializeMapDefinitions( const XMLElement& rootMapDefElement )
{
	for( const XMLElement* element = rootMapDefElement.FirstChildElement(); element; element=element->NextSiblingElement() ) {
		const XMLAttribute* nameAttribute = element->FindAttribute( "name" );

		if( nameAttribute )
		{
			std::string mapDefName = nameAttribute->Value();
			MapDefinition* mapDef = new MapDefinition( *element );
			s_definitions[mapDefName] = mapDef;
		}
	}
}

