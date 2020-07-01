#include "Game/TileMap.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/MapRegionType.hpp"
#include "Game/GameCommon.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/Actor.hpp"
#include "Game/Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Renderer/DebugRender.hpp"

extern RenderContext* g_theRenderer;

TileMap::TileMap( XmlElement const& element )
{
	std::string mapName = element.Name();
	g_theConsole->GuaranteeOrError( mapName == "MapDefinition", Stringf( "ERROR: Expected MapDefinition as root node" ) );


	m_mapSize = ParseXMLAttribute( element, "dimensions", IntVec2( 0, 0 ) );
	m_isValid = g_theConsole->GuaranteeOrError( m_mapSize != IntVec2( 0, 0 ), Stringf( "ERROR: Missing Map dimensions" ) );
	if( !m_isValid )
	{
		return;
	}

	XmlElement const* legendElement = element.FirstChildElement( "Legend" );
	XmlElement const* mapRowsElement = element.FirstChildElement( "MapRows" );
	XmlElement const* entitiesElement = element.FirstChildElement( "Entities" );

	m_isValid = g_theConsole->GuaranteeOrError( legendElement, Stringf( "ERROR: No Legend for map" ) );
	if( !m_isValid )
	{
		return;
	}

	m_isValid = g_theConsole->GuaranteeOrError( mapRowsElement, Stringf( "ERROR: No MapRows found for map" ) );
	if( !m_isValid )
	{
		return;
	}
	
	if( !ParseLegend( *legendElement ) )
	{
		m_isValid = false;
		return;
	}

	if( !SpawnTiles( *mapRowsElement ) )
	{
		m_isValid = false;
		return;
	}

	if( !m_isValid )
	{
		return;
	}

	bool isEntitiesElementValid = false;
	isEntitiesElementValid = g_theConsole->GuaranteeOrError( entitiesElement, Stringf( "ERROR: No Entities found for map" ) );
	if( isEntitiesElementValid )
	{
		ParseEntities( *entitiesElement );
	}

	m_isValid = true;
}

TileMap::~TileMap()
{

}

void TileMap::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	m_vertsToRender.clear();
	m_tileIndices.clear();
	Vec3 tileOffset = Vec3( 0.5f, 0.5f, 0.5f );


	size_t tileCount = m_tiles.size();
	for( size_t tileIndex = 0; tileIndex < tileCount; tileIndex++ )
	{
		MapTile const& tile = m_tiles[tileIndex];
		AppendIndexedVertsTestCube( m_vertsToRender, m_tileIndices, tile );
	}

	Map::Update( deltaSeconds );
	ResolveAllEntityWallCollisions();

	Entity const* possessedEntity = g_theGame->GetPossessedEntity();
	if( possessedEntity )
	{
		Vec3 forwardVector = possessedEntity->GetForwardVector();
		Vec3 entityPosition = possessedEntity->GetEyeHeightPosition();
		float maxDistance = 10.f;
		m_currentRaycastResult = Raycast( entityPosition, forwardVector, maxDistance, possessedEntity );
	}
}

void TileMap::Render()
{
	//Should always be the same texture for ceiling, side, and floor
	if( m_tiles[0].IsSolid() )
	{
		Texture const& texture = m_tiles[0].GetTexture( eMapMaterialArea::SIDE );
		g_theRenderer->BindTexture( &texture );
		g_theRenderer->SetModelMatrix( Mat44() );
		g_theRenderer->DrawIndexedVertexArray( m_vertsToRender, m_tileIndices );
	}
	else
	{
		Texture const& texture = m_tiles[0].GetTexture( eMapMaterialArea::CEILING );
		g_theRenderer->BindTexture( &texture );
		g_theRenderer->SetModelMatrix( Mat44() );
		g_theRenderer->DrawIndexedVertexArray( m_vertsToRender, m_tileIndices );
	}

	Map::Render();


	//DebugRender Raycast

		if( m_currentRaycastResult.didImpact )
		{
			Vec3 startPosition = m_currentRaycastResult.startPosition;
			Vec3 impactPosition = m_currentRaycastResult.impactPosition;
			Vec3 impactSurfaceNormal = m_currentRaycastResult.impactSurfaceNormal;
			Vec3 surfaceNormalToDraw = impactSurfaceNormal * 0.5f;

			LineSegment3 raycastLine = LineSegment3( startPosition, impactPosition );
			LineSegment3 impactSurfaceNormalLine = LineSegment3( impactPosition, impactPosition + surfaceNormalToDraw );
		
			DebugAddWorldLine( raycastLine, Rgba8::BLUE, 0.f );
			DebugAddWorldArrow( impactSurfaceNormalLine, Rgba8::GREEN, 0.f );
		}

}

