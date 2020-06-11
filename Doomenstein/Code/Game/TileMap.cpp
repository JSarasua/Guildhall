#include "Game/TileMap.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/MapRegionType.hpp"

extern RenderContext* g_theRenderer;

TileMap::TileMap( XmlElement const& element, Game* game ) : Map( game )
{
	m_mapSize = ParseXMLAttribute( element, "dimensions", IntVec2( 8, 8 ) );

	XmlElement const* legendElement = element.FirstChildElement( "Legend" );
	XmlElement const* mapRowsElement = element.FirstChildElement( "MapRows" );

	GUARANTEE_OR_DIE( legendElement, "No Legend found in map definition" );
	GUARANTEE_OR_DIE( mapRowsElement, "No MapRows found in MapDefinition" );
	ParseLegend( *legendElement );
	SpawnTiles( *mapRowsElement );
}

TileMap::~TileMap()
{

}

void TileMap::Update( float deltaSeconds )
{
	m_vertsToRender.clear();
	m_tileIndices.clear();
	Vec3 tileOffset = Vec3( 0.5f, 0.5f, 0.5f );


	size_t tileCount = m_tiles.size();
	for( size_t tileIndex = 0; tileIndex < tileCount; tileIndex++ )
	{
		MapTile const& tile = m_tiles[tileIndex];
		AppendIndexedVertsTestCube( m_vertsToRender, m_tileIndices, tile );
	}
}

void TileMap::Render()
{
	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->DrawIndexedVertexArray( m_vertsToRender, m_tileIndices );
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

	Vec2 bLeft( 0.f, 0.f );
	Vec2 tRight( 1.f, 1.f );
	Vec2 tLeft( 0.f, 1.f );
	Vec2 bRight( 1.f, 0.f );

	Vec3 forward	= Vec3( 0.f, 0.f, 1.f );
	Vec3 right	= Vec3( 1.f, 0.f, 0.f );
	Vec3 back		= Vec3( 0.f, 0.f, -1.f );
	Vec3 left		= Vec3( -1.f, 0.f, 0.f );
	Vec3 up		= Vec3( 0.f, 1.f, 0.f );
	Vec3 down		= Vec3( 0.f, -1.f, 0.f );

	Vertex_PCUTBN cubeVerts[24] =
	{
		//Top quad
		Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, forward, right, up ),		//3
		Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, forward, right, up ),		//2
		Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		bRight, forward, right, up ),		//1
		Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		bLeft,  forward, right, up ),	//0
		
		//Bottom quad
		Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bRight, back, left, up ),		//11
		Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bLeft, back, left, up ),		//10
		Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	tLeft, back, left, up ),		//9
		Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		tRight,	back, left, up ),		//8

		//Back quad
		Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft,	right, back, up ),		//4
		Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		bLeft, right, back, up ),		//5
		Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bRight, right, back, up ),		//6
		Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, right, back, up ),		//7
		

		//Front quad
		Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	bRight,	left, forward, up ),		//12
		Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, left, forward, up ),		//13
		Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, left, forward, up ),			//14
		Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bLeft, left, forward, up ),		//15

		//Left quad
		Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		tRight,	up, right, back ),		//16
		Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, up, right, back ),		//17
		Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bLeft, up, right, back ),		//18
		Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,			bRight, up, right, back ),		//19

		//Right quad
		Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	bLeft,	down, right, forward ),		//20
		Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		bRight, down, right, forward ),		//21
		Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, down, right, forward ),		//22
		Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, down, right, forward )			//23
	};

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

