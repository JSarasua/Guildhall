#include "Game/MapGenStep_Worm.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

MapGenStep_Worm::MapGenStep_Worm( const XmlElement& element ) : MapGenStep(element)
{
	m_wormLength = ParseXMLAttribute( element, "wormLength", 5 );
	std::string changeToTypeName = ParseXMLAttribute( element, "changeToType", "INVALID" );

	GUARANTEE_OR_DIE(changeToTypeName != "INVALID", "MapGenStep_Worm requires a 'changeToType' parameter");

	m_changeToTileType	= TileDefinition::s_definitions[changeToTypeName.c_str()];
}

void MapGenStep_Worm::RunStepOnce( Map& mapToMutate )
{
	Tile* randomTile = &mapToMutate.GetRandomMapTile();
	randomTile->SetTileType(m_changeToTileType);

	for( int currentWormLength = 1; currentWormLength < m_wormLength; currentWormLength++ )
	{
		randomTile = &mapToMutate.GetRandomAdjacentMapTile(*randomTile);
		randomTile->SetTileType(m_changeToTileType);
	}

}