void TileMap::SetPlayerToStart()
{
	g_theGame->SetPlayerPosition( Vec3(m_playerStartPosition, 0.5f) );
	Vec3 pitchRollYawDegrees = Vec3( 0.f, 0.f, m_playerStartYaw );
	g_theGame->SetPlayerRotation( pitchRollYawDegrees );
}

Vec2 const& TileMap::GetPlayerStartPosition()
{
	return m_playerStartPosition;
}

float TileMap::GetPlayerStartYaw()
{
	return m_playerStartYaw;
}

RaycastResult TileMap::Raycast( Vec3 const& startPosition, Vec3 const& forwardNormal, float maxDistance, Entity const* entityToIgnore )
{
	//return RaycastFast( startPosition, forwardNormal, maxDistance, entityToIgnore );
	return RaycastStepAndSample( startPosition, forwardNormal, maxDistance, entityToIgnore );
}

RaycastResult TileMap::RaycastStepAndSample( Vec3 const& startPosition, Vec3 const& forwardNormal, float maxDistance, Entity const* entityToIgnore )
{
	RaycastResult result;
	result.startPosition = startPosition;
	result.forwardNormalOfRaycast = forwardNormal;
	result.maxDistance = maxDistance;
	result.entityToIgnore = entityToIgnore;

	float increment = 0.001f;
	float currentDistance = 0.f;

	while( currentDistance < maxDistance )
	{

		Vec3 currentPos = startPosition + (forwardNormal*currentDistance);
		if( currentPos.z >= 1.f )
		{
			result.didImpact = true;
			result.impactPosition = currentPos;
			result.impactFraction = currentDistance / maxDistance;
			result.impactDistance = currentDistance;
			result.impactSurfaceNormal = Vec3( 0.f, 0.f, -1.f );

			return result;
		}
		else if( currentPos.z <= 0.f )
		{
			result.didImpact = true;
			result.impactPosition = currentPos;
			result.impactFraction = currentDistance / maxDistance;
			result.impactDistance = currentDistance;
			result.impactSurfaceNormal = Vec3( 0.f, 0.f, 1.f );

			return result;
		}

		Vec3 surfaceNormal;
		Entity* impactEntity = GetEntityAtImpactPosition( currentPos, surfaceNormal, entityToIgnore );
		if( impactEntity )
		{
			result.didImpact = true;
			result.impactPosition = currentPos;
			result.impactFraction = currentDistance / maxDistance;
			result.impactDistance = currentDistance;
			result.impactSurfaceNormal = surfaceNormal;
			result.impactEntity = impactEntity;

			return result;
		}
		
		IntVec2 currentPos2DIntVec = IntVec2( currentPos );
		if( IsTileSolidAtCoords( currentPos2DIntVec ) )
		{
			result.didImpact = true;
			result.impactPosition = currentPos;
			result.impactFraction = currentDistance / maxDistance;
			result.impactDistance = currentDistance;
			//result.impactSurfaceNormal = Vec3( 0.f, 0.f, 1.f );

			Vec2 currentPos2DVecFloored = Vec2( currentPos2DIntVec );
			Vec2 currentPos2DVec = Vec2( currentPos );

			Vec2 tilePenetration = currentPos2DVec - currentPos2DVecFloored;
			
			float distToLeftTile = tilePenetration.x;
			float distToRightTile = 1.f - tilePenetration.x;
			float distToTopTile = 1.f - tilePenetration.y;
			float distToBottomTile = tilePenetration.y;

			Vec3 currentSurfaceNormal = Vec3( -1.f, 0.f, 0.f );
			float currentClosestTileDist = distToLeftTile;
			if( currentClosestTileDist > distToRightTile )
			{
				currentSurfaceNormal = Vec3( 1.f, 0.f, 0.f );
				currentClosestTileDist = distToRightTile;
			}
			if( currentClosestTileDist > distToTopTile )
			{
				currentSurfaceNormal = Vec3( 0.f, 1.f, 0.f );
				currentClosestTileDist = distToTopTile;
			}
			if( currentClosestTileDist > distToBottomTile )
			{
				currentSurfaceNormal = Vec3( 0.f, -1.f, 0.f );
				currentClosestTileDist = distToBottomTile;
			}
			result.impactSurfaceNormal = currentSurfaceNormal;


			return result;
		}

		currentDistance += increment;
	}

	return result;
}