void TileMap::AddTileIndices( std::vector<uint>& tileIndices, MapTile const& tile )
{
	bool isBackTile = (tile.m_tileCoords.x == m_mapSize.x - 1);
	bool isRightTile = (tile.m_tileCoords.y == 0);
	bool isFrontTile = (tile.m_tileCoords.x == 0);
	bool isLeftTile = (tile.m_tileCoords.y == m_mapSize.y - 1);
	bool isEdgeTile = isBackTile || isRightTile || isLeftTile || isFrontTile;

	IntVec2 left = IntVec2( 0, 1 );
	IntVec2 right = IntVec2( 0, -1 );
	IntVec2 forward = IntVec2( -1, 0 );
	IntVec2 backward = IntVec2( 1, 0 );

	IntVec2 currentTileCoords = tile.m_tileCoords;

	bool isLeftTileSolid = IsTileSolidAtCoords( currentTileCoords + left );
	bool isRightTileSolid = IsTileSolidAtCoords( currentTileCoords + right );
	bool isBackTileSolid = IsTileSolidAtCoords( currentTileCoords + backward );
	bool isFrontTileSolid = IsTileSolidAtCoords( currentTileCoords + forward );

	if( !isEdgeTile )
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

void TileMap::ParseLegend( XmlElement const& legendElement )
{
	//Passed in element with name Legend
	//Has sub elements with format <Tile glyph="#" regionType="WoodWall"/>

	for( XmlElement const* element = legendElement.FirstChildElement(); element; element=element->NextSiblingElement() ) 
	{
		XmlAttribute const* glyphAttribute = element->FindAttribute( "glyph" );
		XmlAttribute const* regionAttribute = element->FindAttribute( "regionType" );
		std::string elementName = element->Name();

		GUARANTEE_OR_DIE( elementName == "Tile", "Legend has elements not named Tile");
		GUARANTEE_OR_DIE( glyphAttribute, "There was no glyph attribute in Legend" );
		GUARANTEE_OR_DIE( regionAttribute, "There was no regionType attribute in Legend" );

		unsigned char glyph = ParseXMLAttribute( *element, "glyph", '\0' );
		std::string regionTypeStr = ParseXMLAttribute( *element, "regionType", "INVALID" );

		GUARANTEE_OR_DIE( glyph != '\0', "glyph was an invalid character or string" );
		GUARANTEE_OR_DIE( regionTypeStr != "INVALID", "Invalid region type" );

		m_legend[glyph] = regionTypeStr;
	}
}

void TileMap::SpawnTiles( XmlElement const& mapRowsElement )
{
	std::vector<std::string> mapRows;
	for( XmlElement const* element = mapRowsElement.FirstChildElement(); element; element=element->NextSiblingElement() )
	{
		std::string mapRow = ParseXMLAttribute( *element, "tiles", "INVALID" );

		GUARANTEE_OR_DIE( mapRow != "INVALID" && mapRow.length() == m_mapSize.x, "Invalid MapRow tiles" );
		mapRows.push_back( mapRow );
	}
	GUARANTEE_OR_DIE( mapRows.size() == m_mapSize.y, "Number of Map rows doesn't match map height dimension" );

	uint heightIndex = 0;
	for( int mapRowsIndex = (int)mapRows.size() - 1; mapRowsIndex >= 0; mapRowsIndex-- )
	{
		SpawnMapRow( mapRows[mapRowsIndex], heightIndex );
		heightIndex++;
	}

// 	for( int heightIndex = 0; heightIndex < m_mapSize.y; heightIndex++ )
// 	{
// 		for( int widthIndex = 0; widthIndex < m_mapSize.x; widthIndex++ )
// 		{
// 			IntVec2 tileCoords = IntVec2( widthIndex, heightIndex );
// 			MapTile mapTile = MapTile( tileCoords );
// 			mapTile.m_tempIsSolid = false;
// 
// 			if( widthIndex == 0 || heightIndex == 0 || widthIndex + 1 == m_mapSize.x || heightIndex + 1 == m_mapSize.y )
// 			{
// 				mapTile.m_tempIsSolid = true;
// 			}
// 			m_tiles.push_back( mapTile );
// 
// 		}
// 	}
}

void TileMap::SpawnMapRow( std::string const& mapRow, uint heightIndex  )
{
	for( size_t mapRowIndex = 0; mapRowIndex < mapRow.size(); mapRowIndex++ )
	{
		char glyph = mapRow[mapRowIndex];
		auto legendIter = m_legend.find(glyph);
		GUARANTEE_OR_DIE( legendIter != m_legend.end(), "Glyph not found in legend" );
		std::string mapRegionStr = legendIter->second;
		MapRegionType* mapRegionType = MapRegionType::GetMapRegionTypeByString( mapRegionStr );

		IntVec2 tileCoords = IntVec2( (int)mapRowIndex, heightIndex );
		m_tiles.push_back( MapTile( tileCoords, mapRegionType ) );
	}
}

