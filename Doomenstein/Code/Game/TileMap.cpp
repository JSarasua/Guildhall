#include "Game/TileMap.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext* g_theRenderer;

TileMap::TileMap( Game* game ) : Map( game )
{
	m_mapSize = IntVec2( 8, 8 );
	SpawnTiles();
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
	if( tile.m_tempIsSolid )
	{
		//Top quad
		cubeIndices.push_back( 0 );
		cubeIndices.push_back( 1 );
		cubeIndices.push_back( 2 );
		cubeIndices.push_back( 0 );
		cubeIndices.push_back( 2 );
		cubeIndices.push_back( 3 );

		//Bottom quad
		cubeIndices.push_back( 4 );
		cubeIndices.push_back( 5 );
		cubeIndices.push_back( 6 );
		cubeIndices.push_back( 4 );
		cubeIndices.push_back( 6 );
		cubeIndices.push_back( 7 );
	}

	//Back quad
	cubeIndices.push_back( 8 );
	cubeIndices.push_back( 9 );
	cubeIndices.push_back( 10 );
	cubeIndices.push_back( 8 );
	cubeIndices.push_back( 10 );
	cubeIndices.push_back( 11 );

	//Front quad
	cubeIndices.push_back( 12 );
	cubeIndices.push_back( 13 );
	cubeIndices.push_back( 14 );
	cubeIndices.push_back( 12 );
	cubeIndices.push_back( 14 );
	cubeIndices.push_back( 15 );

	//Left quad
	cubeIndices.push_back( 16 );
	cubeIndices.push_back( 17 );
	cubeIndices.push_back( 18 );
	cubeIndices.push_back( 16 );
	cubeIndices.push_back( 18 );
	cubeIndices.push_back( 19 );

	//Right quad
	cubeIndices.push_back( 20 );
	cubeIndices.push_back( 21 );
	cubeIndices.push_back( 22 );
	cubeIndices.push_back( 20 );
	cubeIndices.push_back( 22 );
	cubeIndices.push_back( 23 );


	

// 	uint cubeIndices[36] =
// 	{
// 		//Top quad
// 		0,
// 		1,
// 		2,
// 
// 		0,
// 		2,
// 		3,
// 
// 		//Bottom quad
// 		4,
// 		5,
// 		6,
// 
// 		4,
// 		6,
// 		7,
// 
// 		//Back quad
// 		8,
// 		9,
// 		10,
// 
// 		8 ,
// 		10,
// 		11,
// 
// 		//Front quad
// 		12,
// 		13,
// 		14,
// 
// 		12,
// 		14,
// 		15,
// 
// 		//Left quad
// 		16,
// 		17,
// 		18,
// 
// 		16,
// 		18,
// 		19,
// 
// 		//Right quad
// 		20,
// 		21,
// 		22,
// 
// 		20,
// 		22,
// 		23
// 	};


	for( size_t indicesIndex = 0; indicesIndex < cubeIndices.size(); indicesIndex++ )
	{
		masterIndexList.push_back( cubeIndices[indicesIndex] + currentVertexListSize );
	}
}

void TileMap::SpawnTiles()
{
	for( int heightIndex = 0; heightIndex < m_mapSize.y; heightIndex++ )
	{
		for( int widthIndex = 0; widthIndex < m_mapSize.x; widthIndex++ )
		{
			IntVec2 tileCoords = IntVec2( widthIndex, heightIndex );
			MapTile mapTile = MapTile( tileCoords );
			mapTile.m_tempIsSolid = true;
			m_tiles.push_back( mapTile );

		}
	}
}

