#include "Game/MapGenStep.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/MapGenStep_Mutate.hpp"
#include "Game/MapGenStep_Worm.hpp"
#include "Game/MapGenStep_CellularAutomata.hpp"
#include "Game/MapGenStep_FromImage.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/TileDefinition.hpp"

MapGenStep::MapGenStep( const XmlElement& element )
{
	std::string ifTileTypeName = ParseXMLAttribute( element, "ifTileType", "INVALID" );
	std::string changeToTypeName = ParseXMLAttribute( element, "changeToType", "INVALID" );
	m_chanceToMutate	= ParseXMLAttribute( element, "chanceToMutate", 0.3f );
	m_chanceToRun		= ParseXMLAttribute(element, "chanceToRun", m_chanceToRun);
	m_iterations		= ParseXMLAttribute(element, "iterations", m_iterations);
	std::string ifTags	= ParseXMLAttribute(element, "ifTags", "");
	std::string setTags = ParseXMLAttribute(element, "setTags", "");
	m_ifHeatWithin		= ParseXMLAttribute(element, "ifHeatWithin", FloatRange(-999999,999999));
	m_setHeat			= ParseXMLAttribute(element, "setHeat", 0.f);

	std::vector<std::string> ifTagsVector = SplitStringOnDelimeter(ifTags.c_str(), ',');
	std::vector<std::string> setTagsVector = SplitStringOnDelimeter(setTags.c_str(), ',');


	if( ifTileTypeName != "INVALID" )
	{
		m_ifTileType		= TileDefinition::s_definitions[ifTileTypeName.c_str()];
	}
	if( changeToTypeName != "INVALID" )
	{
		m_changeToTileType	= TileDefinition::s_definitions[changeToTypeName.c_str()];
	}

	if( ifTagsVector.size() != 1 || ifTagsVector[0] != "" )
	{
		m_ifTags = Tags(ifTagsVector);

	}

	if( setTagsVector.size() != 1 || setTagsVector[0] != "")
	{
		m_setTags = Tags( setTagsVector );
	}
}

MapGenStep* MapGenStep::CreateNewMapGenStep( const XmlElement& element )
{
	std::string mapGenStepName = element.Name();


	MapGenStep* mapGenStep = nullptr;
	if(mapGenStepName == "Mutate" ) mapGenStep = new MapGenStep_Mutate(element);
	else if(mapGenStepName == "Worm" ) mapGenStep = new MapGenStep_Worm(element);
	else if(mapGenStepName == "CellularAutomata" ) mapGenStep = new MapGenStep_CellularAutomata(element);
	else if(mapGenStepName == "FromImage" ) mapGenStep = new MapGenStep_FromImage(element);

	return mapGenStep;
}

void MapGenStep::RunStep( Map& mapToMutate )
{
	if( !g_theGame->m_rand.RollPercentChance( m_chanceToRun ) )
	{
		return;
	}

	for( int iteration = 0; iteration < m_iterations; iteration++ )
	{
		RunStepOnce(mapToMutate);
	}
}