RaycastResult TileMap::RaycastFast( Vec3 const& startPosition, Vec3 const& forwardNormal, float maxDistance, Entity const* entityToIgnore )
{
	RaycastResult result;
	result.startPosition = startPosition;
	result.forwardNormalOfRaycast = forwardNormal;
	result.maxDistance = maxDistance;

	result.impactDistance = maxDistance;

	float currentMaxDistance = maxDistance;
	Entity* impactEntity = nullptr;
	UNUSED( impactEntity );
	UNUSED( entityToIgnore );
	//Ceiling Check
	float startZ = startPosition.z;
	float normalZ = forwardNormal.z;
	if( normalZ < 0.f )
	{
		//0 = startZ + normalZ * dist
		//-startZ = normalZ * dist
		//-startZ/normalZ = dist;
		float distanceToCheck = -startZ/normalZ;
		if( distanceToCheck < currentMaxDistance )
		{
			currentMaxDistance = distanceToCheck;

			result.impactPosition = startPosition + forwardNormal * currentMaxDistance;
			result.impactDistance = currentMaxDistance;
			result.impactFraction = currentMaxDistance / maxDistance;
			result.didImpact = true;
			result.impactSurfaceNormal = Vec3( 0.f, 0.f, 1.f );
		}
	}
	else if( normalZ > 0.f )
	{
		// (1 - startZ)/normalZ = dist
		float distanceToCheck = (1.f-startZ) / normalZ;
		if( distanceToCheck < currentMaxDistance )
		{
			currentMaxDistance = distanceToCheck;

			result.impactPosition = startPosition + forwardNormal * currentMaxDistance;
			result.impactDistance = currentMaxDistance;
			result.impactFraction = currentMaxDistance / maxDistance;
			result.didImpact = true;
			result.impactSurfaceNormal = Vec3( 0.f, 0.f, -1.f );
		}
	}

	bool canCheckX = !AlmostEqualsFloat( forwardNormal.x, 0.f, 0.001f );
	bool canCheckY = !AlmostEqualsFloat( forwardNormal.y, 0.f, 0.001f );

	Vec3 rayDisplacement = forwardNormal * maxDistance;

	float distOfNextXCrossing = 999999999.f;
	int firstVerticalIntersectionX = 0;
	int offsetToLeadingEdgeX = 0;
	int tileStepX = 0;
	int startTileX = 0;
	float distPerXCrossing = 0.f;

	float distOfNextYCrossing = 999999999.f;
	int firstHorizontalIntersectionY = 0;
	int offsetToLeadingEdgeY = 0;
	int tileStepY = 0;
	int startTileY = 0;
	float distPerYCrossing = 0.f;
	//Wall check
	if( canCheckX )
	{
		startTileX = (int)startPosition.x;
		float maxXDisplacement = forwardNormal.x * currentMaxDistance;
		distPerXCrossing =  currentMaxDistance / absFloat( maxXDisplacement );

		if( forwardNormal.x > 0.f )
		{
			tileStepX = 1;
		}
		else if( forwardNormal.x < 0.f )
		{
			tileStepX = -1;
		}

		offsetToLeadingEdgeX = (tileStepX + 1) / 2;
		firstVerticalIntersectionX = ( startTileX + offsetToLeadingEdgeX );
		distOfNextXCrossing = absFloat( (float)(firstVerticalIntersectionX - startPosition.x) ) * distPerXCrossing;
	}

	if( canCheckY )
	{
		startTileY = (int)startPosition.y;
		float maxYDisplacement = forwardNormal.y * currentMaxDistance;
		distPerYCrossing =  currentMaxDistance / absFloat( maxYDisplacement );

		if( forwardNormal.y > 0.f )
		{
			tileStepY = 1;
		}
		else if( forwardNormal.y < 0.f )
		{
			tileStepY = -1;
		}

		offsetToLeadingEdgeY = (tileStepY + 1) / 2;
		firstHorizontalIntersectionY = (startTileY + offsetToLeadingEdgeY);
		distOfNextYCrossing = absFloat( (float)(firstHorizontalIntersectionY - startPosition.y) ) * distPerYCrossing;

	}

	int tileX = firstVerticalIntersectionX;
	int tileY = firstHorizontalIntersectionY;


	while( true )
	{
		if( distOfNextXCrossing < distOfNextYCrossing )
		{
			if( distOfNextXCrossing > currentMaxDistance )
			{
				break;
			}

			tileX += tileStepX;
			distOfNextXCrossing = absFloat( (float)(tileX - startTileX) ) * distPerXCrossing;

			if( IsTileSolidAtCoords( IntVec2( tileX, tileY ) ) )
			{
				result.didImpact = true;
				result.impactPosition = startPosition + forwardNormal * distOfNextXCrossing;
				result.impactDistance = distOfNextXCrossing;
				result.impactFraction = distOfNextXCrossing / currentMaxDistance;

				if( tileStepX == 1 )
				{
					result.impactSurfaceNormal = Vec3( -1.f, 0.f, 0.f );
				}
				else
				{
					result.impactSurfaceNormal = Vec3( 1.f, 0.f, 0.f );
				}

				currentMaxDistance = distOfNextXCrossing;
				break;
			}
			else
			{

				continue;
			}
		}
		else
		{
			if( distOfNextYCrossing > currentMaxDistance )
			{
				break;
			}

			tileY += tileStepY;
			distOfNextYCrossing = absFloat( (float)(tileY - startTileY) ) * distPerYCrossing;

			if( IsTileSolidAtCoords( IntVec2( tileX, tileY ) ) )
			{
				result.didImpact = true;
				result.impactPosition = startPosition + forwardNormal * distOfNextYCrossing;
				result.impactDistance = distOfNextYCrossing;
				result.impactFraction = distOfNextYCrossing / currentMaxDistance;

				if( tileStepY == 1 )
				{
					result.impactSurfaceNormal = Vec3( 0.f, -1.f, 0.f );
				}
				else
				{
					result.impactSurfaceNormal = Vec3( 0.f, 1.f, 0.f );
				}

				currentMaxDistance = distOfNextYCrossing;
				break;
			}
			else
			{

				continue;
			}
		}
	}

	//EntityCheck
	return result;
}

