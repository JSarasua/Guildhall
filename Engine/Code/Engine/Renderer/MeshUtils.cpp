#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

void AppendIndexedVertsCube( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, float cubeHalfHeight /*= 1.f */ )
{
	float c = cubeHalfHeight;

	Vec2 bLeft( 0.f, 0.f );
	Vec2 tRight( 1.f, 1.f );
	Vec2 tLeft( 0.f, 1.f );
	Vec2 bRight( 1.f, 0.f );

	Vertex_PCU cubeVerts[24] =
	{
		//Front Quad
		Vertex_PCU( Vec3( -c, -c, c ), Rgba8::WHITE,		bLeft ),	//0
		Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		bRight ),		//1
		Vertex_PCU( Vec3( c, c, c ), Rgba8::WHITE,		tRight ),		//2

		//Vertex_PCU( Vec3( -c, -c, c ), Rgba8::WHITE,		bLeft ),	//0
		//Vertex_PCU( Vec3( c, c, c ), Rgba8::WHITE,		tRight ),	//2
		Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,		tLeft ),		//3

		//Right Quad
		Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		bLeft ),		//4
		Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bRight ),		//5
		Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),		//6

		//Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		bLeft ),	//4
		//Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),	//6
		Vertex_PCU( Vec3( c, c, c ), Rgba8::WHITE,		tLeft ),		//7

		//Back Quad
		Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bLeft ),		//8
		Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bRight ),		//9
		Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tRight ),		//10

		//Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bLeft ),		//8
		//Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tRight ),		//10
		Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tLeft ),		//11

		//Left Quad
		Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//12
		Vertex_PCU( Vec3( -c, -c, c ), Rgba8::WHITE,	bRight ),		//13
		Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,	tRight ),			//14

		//Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//12
		//Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,		tRight ),	//14
		Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tLeft ),		//15

		//Top Quad
		Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,		bLeft ),		//16
		Vertex_PCU( Vec3( c, c, c ), Rgba8::WHITE,		bRight ),		//17
		Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),		//18

		//Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,		bLeft ),	//16
		//Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),	//18
		Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tLeft ),		//19

		//Bottom Quad
		Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//20
		Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bRight ),		//21
		Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		tRight ),		//22

		//Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//20
		//Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		tRight ),	//22
		Vertex_PCU( Vec3( -c, -c, c ), Rgba8::WHITE,	tLeft )			//23
	};

	for( int vertexIndex = 0; vertexIndex < 24; vertexIndex++ )
	{
		masterVertexList.push_back( cubeVerts[vertexIndex] );
	}

	uint cubeIndices[36] =
	{
		//Front Quad
		0,
		1,
		2,

		0,
		2,
		3,

		//Right Quad
		4,
		5,
		6,

		4,
		6,
		7,

		//Back Quad
		8,
		9,
		10,

		8 ,
		10,
		11,

		//Left Quad
		12,
		13,
		14,

		12,
		14,
		15,

		//Top Quad
		16,
		17,
		18,

		16,
		18,
		19,

		//Bottom Quad
		20,
		21,
		22,

		20,
		22,
		23
	};

	for( int indicesIndex = 0; indicesIndex < 36; indicesIndex++ )
	{
		masterIndexList.push_back( cubeIndices[indicesIndex] );
	}
}

