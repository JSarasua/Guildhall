#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/BufferAttribute.hpp"
#include "Engine/Math/MathUtils.hpp"

BufferAttribute const Vertex_PCUTBN::LAYOUT[] ={
	BufferAttribute( "POSITION",	BUFFER_FORMAT_VEC3,      		offsetof( Vertex_PCUTBN, position ) ),
	BufferAttribute( "COLOR",		BUFFER_FORMAT_R8G8B8A8_UNORM, 	offsetof( Vertex_PCUTBN, tint ) ),
	BufferAttribute( "TEXCOORD",    BUFFER_FORMAT_VEC2,      		offsetof( Vertex_PCUTBN, uvTexCoords ) ),
	BufferAttribute( "TANGENT",		BUFFER_FORMAT_VEC3,				offsetof( Vertex_PCUTBN, tangent ) ),
	BufferAttribute( "BITANGENT",		BUFFER_FORMAT_VEC3,				offsetof( Vertex_PCUTBN, bitangent ) ),
	BufferAttribute( "NORMAL",		BUFFER_FORMAT_VEC3,				offsetof( Vertex_PCUTBN, normal ) ),
	BufferAttribute() // end - terminator element; 
};

void Vertex_PCUTBN::AppendIndexedVertsCube( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList, float cubeHalfHeight /*= 1.f */ )
{
	float c = cubeHalfHeight;

	Vec2 bLeft( 0.f, 0.f );
	Vec2 tRight( 1.f, 1.f );
	Vec2 tLeft( 0.f, 1.f );
	Vec2 bRight( 1.f, 0.f );

	Vec3 frontNormal	= Vec3( 0.f, 0.f, 1.f );
	Vec3 rightNormal	= Vec3( 1.f, 0.f, 0.f );
	Vec3 backNormal		= Vec3( 0.f, 0.f, -1.f );
	Vec3 leftNormal		= Vec3( -1.f, 0.f, 0.f );
	Vec3 upNormal		= Vec3( 0.f, 1.f, 0.f );
	Vec3 downNormal		= Vec3( 0.f, -1.f, 0.f );

	Vertex_PCUTBN cubeVerts[24] =
	{
		//Front Quad
		Vertex_PCUTBN( Vec3( -c, -c, c ), Rgba8::WHITE,		bLeft,  frontNormal ),	//0
		Vertex_PCUTBN( Vec3( c, -c, c ), Rgba8::WHITE,		bRight, frontNormal ),		//1
		Vertex_PCUTBN( Vec3( c, c, c ), Rgba8::WHITE,		tRight, frontNormal ),		//2

		//Vertex_PCUTBN( Vec3( -c, -c, c ), Rgba8::WHITE,		bLeft ),	//0
		//Vertex_PCUTBN( Vec3( c, c, c ), Rgba8::WHITE,		tRight ),	//2
		Vertex_PCUTBN( Vec3( -c, c, c ), Rgba8::WHITE,		tLeft, frontNormal ),		//3

		//Right Quad
		Vertex_PCUTBN( Vec3( c, -c, c ), Rgba8::WHITE,		bLeft,	rightNormal ),		//4
		Vertex_PCUTBN( Vec3( c, -c, -c ), Rgba8::WHITE,		bRight, rightNormal ),		//5
		Vertex_PCUTBN( Vec3( c, c, -c ), Rgba8::WHITE,		tRight, rightNormal ),		//6

		//Vertex_PCUTBN( Vec3( c, -c, c ), Rgba8::WHITE,		bLeft ),	//4
		//Vertex_PCUTBN( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),	//6
		Vertex_PCUTBN( Vec3( c, c, c ), Rgba8::WHITE,		tLeft, rightNormal ),		//7

		//Back Quad
		Vertex_PCUTBN( Vec3( c, -c, -c ), Rgba8::WHITE,		bLeft,	backNormal ),		//8
		Vertex_PCUTBN( Vec3( -c, -c, -c ), Rgba8::WHITE,	bRight, backNormal ),		//9
		Vertex_PCUTBN( Vec3( -c, c, -c ), Rgba8::WHITE,		tRight, backNormal ),		//10

		//Vertex_PCUTBN( Vec3( c, -c, -c ), Rgba8::WHITE,	bLeft ),		//8
		//Vertex_PCUTBN( Vec3( -c, c, -c ), Rgba8::WHITE,	tRight ),		//10
		Vertex_PCUTBN( Vec3( c, c, -c ), Rgba8::WHITE,		tLeft, backNormal ),		//11

		//Left Quad
		Vertex_PCUTBN( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft,	leftNormal ),		//12
		Vertex_PCUTBN( Vec3( -c, -c, c ), Rgba8::WHITE,		bRight, leftNormal ),		//13
		Vertex_PCUTBN( Vec3( -c, c, c ), Rgba8::WHITE,		tRight, leftNormal ),			//14

		//Vertex_PCUTBN( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//12
		//Vertex_PCUTBN( Vec3( -c, c, c ), Rgba8::WHITE,		tRight ),	//14
		Vertex_PCUTBN( Vec3( -c, c, -c ), Rgba8::WHITE,		tLeft, leftNormal ),		//15

		//Top Quad
		Vertex_PCUTBN( Vec3( -c, c, c ), Rgba8::WHITE,		bLeft,	upNormal ),		//16
		Vertex_PCUTBN( Vec3( c, c, c ), Rgba8::WHITE,		bRight, upNormal ),		//17
		Vertex_PCUTBN( Vec3( c, c, -c ), Rgba8::WHITE,		tRight, upNormal ),		//18

		//Vertex_PCUTBN( Vec3( -c, c, c ), Rgba8::WHITE,		bLeft ),	//16
		//Vertex_PCUTBN( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),	//18
		Vertex_PCUTBN( Vec3( -c, c, -c ), Rgba8::WHITE,			tLeft, upNormal ),		//19

		//Bottom Quad
		Vertex_PCUTBN( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft,	downNormal ),		//20
		Vertex_PCUTBN( Vec3( c, -c, -c ), Rgba8::WHITE,		bRight, downNormal ),		//21
		Vertex_PCUTBN( Vec3( c, -c, c ), Rgba8::WHITE,		tRight, downNormal),		//22

		//Vertex_PCUTBN( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//20
		//Vertex_PCUTBN( Vec3( c, -c, c ), Rgba8::WHITE,		tRight ),	//22
		Vertex_PCUTBN( Vec3( -c, -c, c ), Rgba8::WHITE,		tLeft, downNormal )			//23
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

void Vertex_PCUTBN::AppendIndexedVertsSphere( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList, float sphereRadius /*= 1.f */ )
{
	//first point is top
	//last point is bottom
	//rotating counter clockwise
	//top piece is triangles
	//Afterwards is quads


	int numOfVerticalSlices = 64;		//like an orange slice, change based on theta
	int numOfHorizontalSlices = 64;		//like latitude lines, change based on phi
	float thetaIncrements = 360.f / (float)numOfVerticalSlices;
	float phiIncrements = 180.f / (float)numOfHorizontalSlices;

	Vec3 topPoint = Vec3::MakeFromSphericalDegrees( 0.f, 90.f, 1.f );
	Vec2 topUV = Vec2( .5f, 1.f );
	Vec3 bottomPoint = Vec3::MakeFromSphericalDegrees( 0.f, -90.f, 1.f );
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
	for( int topIndices = 1; topIndices < numOfVerticalSlices + 1; topIndices++ )
	{
		indicesList.push_back( 0 );
		indicesList.push_back( topIndices );
		indicesList.push_back( topIndices + 1 );
	}

	//Body of Sphere
	//reduce by 2 the number of vertical slices for the top and bottom of the sphere
	for( int latitudeIndex = 0; latitudeIndex < numOfHorizontalSlices - 2; latitudeIndex++ )
	{
		for( int longitudeIndex = 0; longitudeIndex < numOfVerticalSlices; longitudeIndex++ )
		{

			int topLeft = latitudeIndex * (numOfVerticalSlices + 1) + longitudeIndex + 1;
			int topRight = topLeft + 1;
			int bottomLeft = (latitudeIndex + 1) * (numOfVerticalSlices + 1) + longitudeIndex + 1;
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
	for( uint bottomIndices = ((uint)vertexList.size() - 1) - numOfVerticalSlices; bottomIndices < ((uint)vertexList.size() - 1); bottomIndices++ )
	{
		if( bottomIndices == ((uint)vertexList.size() - 2) )
		{
			indicesList.push_back( bottomIndices );
			indicesList.push_back( (uint)vertexList.size() - 1 );
			indicesList.push_back( bottomIndices - (numOfVerticalSlices - 1) );
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
		masterVertexList.push_back( Vertex_PCUTBN( vertexList[vertexIndex], Rgba8::WHITE, uvList[vertexIndex], vertexList[vertexIndex] ) );
	}
	//	masterVertexList.insert( masterVertexList.end(), vertexList.begin(), vertexList.end() );
	masterIndexList.insert( std::end( masterIndexList ), std::begin( indicesList ), std::end( indicesList ) );

}

void Vertex_PCUTBN::AppendVerts4Points( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList, Vec3 const& p0, Vec3 const& p1, Vec3 const& p2, Vec3 const& p3, Rgba8 const& color, AABB2 const& uvs /*= AABB2() */ )
{
	Vec3 p0Top1 = p1 - p0;
	Vec3 p0Top3 = p3 - p0;
	Vec3 normal = CrossProduct( p0Top1, p0Top3 );

	uint currentIndex = (uint)masterIndexList.size();

	masterVertexList.push_back( Vertex_PCUTBN( p0, color, uvs.mins, normal ) );
	masterVertexList.push_back( Vertex_PCUTBN( p1, color, Vec2( uvs.maxs.x, uvs.mins.y ), normal ) );
	masterVertexList.push_back( Vertex_PCUTBN( p2, color, uvs.maxs, normal ) );

	masterVertexList.push_back( Vertex_PCUTBN( p0, color, uvs.mins, normal ) );
	masterVertexList.push_back( Vertex_PCUTBN( p2, color, uvs.maxs, normal ) );
	masterVertexList.push_back( Vertex_PCUTBN( p3, color, Vec2( uvs.mins.x, uvs.maxs.y ), normal ) );


	masterIndexList.push_back( currentIndex );
	masterIndexList.push_back( currentIndex + 1 );
	masterIndexList.push_back( currentIndex + 2 );

	masterIndexList.push_back( currentIndex + 3 );
	masterIndexList.push_back( currentIndex + 4 );
	masterIndexList.push_back( currentIndex + 5 );
}

Vertex_PCUTBN::Vertex_PCUTBN( Vertex_PCUTBN const& copyFrom )
	: position( copyFrom.position )
	, tint( copyFrom.tint )
	, uvTexCoords( copyFrom.uvTexCoords )
	, normal( copyFrom.normal )
{}

Vertex_PCUTBN::Vertex_PCUTBN( Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords, Vec3 const& normal )
	: position( position )
	, tint( tint )
	, uvTexCoords( uvTexCoords )
	, normal( normal )
{}