void TileMap::AppendIndexedVertsTestCube( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList, MapTile const& tile   )
{
	Vec3 tileOffset = Vec3( 0.5f, 0.5f, 0.5f );
	Vec3 centerPosition = Vec3( Vec2( tile.m_tileCoords ), 0.f );
	centerPosition += tileOffset;

	int currentVertexListSize = (int)masterVertexList.size();
	Vec3 nonUniformScale = Vec3( 1.f, 1.f, 1.f );
	float cubeHalfHeight = 0.5f;
	float c = cubeHalfHeight;

	Vec3 forward	= Vec3( 0.f, 0.f, 1.f );
	Vec3 right	= Vec3( 1.f, 0.f, 0.f );
	Vec3 back		= Vec3( 0.f, 0.f, -1.f );
	Vec3 left		= Vec3( -1.f, 0.f, 0.f );
	Vec3 up		= Vec3( 0.f, 1.f, 0.f );
	Vec3 down		= Vec3( 0.f, -1.f, 0.f );

	Vertex_PCUTBN cubeVerts[24];
	if( tile.IsSolid() )
	{
		Vec2 sideBLeft;
		Vec2 sideTRight;

		tile.GetUVs( sideBLeft, sideTRight, eMapMaterialArea::SIDE );

		Vec2 sideTLeft( sideBLeft.x, sideTRight.y );
		Vec2 sideBRight( sideTRight.x, sideBLeft.y );

		//Top quad
		cubeVerts[0] =	Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		sideTRight, forward, right, up );		//3
		cubeVerts[1] =	Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		sideTLeft, forward, right, up );		//2
		cubeVerts[2] =	Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		sideBLeft, forward, right, up );		//1
		cubeVerts[3] =	Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		sideBRight,  forward, right, up );	//0

		//Bottom quad
		cubeVerts[4] =	Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		sideTRight, back, left, up );		//11
		cubeVerts[5] =	Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,		sideTLeft, back, left, up );		//10
		cubeVerts[6] =	Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	sideBLeft, back, left, up );		//9
		cubeVerts[7] =	Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		sideBRight,	back, left, up );		//8
	
		//Back quad
		cubeVerts[8] =	Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		sideTLeft,	right, back, up );		//4
		cubeVerts[9] =	Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		sideBLeft, right, back, up );		//5
		cubeVerts[10] =	Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		sideBRight, right, back, up );		//6
		cubeVerts[11] =	Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		sideTRight, right, back, up );		//7
		
		//Front quad
		cubeVerts[12] =	Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	sideBRight,	left, forward, up );		//12
		cubeVerts[13] =	Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		sideTRight, left, forward, up );		//13
		cubeVerts[14] =	Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		sideTLeft, left, forward, up );			//14
		cubeVerts[15] =	Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,		sideBLeft, left, forward, up );		//15
			
		//Left quad
		cubeVerts[16] =	Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		sideTRight,	up, right, back );		//16
		cubeVerts[17] =	Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		sideTLeft, up, right, back );		//17
		cubeVerts[18] =	Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		sideBLeft, up, right, back );		//18
		cubeVerts[19] =	Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,			sideBRight, up, right, back );		//19
			
		//Right quad
		cubeVerts[20] =	Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	sideBLeft,	down, right, forward );		//20
		cubeVerts[21] =	Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		sideBRight, down, right, forward );		//21
		cubeVerts[22] =	Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		sideTRight, down, right, forward );		//22
		cubeVerts[23] =	Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		sideTLeft, down, right, forward );			//23


		for( int vertexIndex = 0; vertexIndex < 24; vertexIndex++ )
		{
			cubeVerts[vertexIndex].position += centerPosition;
		}

		for( int vertexIndex = 0; vertexIndex < 24; vertexIndex++ )
		{
			masterVertexList.push_back( cubeVerts[vertexIndex] );
		}

		std::vector<uint> cubeIndices;
		AddTileIndices( cubeIndices, tile );


		for( size_t indicesIndex = 0; indicesIndex < cubeIndices.size(); indicesIndex++ )
		{
			masterIndexList.push_back( cubeIndices[indicesIndex] + currentVertexListSize );
		}
	}
	else
	{
		Vec2 floorBLeft;
		Vec2 floorTRight;
		tile.GetUVs( floorBLeft, floorTRight, eMapMaterialArea::FLOOR );
		Vec2 floorTLeft( floorBLeft.x, floorTRight.y );
		Vec2 floorBRight( floorTRight.x, floorBLeft.y );

		Vec2 ceilingBLeft;
		Vec2 ceilingTRight;
		tile.GetUVs( ceilingBLeft, ceilingTRight, eMapMaterialArea::CEILING );
		Vec2 ceilingTLeft( ceilingBLeft.x, ceilingTRight.y );
		Vec2 ceilingBRight( ceilingTRight.x, ceilingBLeft.y );

		//Top quad
		cubeVerts[0] =	Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE, ceilingTRight, forward, right, up );		//3
		cubeVerts[1] =	Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE, ceilingTLeft, forward, right, up );		//2
		cubeVerts[2] =	Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE, ceilingBLeft, forward, right, up );		//1
		cubeVerts[3] =	Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE, ceilingBRight, forward, right, up );	//0

																																//Bottom quad
		cubeVerts[4] =	Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE, floorTRight, back, left, up );		//11
		cubeVerts[5] =	Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE, floorTLeft, back, left, up );		//10
		cubeVerts[6] =	Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE, floorBLeft, back, left, up );		//9
		cubeVerts[7] =	Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE, floorBRight, back, left, up );		//8

																																//Back quad
		cubeVerts[8] =	Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE, ceilingTLeft, right, back, up );		//4
		cubeVerts[9] =	Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE, ceilingBLeft, right, back, up );		//5
		cubeVerts[10] =	Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE, ceilingBRight, right, back, up );		//6
		cubeVerts[11] =	Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE, ceilingTRight, right, back, up );		//7

																															//Front quad
		cubeVerts[12] =	Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE, ceilingBRight, left, forward, up );		//12
		cubeVerts[13] =	Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE, ceilingTRight, left, forward, up );		//13
		cubeVerts[14] =	Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE, ceilingTLeft, left, forward, up );			//14
		cubeVerts[15] =	Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE, ceilingBLeft, left, forward, up );		//15

																																//Left quad
		cubeVerts[16] =	Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE, ceilingTRight, up, right, back );		//16
		cubeVerts[17] =	Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE, ceilingTLeft, up, right, back );		//17
		cubeVerts[18] =	Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE, ceilingBLeft, up, right, back );		//18
		cubeVerts[19] =	Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE, ceilingBRight, up, right, back );		//19

																																	//Right quad
		cubeVerts[20] =	Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE, ceilingBLeft, down, right, forward );		//20
		cubeVerts[21] =	Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE, ceilingBRight, down, right, forward );		//21
		cubeVerts[22] =	Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE, ceilingTRight, down, right, forward );		//22
		cubeVerts[23] =	Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE, ceilingTLeft, down, right, forward );			//23


		for( int vertexIndex = 0; vertexIndex < 24; vertexIndex++ )
		{
			cubeVerts[vertexIndex].position += centerPosition;
		}

		for( int vertexIndex = 0; vertexIndex < 24; vertexIndex++ )
		{
			masterVertexList.push_back( cubeVerts[vertexIndex] );
		}

		std::vector<uint> cubeIndices;
		AddTileIndices( cubeIndices, tile );


		for( size_t indicesIndex = 0; indicesIndex < cubeIndices.size(); indicesIndex++ )
		{
			masterIndexList.push_back( cubeIndices[indicesIndex] + currentVertexListSize );
		}
	}






