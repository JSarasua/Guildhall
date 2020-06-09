#include "Game/MapGenStep_FromImage.hpp"
#include "Game/Game.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/TileMetaData.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


MapGenStep_FromImage::MapGenStep_FromImage( const XmlElement& element ) : MapGenStep(element)
{
	m_fromImageFilePath = ParseXMLAttribute(element, "fromImageFilePath", "INVALID");
	m_rotations			= ParseXMLAttribute(element, "rotations", IntRange(0,3));
	m_chanceToMirror	= ParseXMLAttribute(element, "chanceToMirror", FloatRange(0.f,1.f));
	m_uAlignment		= ParseXMLAttribute(element, "uAlignment", FloatRange(0.f,1.f));		//Think about changing to x
	m_vAlignment		= ParseXMLAttribute(element, "vAlignment", FloatRange(0.f,1.f));		//Think about changing to y

	GUARANTEE_OR_DIE(m_fromImageFilePath != "INVALID", "MapGenStep_FromImage requires a 'fromImageFilePath'");

	m_imageToMap = new Image(m_fromImageFilePath.c_str());

// 	m_wormLength = ParseXMLAttribute( element, "wormLength", 5 );
// 	std::string changeToTypeName = ParseXMLAttribute( element, "changeToType", "INVALID" );
// 
// 	GUARANTEE_OR_DIE( changeToTypeName != "INVALID", "MapGenStep_Worm requires a 'changeToType' parameter" );
// 
// 	m_changeToTileType	= TileDefinition::s_definitions[changeToTypeName.c_str()];
}

void MapGenStep_FromImage::RunStepOnce( Map& mapToMutate )
{
	std::vector<IntVec2> ifTagTilePositions;
	if( m_ifTags.m_tags.size() != 0 )
	{
		ifTagTilePositions = GetTagTilePositions(mapToMutate);
		DrawImageAtTagPositions(ifTagTilePositions, mapToMutate);
	}

	RandomNumberGenerator& randToUse = g_theGame->m_rand;
	Vec2 uvs = Vec2( m_uAlignment.GetRandomInRange( randToUse ), m_vAlignment.GetRandomInRange( randToUse ) );

	DrawImageAtUVPosition(mapToMutate, uvs);
}

IntVec2 MapGenStep_FromImage::GetStartTilePosition( const IntVec2& mapSize, const Vec2& UVs )
{
	const IntVec2& imageMapBounds = m_imageToMap->GetDimensions();
	IntVec2 innerMapBounds = mapSize - imageMapBounds;

	float minimumImageX = RangeMap( 0.f, 1.f, 0.f, (float)innerMapBounds.x, UVs.x );
	float minimumImageY = RangeMap( 0.f, 1.f, 0.f, (float)innerMapBounds.y, UVs.y );

	IntVec2 startTilePosition = IntVec2((int)minimumImageX, (int)minimumImageY);
	return startTilePosition;
}

TileDefinition* MapGenStep_FromImage::GetTileWithMatchingGlyphColorIfValid( const IntVec2& texelCoords)
{
	const Rgba8 colorValues = m_imageToMap->GetTexelColor(texelCoords);
	if( colorValues.a == 0 )
	{
		return nullptr;
	}

	TileDefinition* tileDef = TileDefinition::GetTileDefFromColor(colorValues);
	return tileDef;
}

std::vector<IntVec2> MapGenStep_FromImage::GetTagTilePositions( Map& mapToMutate )
{
	const IntVec2& mapBounds = mapToMutate.GetMapBounds();
	std::vector<IntVec2> tileCoordsWithMatchingTags;
	for( int yCoord = 0; yCoord < mapBounds.x; yCoord++ )
	{
		for( int xCoord = 0; xCoord < mapBounds.x; xCoord++ )
		{
			IntVec2 currentCoords = IntVec2(xCoord,yCoord);
			Tags& tileMetaDataAtCurrentTile = mapToMutate.GetTileMetaDataAtTilePosition(currentCoords).m_tags;
			if( tileMetaDataAtCurrentTile.HasTags( m_ifTags ) )
			{
				tileCoordsWithMatchingTags.push_back(currentCoords);
			}
		}
	}

	return tileCoordsWithMatchingTags;
}

void MapGenStep_FromImage::DrawImageAtTagPositions( std::vector<IntVec2>& ifTagTilePositions, Map& mapToMutate )
{
	for( int tilePositionIndex = 0; tilePositionIndex < ifTagTilePositions.size(); tilePositionIndex++ )
	{
		IntVec2& positionToDraw = ifTagTilePositions[tilePositionIndex];
		DrawImageAtPosition( positionToDraw, mapToMutate );
	}
}

void MapGenStep_FromImage::DrawImageAtUVPosition( Map& mapToMutate, Vec2& UVs )
{
	const IntVec2& mapSize = mapToMutate.GetMapBounds();
	const IntVec2& imageMapBounds = m_imageToMap->GetDimensions();
	IntVec2 startTilePosition = GetStartTilePosition( mapSize, UVs );

	TileDefinition* tileDefToChange = nullptr;

	for( int imageYIndex = 0; imageYIndex < imageMapBounds.y; imageYIndex++ )
	{
		for( int imageXIndex = 0; imageXIndex < imageMapBounds.x; imageXIndex++ )
		{
			tileDefToChange = GetTileWithMatchingGlyphColorIfValid( IntVec2( imageXIndex, imageYIndex ) );
			if( tileDefToChange )
			{
				IntVec2 tilePosition = startTilePosition + IntVec2( imageXIndex, imageYIndex );
				mapToMutate.SetTileAtPosition( tilePosition, tileDefToChange );
			}
		}
	}
}

void MapGenStep_FromImage::DrawImageAtPosition( const IntVec2& positionToDraw, Map& mapToMutate )
{
	IntVec2 mapBounds = mapToMutate.GetMapBounds();
	AABB2 mapBox = AABB2(Vec2(0.f,0.f),Vec2(mapBounds));
	Vec2 positionUVs = mapBox.GetUVForPoint(Vec2(positionToDraw));
	DrawImageAtUVPosition(mapToMutate, positionUVs);
}

