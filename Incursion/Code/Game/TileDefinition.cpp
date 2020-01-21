#include "Game/TileDefinition.hpp"
#include "Game/Tile.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"

std::vector<TileDefinition> TileDefinition::s_definitions;

TileDefinition::TileDefinition()
{}

void TileDefinition::InitializeTileDefinitions(const Texture& texture)
{
	SpriteSheet* testSpriteSheet = new SpriteSheet( texture, IntVec2( 8, 8 ) );

	const SpriteDefinition& testSpriteDefStone = testSpriteSheet->GetSpriteDefinition( 33 );
	const SpriteDefinition& testSpriteDefLava = testSpriteSheet->GetSpriteDefinition( 62 );
	const SpriteDefinition& testSpriteDefIce = testSpriteSheet->GetSpriteDefinition( 52 );
	const SpriteDefinition& testSpriteDefGrass = testSpriteSheet->GetSpriteDefinition( 3 );
	const SpriteDefinition& testSpriteDefGravel = testSpriteSheet->GetSpriteDefinition( 47 );
	const SpriteDefinition& testSpriteDefBrick = testSpriteSheet->GetSpriteDefinition( 40 );
	const SpriteDefinition& testSpriteDefSand = testSpriteSheet->GetSpriteDefinition( 15 );
	const SpriteDefinition& testSpriteDefVolcanicRock = testSpriteSheet->GetSpriteDefinition( 25 );
	const SpriteDefinition& testSpriteDefMud = testSpriteSheet->GetSpriteDefinition( 29 );
	const SpriteDefinition& testSpriteDefExit = testSpriteSheet->GetSpriteDefinition( 57 );



	for( int tileTypeIndex = 0; tileTypeIndex < NUM_TILETYPES; tileTypeIndex++ )
	{			
		s_definitions.push_back(TileDefinition());

		if( tileTypeIndex == STONE )
		{
			testSpriteDefStone.GetUVs( s_definitions[tileTypeIndex].m_uvMins, s_definitions[tileTypeIndex].m_uvMaxs );
			s_definitions[tileTypeIndex].m_isSolid = true;
		}
		else if( tileTypeIndex == LAVA )
		{
			testSpriteDefLava.GetUVs( s_definitions[tileTypeIndex].m_uvMins, s_definitions[tileTypeIndex].m_uvMaxs );
			s_definitions[tileTypeIndex].m_isSolid = true;
		}
		else if( tileTypeIndex == ICE )
		{
			testSpriteDefIce.GetUVs( s_definitions[tileTypeIndex].m_uvMins, s_definitions[tileTypeIndex].m_uvMaxs );
			s_definitions[tileTypeIndex].m_isSolid = true;
		}
		else if( tileTypeIndex == GRASS )
		{

			testSpriteDefGrass.GetUVs( s_definitions[tileTypeIndex].m_uvMins, s_definitions[tileTypeIndex].m_uvMaxs );
			s_definitions[tileTypeIndex].m_isSolid = false;
		}
		else if( tileTypeIndex == GRAVEL )
		{

			testSpriteDefGravel.GetUVs( s_definitions[tileTypeIndex].m_uvMins, s_definitions[tileTypeIndex].m_uvMaxs );
			s_definitions[tileTypeIndex].m_isSolid = false;
		}
		else if( tileTypeIndex == BRICK )
		{

			testSpriteDefBrick.GetUVs( s_definitions[tileTypeIndex].m_uvMins, s_definitions[tileTypeIndex].m_uvMaxs );
			s_definitions[tileTypeIndex].m_isSolid = true;
		}
		else if( tileTypeIndex == SAND )
		{

			testSpriteDefSand.GetUVs( s_definitions[tileTypeIndex].m_uvMins, s_definitions[tileTypeIndex].m_uvMaxs );
			s_definitions[tileTypeIndex].m_isSolid = false;
		}
		else if( tileTypeIndex == VOLCANICROCK )
		{

			testSpriteDefVolcanicRock.GetUVs( s_definitions[tileTypeIndex].m_uvMins, s_definitions[tileTypeIndex].m_uvMaxs );
			s_definitions[tileTypeIndex].m_isSolid = true;
		}
		else if( tileTypeIndex == MUD )
		{

			testSpriteDefMud.GetUVs( s_definitions[tileTypeIndex].m_uvMins, s_definitions[tileTypeIndex].m_uvMaxs );
			s_definitions[tileTypeIndex].m_isSolid = false;
		}
		else if( tileTypeIndex == EXIT )
		{
			testSpriteDefExit.GetUVs( s_definitions[tileTypeIndex].m_uvMins, s_definitions[tileTypeIndex].m_uvMaxs );
			s_definitions[tileTypeIndex].m_isSolid = false;
		}
	}
}