// 	Vec2 bLeft;
// 	Vec2 tRight;
// 	tile.GetUVs( bLeft, tRight, eMapMaterialArea::CEILING );
// 
// 	Vec2 tLeft( bLeft.x, tRight.y );
// 	Vec2 bRight( tRight.x, bLeft.y );
// 
// 
// 
// 	Vertex_PCUTBN cubeVerts[24] = 
// 	{
// 		//Top quad
// 		Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, forward, right, up ),		//3
// 		Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, forward, right, up ),		//2
// 		Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		bLeft, forward, right, up ),		//1
// 		Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		bRight,  forward, right, up ),	//0
// 		
// 		//Bottom quad
// 		Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		tRight, back, left, up ),		//11
// 		Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,		tLeft, back, left, up ),		//10
// 		Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	bLeft, back, left, up ),		//9
// 		Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		bRight,	back, left, up ),		//8
// 
// 		//Back quad
// 		Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft,	right, back, up ),		//4
// 		Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		bLeft, right, back, up ),		//5
// 		Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bRight, right, back, up ),		//6
// 		Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, right, back, up ),		//7
// 		
// 
// 		//Front quad
// 		Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	bRight,	left, forward, up ),		//12
// 		Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, left, forward, up ),		//13
// 		Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, left, forward, up ),			//14
// 		Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bLeft, left, forward, up ),		//15
// 
// 		//Left quad
// 		Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		tRight,	up, right, back ),		//16
// 		Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, up, right, back ),		//17
// 		Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bLeft, up, right, back ),		//18
// 		Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,			bRight, up, right, back ),		//19
// 
// 		//Right quad
// 		Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	bLeft,	down, right, forward ),		//20
// 		Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		bRight, down, right, forward ),		//21
// 		Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, down, right, forward ),		//22
// 		Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, down, right, forward )			//23
// 	};
// 
// 	for( int vertexIndex = 0; vertexIndex < 24; vertexIndex++ )
// 	{
// 		cubeVerts[vertexIndex].position += centerPosition;
// 	}
// 
// 	for( int vertexIndex = 0; vertexIndex < 24; vertexIndex++ )
// 	{
// 		masterVertexList.push_back( cubeVerts[vertexIndex] );
// 	}
// 
// 	std::vector<uint> cubeIndices;
// 	AddTileIndices( cubeIndices, tile );
// 
// 
// 	for( size_t indicesIndex = 0; indicesIndex < cubeIndices.size(); indicesIndex++ )
// 	{
// 		masterIndexList.push_back( cubeIndices[indicesIndex] + currentVertexListSize );
// 	}
}