void AppendIndexedVertsSphere( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, Vec3 center, float sphereRadius /*= 1.f*/, uint horizontalSlices /*= 64*/, uint verticalSlices /*= 64*/, Rgba8 color /*= Rgba8::WHITE */ )
{
	//first point is top
	//last point is bottom
	//rotating counter clockwise
	//top piece is triangles
	//Afterwards is quads


//  	int verticalSlices = 64;		//like an orange slice, change based on theta
//  	int horizontalSlices = 64;		//like latitude lines, change based on phi
	float thetaIncrements = 360.f / (float)verticalSlices;
	float phiIncrements = 180.f / (float)horizontalSlices;

	Vec3 topPoint = Vec3::MakeFromSphericalDegrees( 0.f, 90.f, 1.f );
	topPoint += center;
	Vec2 topUV = Vec2( .5f, 1.f );
	Vec3 bottomPoint = Vec3::MakeFromSphericalDegrees( 0.f, -90.f, 1.f );
	bottomPoint += center;
	Vec2 bottomUV = Vec2( 0.5f, 0.f );

	std::vector<Vec3> vertexList;
	std::vector<Vec2> uvList;
	vertexList.push_back( topPoint );
	uvList.push_back( topUV );

	for( float latitudeDegrees = 90.f - phiIncrements; latitudeDegrees > -90.f; latitudeDegrees -= phiIncrements )
	{

		for( float longitudeDegrees = 360.f; longitudeDegrees >= 0.f; longitudeDegrees -= thetaIncrements )
		{
			Vec3 currentPoint = Vec3::MakeFromSphericalDegrees( longitudeDegrees, latitudeDegrees, sphereRadius );
			currentPoint += center;
			vertexList.push_back( currentPoint );

			float u = RangeMap( 360.f, 0.f, 0.f, 1.f, longitudeDegrees );
			float v = RangeMap( -90.f, 90.f, 0.f, 1.f, latitudeDegrees );
			uvList.push_back( Vec2( u, v ) );
		}
	}
	vertexList.push_back( bottomPoint );
	uvList.push_back( bottomUV );


	std::vector<uint> indicesList;
	//Top of sphere
	for( uint topIndices = 1; topIndices < verticalSlices + 1; topIndices++ )
	{
		indicesList.push_back( 0 );
		indicesList.push_back( topIndices );
		indicesList.push_back( topIndices + 1 );
	}

	//Body of Sphere
	//reduce by 2 the number of vertical slices for the top and bottom of the sphere
	for( uint latitudeIndex = 0; latitudeIndex < horizontalSlices - 2; latitudeIndex++ )
	{
		for( uint longitudeIndex = 0; longitudeIndex < verticalSlices; longitudeIndex++ )
		{

			int topLeft = latitudeIndex * (verticalSlices + 1) + longitudeIndex + 1;
			int topRight = topLeft + 1;
			int bottomLeft = (latitudeIndex + 1) * (verticalSlices + 1) + longitudeIndex + 1;
			int bottomRight = bottomLeft + 1;

			//left quad triangle
			indicesList.push_back( topLeft );
			indicesList.push_back( bottomLeft );
			indicesList.push_back( topRight );

			//right quad triangle
			indicesList.push_back( topRight );
			indicesList.push_back( bottomLeft );
			indicesList.push_back( bottomRight );
		}
	}

	//Bottom of Sphere
	for( uint bottomIndices = ((uint)vertexList.size() - 1) - verticalSlices; bottomIndices < ((uint)vertexList.size() - 1); bottomIndices++ )
	{
		if( bottomIndices == ((uint)vertexList.size() - 2) )
		{
			indicesList.push_back( bottomIndices );
			indicesList.push_back( (uint)vertexList.size() - 1 );
			indicesList.push_back( bottomIndices - (verticalSlices - 1) );
		}
		else
		{
			indicesList.push_back( bottomIndices );
			indicesList.push_back( (uint)vertexList.size() - 1 );
			indicesList.push_back( bottomIndices + 1 );
		}

	}

	for( size_t vertexIndex = 0; vertexIndex < vertexList.size(); vertexIndex++ )
	{
		masterVertexList.push_back( Vertex_PCU( vertexList[vertexIndex], color, uvList[vertexIndex] ) );
	}
	//	masterVertexList.insert( masterVertexList.end(), vertexList.begin(), vertexList.end() );
	masterIndexList.insert( std::end( masterIndexList ), std::begin( indicesList ), std::end( indicesList ) );

}

