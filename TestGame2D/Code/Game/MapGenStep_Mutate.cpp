#include "Game/MapGenStep_Mutate.hpp"
#include "Game/TileMetaData.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

MapGenStep_Mutate::MapGenStep_Mutate( const XMLElement& element ) : MapGenStep(element)
{}

void MapGenStep_Mutate::RunStepOnce( Map& mapToMutate )
{
	std::vector<Tile>& tiles = mapToMutate.m_tiles;

	for( int tileIndex = 0; tileIndex < tiles.size(); tileIndex++ )
	{
		IntVec2 currentTilePosition = mapToMutate.GetTileCoordinatesForTileIndex(tileIndex);
		Tile* currentTile = mapToMutate.GetTileAtPosition(currentTilePosition);
		Tags& currentTags = mapToMutate.GetTileMetaDataAtTilePosition(currentTilePosition).m_tags;
		TileDefinition* currentTileDef = currentTile->m_tileDef;

		if( DoTagsAndTilesMatch( currentTileDef, currentTags ) )
		{
			if( m_changeToTileType )
			{
				bool doesMutate = g_theGame->m_rand.RollPercentChance( m_chanceToMutate );
				if( doesMutate )
				{
					currentTile->SetTileType( m_changeToTileType );
				}
			}

			if( m_setTags.m_tags.size() != 0 )
			{
				bool doesSetTag = g_theGame->m_rand.RollPercentChance( m_chanceToMutate );
				if( doesSetTag )
				{
					mapToMutate.AddTagsAtPosition( currentTilePosition, m_setTags );
				}
			}
		}
	}
}

bool MapGenStep_Mutate::DoTagsAndTilesMatch( TileDefinition* currentTileDef, Tags& currentTags )
{
	if( !m_ifTileType || m_ifTileType == currentTileDef )
	{
		if( m_ifTags.m_tags.size() == 0 || m_ifTags.HasTags( currentTags ) )
		{
			return true;
		}
	}
	return false;
}