void TileMap::AddTileIndices( std::vector<uint>& tileIndices, MapTile const& tile )
{
// 	bool isBackTile = (tile.m_tileCoords.x == m_mapSize.x - 1);
// 	bool isRightTile = (tile.m_tileCoords.y == 0);
// 	bool isFrontTile = (tile.m_tileCoords.x == 0);
// 	bool isLeftTile = (tile.m_tileCoords.y == m_mapSize.y - 1);
	//bool isEdgeTile = isBackTile || isRightTile || isLeftTile || isFrontTile;

	IntVec2 left = IntVec2( 0, 1 );
	IntVec2 right = IntVec2( 0, -1 );
	IntVec2 forward = IntVec2( -1, 0 );
	IntVec2 backward = IntVec2( 1, 0 );

	IntVec2 currentTileCoords = tile.m_tileCoords;

	bool isLeftTileSolid = IsTileSolidAtCoords( currentTileCoords + left );
	bool isRightTileSolid = IsTileSolidAtCoords( currentTileCoords + right );
	bool isBackTileSolid = IsTileSolidAtCoords( currentTileCoords + backward );
	bool isFrontTileSolid = IsTileSolidAtCoords( currentTileCoords + forward );

	if( !tile.IsSolid() )
	{
		//Top quad
		tileIndices.push_back( 0 );
		tileIndices.push_back( 1 );
		tileIndices.push_back( 2 );
		tileIndices.push_back( 0 );
		tileIndices.push_back( 2 );
		tileIndices.push_back( 3 );

		//Bottom quad
		tileIndices.push_back( 4 );
		tileIndices.push_back( 5 );
		tileIndices.push_back( 6 );
		tileIndices.push_back( 4 );
		tileIndices.push_back( 6 );
		tileIndices.push_back( 7 );
	}

	if( tile.IsSolid() )
	{
		//Back quad
		if( !isBackTileSolid )
		{
			tileIndices.push_back( 8 );
			tileIndices.push_back( 9 );
			tileIndices.push_back( 10 );
			tileIndices.push_back( 8 );
			tileIndices.push_back( 10 );
			tileIndices.push_back( 11 );
		}

		//Front quad
		if( !isFrontTileSolid )
		{
			tileIndices.push_back( 12 );
			tileIndices.push_back( 13 );
			tileIndices.push_back( 14 );
			tileIndices.push_back( 12 );
			tileIndices.push_back( 14 );
			tileIndices.push_back( 15 );
		}


		//Left quad
		if( !isLeftTileSolid )
		{
			tileIndices.push_back( 16 );
			tileIndices.push_back( 17 );
			tileIndices.push_back( 18 );
			tileIndices.push_back( 16 );
			tileIndices.push_back( 18 );
			tileIndices.push_back( 19 );
		}


		//Right quad
		if( !isRightTileSolid )
		{
			tileIndices.push_back( 20 );
			tileIndices.push_back( 21 );
			tileIndices.push_back( 22 );
			tileIndices.push_back( 20 );
			tileIndices.push_back( 22 );
			tileIndices.push_back( 23 );
		}
	}
}

