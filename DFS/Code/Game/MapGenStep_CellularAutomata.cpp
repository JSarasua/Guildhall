#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Tile.hpp"
#include "Game/MapGenStep_CellularAutomata.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


MapGenStep_CellularAutomata::MapGenStep_CellularAutomata( const XmlElement& element ) : MapGenStep(element)
{
	std::string ifNeighborTileType = ParseXMLAttribute( element, "ifNeighborTileType", "NONE" );
	m_numOfNeighbors = ParseXMLAttribute(element, "numOfNeighbors", IntRange(1,2));
/*	m_chanceToMutate = ParseXMLAttribute( element, "chanceToMutate", 0.3f );*/

	if( ifNeighborTileType != "NONE" )
	{
		m_ifNeighborTileType = TileDefinition::s_definitions[ifNeighborTileType.c_str()];
	}
}

void MapGenStep_CellularAutomata::RunStepOnce( Map& mapToMutate )
{
	std::vector<Tile>& tiles = mapToMutate.m_tiles;

	for( int tileIndex = 0; tileIndex < tiles.size(); tileIndex++ )
	{
		Tile& currentTile = tiles[tileIndex];
		TileDefinition* currentTileDef = currentTile.m_tileDef;
		if( currentTileDef != m_ifTileType )
		{
			continue;
		}

		if( MetNeighborTileRequirement( currentTile, mapToMutate ) )
		{
			bool doesMutate = g_theGame->m_rand.RollPercentChance( m_chanceToMutate );
			if( doesMutate )
			{
				currentTile.SetTileType( m_changeToTileType );
			}
		}

	}
}

bool MapGenStep_CellularAutomata::MetNeighborTileRequirement( Tile& currentTile, Map& mapToMutate )
{
	if( !m_ifNeighborTileType )
	{
		return true;
	}

	std::vector<Tile*> neighboringTiles = mapToMutate.GetNeighboringTiles(&currentTile);
	int countOfNeighboringTilesOfCorrectType = 0;
	for( int tileIndex = 0; tileIndex < neighboringTiles.size(); tileIndex++ )
	{
		TileDefinition* currentTileDef = neighboringTiles[tileIndex]->m_tileDef;
		if( currentTileDef == m_ifNeighborTileType )
		{
			countOfNeighboringTilesOfCorrectType++;
		}
	}

	if( m_numOfNeighbors.IsInRange( countOfNeighboringTilesOfCorrectType ) )
	{
		return true;
	}

	return false;
}

