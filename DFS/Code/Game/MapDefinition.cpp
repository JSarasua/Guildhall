#include "Game/MapDefinition.hpp"
#include "Game/MapGenStep.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

std::map< std::string, MapDefinition*> MapDefinition::s_definitions;

MapDefinition::MapDefinition( const XmlElement& element )
{
	m_name				= ParseXMLAttribute( element, "name", "INVALID" );
	m_mapDimensions.x	= ParseXMLAttribute( element, "width", 0 );
	m_mapDimensions.y	= ParseXMLAttribute( element, "height", 0 );
	m_fillTile			= ParseXMLAttribute( element, "fillTile", "INVALID" );
	m_edgeTile			= ParseXMLAttribute( element, "edgeTile", "INVALID" );

	const XmlElement* generationStepsElement = nullptr;
	for( const XmlElement* childElement = element.FirstChildElement(); childElement; childElement = childElement->NextSiblingElement() )
	{
		const std::string name( childElement->Name() );
		if( name == "GenerationSteps" )
		{
			generationStepsElement = childElement;
			break;
		}
	}

	GUARANTEE_OR_DIE(m_name != "INVALID", "NAME IS INVALID");
	GUARANTEE_OR_DIE(m_fillTile != "INVALID", "FILLTILE IS INVALID");
	GUARANTEE_OR_DIE(m_edgeTile != "INVALID", "EDGETILE IS INVALID");

	//Find generationStepElement
	if( generationStepsElement )
	{
		for( const XmlElement* childStepElement = generationStepsElement->FirstChildElement(); childStepElement; childStepElement = childStepElement->NextSiblingElement() )
		{
			m_mapGenSteps.push_back( MapGenStep::CreateNewMapGenStep( *childStepElement ) );
		}
	}
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