bool TileMap::IsTileSolidAtCoords( IntVec2 const& tileCoords )
{
	//if tile doesn't exist its solid
	if( tileCoords.x < 0 || tileCoords.x >= m_mapSize.x )
	{
		return true;
	}
	if( tileCoords.y < 0 || tileCoords.y >= m_mapSize.y )
	{
		return true;
	}

	MapTile const& tile = GetMapTileAtTileCoords( tileCoords );
	return tile.IsSolid();

}

MapTile const& TileMap::GetMapTileAtTileCoords( IntVec2 const& tileCoords )
{
	int tileIndex = tileCoords.x + tileCoords.y * m_mapSize.x;
	return m_tiles[tileIndex];
}

bool TileMap::ParseLegend( XmlElement const& legendElement )
{
	//Passed in element with name Legend
	//Has sub elements with format <Tile glyph="#" regionType="WoodWall"/>

	for( XmlElement const* element = legendElement.FirstChildElement(); element; element=element->NextSiblingElement() ) 
	{
		XmlAttribute const* glyphAttribute = element->FindAttribute( "glyph" );
		XmlAttribute const* regionAttribute = element->FindAttribute( "regionType" );
		std::string elementName = element->Name();

		if( !g_theConsole->GuaranteeOrError( elementName == "Tile", "Legend has elements not named Tile" ) )
		{
			return false;
		}


		if( !g_theConsole->GuaranteeOrError( glyphAttribute, "There was no glyph attribute in Legend" ) )
		{
			return false;
		}

		if( !g_theConsole->GuaranteeOrError( regionAttribute, "There was no regionType attribute in Legend" ) )
		{
			return false;
		}

		unsigned char glyph = ParseXMLAttribute( *element, "glyph", '\0' );
		std::string regionTypeStr = ParseXMLAttribute( *element, "regionType", "INVALID" );

		if( !g_theConsole->GuaranteeOrError( glyph != '\0', "glyph was an invalid character or string") )
		{
			return false;
		}

		if( !g_theConsole->GuaranteeOrError( regionTypeStr != "INVALID", "Invalid region type" ) )
		{
			return false;
		}

		m_legend[glyph] = regionTypeStr;
	}

	return true;
}

bool TileMap::SpawnTiles( XmlElement const& mapRowsElement )
{
	std::vector<std::string> mapRows;
	for( XmlElement const* element = mapRowsElement.FirstChildElement(); element; element=element->NextSiblingElement() )
	{
		std::string elementName = element->Name();
		g_theConsole->GuaranteeOrError( elementName == "MapRow", "ERROR: MapRows has element not named MapRow" );

		std::string mapRow = ParseXMLAttribute( *element, "tiles", "INVALID" );

		if( !g_theConsole->GuaranteeOrError( mapRow != "INVALID", "ERROR: Missing MapRow tiles" ) )
		{
			return false;
		}

		if( !g_theConsole->GuaranteeOrError( mapRow.length() == m_mapSize.x, "Invalid MapRow tile width" ) )
		{
			return false;
		}

		mapRows.push_back( mapRow );
	}

	if( !g_theConsole->GuaranteeOrError( mapRows.size() == m_mapSize.y, "Number of Map rows doesn't match map height dimension" ) )
	{
		return false;
	}

	uint heightIndex = 0;
	for( int mapRowsIndex = (int)mapRows.size() - 1; mapRowsIndex >= 0; mapRowsIndex-- )
	{
		bool isValid = SpawnMapRow( mapRows[mapRowsIndex], heightIndex );
		heightIndex++;

		if( !isValid )
		{
			return false;
		}
	}

	return true;
}

bool TileMap::SpawnMapRow( std::string const& mapRow, uint heightIndex  )
{
	for( size_t mapRowIndex = 0; mapRowIndex < mapRow.size(); mapRowIndex++ )
	{
		char glyph = mapRow[mapRowIndex];
		auto legendIter = m_legend.find(glyph);
		
		if( !g_theConsole->GuaranteeOrError( legendIter != m_legend.end(), Stringf( "Glyph %c not found in legend", glyph ) ) )
		{
			return false;
		}
		
		std::string mapRegionStr = legendIter->second;
		MapRegionType* mapRegionType = MapRegionType::GetMapRegionTypeByString( mapRegionStr );

		if( !mapRegionType )
		{
			return false;
		}

		IntVec2 tileCoords = IntVec2( (int)mapRowIndex, heightIndex );
		m_tiles.push_back( MapTile( tileCoords, mapRegionType ) );
	}

	return true;
}

