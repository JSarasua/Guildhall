#include "Game/MapDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

std::map< std::string, MapDefinition*> MapDefinition::s_definitions;

MapDefinition::MapDefinition( const XmlElement& element )
{

}

void MapDefinition::InitializeMapDefinitions( const XmlElement& rootMapDefElement )
{
	for( const XmlElement* element = rootMapDefElement.FirstChildElement(); element; element=element->NextSiblingElement() ) {
		const XmlAttribute* nameAttribute = element->FindAttribute( "name" );
		if( nameAttribute )
		{
			std::string mapDefName = nameAttribute->Value();
			MapDefinition* mapDef = new MapDefinition( *element );
			s_definitions[mapDefName] = mapDef;
		}
	}
}