void TileMap::ParseEntities( XmlElement const& entitiesElement )
{
	XmlElement const* playerStartElement = entitiesElement.FirstChildElement( "PlayerStart" );

	if( !g_theConsole->GuaranteeOrError( playerStartElement, Stringf( "ERROR: No PlayerStart found for map" ) ) )
	{
		return;
	}
	
	m_playerStartPosition = ParseXMLAttribute( *playerStartElement, "pos", Vec2( -999.f, -999.f ) );
	m_playerStartYaw = ParseXMLAttribute( *playerStartElement, "yaw", -999.f );

	if( !g_theConsole->GuaranteeOrError( m_playerStartPosition != Vec2( -999.f, -999.f ), Stringf( "ERROR: Missing player start position for map" ) ) )
	{
		//return;
	}

	if( !g_theConsole->GuaranteeOrError( m_playerStartYaw != -999.f, Stringf( "ERROR: Missing player start yaw for map" ) ) )
	{
		//return;
	}

	for( XmlElement const* entityElement = playerStartElement->NextSiblingElement(); entityElement; entityElement = entityElement->NextSiblingElement() )
	{
		std::string entityType = entityElement->Name();
		std::string entityName = ParseXMLAttribute( *entityElement, "name", "INVALID" );
		Vec2 position = ParseXMLAttribute( *entityElement, "pos", Vec2(-1.f,-1.f) );
		float yaw = ParseXMLAttribute( *entityElement, "yaw", 0.f );

		Map::SpawnNewEntityOfType( entityName, position, Vec3( 0.f, 0.f, yaw ) );
	}

	//SpawnEntities();
}

void TileMap::SpawnEntities()
{
	EntityDefinition const* entityDef = EntityDefinition::s_definitions["Pinky"];
	Actor* actor1 = new Actor( entityDef, Vec2( 2.5, 1.5f ), Vec3( 0.f, 0.f, 30.f ) );
	Actor* actor2 = new Actor( entityDef, Vec2( 4.5f, 1.5f ), Vec3( 0.f, 0.f, 85.f ) );
	m_allEntities.push_back( actor1 );
	m_allEntities.push_back( actor2 );
}

void TileMap::ResolveAllEntityWallCollisions()
{
	size_t numEntities = m_allEntities.size();
	for( size_t entityIndex = 0; entityIndex < numEntities; entityIndex++ )
	{
		Entity* entity = m_allEntities[entityIndex];
		if( entity )
		{
			ResolveEntityWallCollisions( entity );
		}
	}
}

void TileMap::ResolveEntityWallCollisions( Entity* entity )
{
	IntVec2 north( 1, 0 );
	IntVec2 east( 0, 1 );
	IntVec2 south( -1, 0 );
	IntVec2 west( 0, -1 );
	
	IntVec2 northEast( 1, 1 );
	IntVec2 southEast( -1, 1 );
	IntVec2 southWest( -1, -1 );
	IntVec2 northWest( 1,-1 );

	ResolveEntityWallCollision( entity, north );
	ResolveEntityWallCollision( entity, east );
	ResolveEntityWallCollision( entity, south );
	ResolveEntityWallCollision( entity, west );
	
	ResolveEntityWallCollision( entity, northEast );
	ResolveEntityWallCollision( entity, southEast );
	ResolveEntityWallCollision( entity, southWest );
	ResolveEntityWallCollision( entity, northWest );
}

void TileMap::ResolveEntityWallCollision( Entity* entity, IntVec2 const& direction )
{
	bool isPushed = entity->IsPushedByWalls();
	if( !isPushed )
	{
		return;
	}

	IntVec2 tilePosition = (IntVec2)entity->GetPosition();
	tilePosition += direction;

	Vec2 tileBottomLeft = Vec2( tilePosition );
	Vec2 tileTopRight = Vec2( tileBottomLeft + Vec2( 1.f, 1.f ) );


	if( IsTileSolidAtCoords( tilePosition ) )
	{
		Vec2 entityPos = entity->GetPosition();
		float entityRadius = entity->GetPhysicsRadius();
		AABB2 tileaabb = AABB2( tileBottomLeft, tileTopRight );
		PushDiscOutOfAABB2D( entityPos, entityRadius, tileaabb );

		entity->SetPosition( entityPos );
	}
}

