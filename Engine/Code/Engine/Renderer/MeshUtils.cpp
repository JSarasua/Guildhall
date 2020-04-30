#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"

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

void AppendIndexedVertsCylinder( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, Vec3 const& startPos, float startRadius, Vec3 const& endPos, float endRadius, uint cuts /*= 16*/, Rgba8 color /*= Rgba8::WHITE */ )
{
	float height = GetDistance3D( startPos, endPos );

	float degIncr = 360.f / (float)cuts;
	std::vector<Vec3> vertexes;
	std::vector<uint> indices;

	for( uint cutIndex = 0; cutIndex < cuts; cutIndex++ )
	{
		float currentAngle = degIncr * (float)cutIndex;
		Vec2 baseVert = Vec2::MakeFromPolarDegrees( currentAngle, startRadius );
		Vec3 baseVert3D = (Vec3)baseVert;
		vertexes.push_back( baseVert3D );
	}

	for( uint cutIndex = 0; cutIndex < cuts; cutIndex++ )
	{
		float currentAngle = degIncr * (float)cutIndex;
		Vec2 topVert = Vec2::MakeFromPolarDegrees( currentAngle, endRadius );
		Vec3 topVert3D = (Vec3)topVert;
		topVert3D.z -= height;
		vertexes.push_back( topVert3D );
	}

	//0,1,2
	//0,2,3
	//0,3,4

	//Bottom of Cylinder indices
	for( uint cutIndex = 0; cutIndex < cuts - 2; cutIndex++ )
	{
		indices.push_back( 0 );
		indices.push_back( cutIndex + 1 );
		indices.push_back( cutIndex + 2 );
	}


	//for a 5 cut
	//0 5 1
	//5 6 1

	//1 6 2
	//6 7 2
	
	//etc

	//Between the two circles
	for( uint cutIndex = 0; cutIndex < cuts; cutIndex++ )
	{
		if( cutIndex == cuts - 1 )
		{
			indices.push_back( cutIndex );
			indices.push_back( cuts + cutIndex );
			indices.push_back( 0 );

			indices.push_back( cuts + cutIndex );
			indices.push_back( cuts );
			indices.push_back( 0 );
		}
		else
		{
			indices.push_back( cutIndex );
			indices.push_back( cuts + cutIndex );
			indices.push_back( cutIndex + 1 );

			indices.push_back( cuts + cutIndex );
			indices.push_back( cuts + cutIndex + 1 );
			indices.push_back( cutIndex + 1 );
		}

	}

	//Top of cylinder indices
	for( uint cutIndex = 0; cutIndex < cuts - 2; cutIndex++ )
	{
		indices.push_back( cuts + cutIndex + 2 );
		indices.push_back( cuts + cutIndex + 1 );
		indices.push_back( cuts );
	}

	uint startIndex = (uint)masterVertexList.size();
	//transform
	Mat44 transform = LookAtAndMoveToWorld( startPos, endPos, Vec3(0.f, 1.f, 0.f ) );
	for( size_t vertexIndex = 0; vertexIndex < vertexes.size(); vertexIndex++ )
	{
		Vec3 vertex = vertexes[vertexIndex];
		vertex = transform.TransformPosition3D( vertex );

		masterVertexList.push_back( Vertex_PCU( vertex, color, Vec2() ) );
	}

	for( size_t indexIndex = 0; indexIndex < indices.size(); indexIndex++ )
	{
		uint currentIndex = indices[indexIndex] + startIndex;
		masterIndexList.push_back( currentIndex );
	}
}

void AppendIndexedVertsCone( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, Vec3 const& basePos, float baseRadius, Vec3 const& endPos, uint cuts /*= 16*/, Rgba8 color /*= Rgba8::WHITE */ )
{
	AppendIndexedVertsCylinder( masterVertexList, masterIndexList, basePos, baseRadius, endPos, 0.f, cuts, color );
}

void LoadOBJToVertexArray( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList, char const* filename, MeshImportOptions_t const& options )
{
	size_t fileSize = 0;
	char const* objFile = (char const*)FileReadToNewBuffer( filename, &fileSize );
	std::vector<std::string> delimetedObj = SplitStringOnDelimeter( objFile, '\n' );

	Mat44 tranform = options.m_transform.ToMatrix();

	std::vector<Vec3> vertexes;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvs;
	size_t delimetedOBJSize = delimetedObj.size();

	std::string currentVertexIndex = "1";
	std::string currentUVIndex = "1";
	std::string currentNormalIndex = "1";

	for( size_t lineIndex = 0; lineIndex < delimetedOBJSize; lineIndex++ )
	{
		std::string& currentLine = delimetedObj[lineIndex];
		if( currentLine.length() < 3 )
		{
			continue;
		}
		char const firstChar = currentLine.at(0);
		char const secondChar = currentLine.at(1);
		if( firstChar == 'v' )
		{
			if( secondChar == ' ' )
			{
				//starting at 3rd char split on space
				std::string dataString = currentLine.substr( 3 );
				Vec3 vertex;
				vertex.SetFromText( dataString.c_str(), ' ' );
				vertex = tranform.TransformPosition3D( vertex );
				vertexes.push_back( vertex );
			}
			else if( secondChar == 'n' )
			{
				std::string dataString = currentLine.substr( 3 );
				Vec3 normal;
				normal.SetFromText( dataString.c_str(), ' ' );
				normals.push_back( normal );
			}
			else if( secondChar == 't' )
			{
				std::string dataString = currentLine.substr( 3 );
				Vec3 uv3D;
				uv3D.SetFromText( dataString.c_str(), ' ' );
				Vec2 uv2D = Vec2( uv3D );

				if( options.m_invertV )
				{
					uv2D.y = 1.f - uv2D.y;
				}

				uvs.push_back( uv2D );
			}

		}
		else if( firstChar == 'f' )
		{
			std::string dataString = currentLine.substr( 2 );
			//index_vert/index_normal/index_texture
			std::vector<std::string> faces = SplitStringOnDelimeter( dataString.c_str(), ' ' );
			if( faces.back() == "" )
			{
				faces.pop_back();
			}
			if( faces.size() == 3 )
			{
				std::vector<std::string> vertexData0Str = SplitStringOnDelimeter( faces[0].c_str(), '/' );
				std::vector<std::string> vertexData1Str = SplitStringOnDelimeter( faces[1].c_str(), '/' );
				std::vector<std::string> vertexData2Str = SplitStringOnDelimeter( faces[2].c_str(), '/' );

				if( vertexData0Str[0] == "" )
				{
					vertexData0Str[0] = currentVertexIndex;
				}
				else
				{
					currentVertexIndex = vertexData0Str[0];
				}
				if( vertexData0Str[1] == "" )
				{
					vertexData0Str[1] = currentUVIndex;
				}
				else
				{
					currentUVIndex = vertexData0Str[1];
				}
				if( vertexData0Str[2] == "" )
				{
					vertexData0Str[2] = currentNormalIndex;
				}
				else
				{
					currentNormalIndex = vertexData0Str[2];
				}

				if( vertexData1Str[0] == "" )
				{
					vertexData1Str[0] = currentVertexIndex;
				}
				else
				{
					currentVertexIndex = vertexData1Str[0];
				}
				if( vertexData1Str[1] == "" )
				{
					vertexData1Str[1] = currentUVIndex;
				}
				else
				{
					currentUVIndex = vertexData1Str[1];
				}
				if( vertexData1Str[2] == "" )
				{
					vertexData1Str[2] = currentNormalIndex;
				}
				else
				{
					currentNormalIndex = vertexData1Str[2];
				}

				if( vertexData2Str[0] == "" )
				{
					vertexData2Str[0] = currentVertexIndex;
				}
				else
				{
					currentVertexIndex = vertexData2Str[0];
				}
				if( vertexData2Str[1] == "" )
				{
					vertexData2Str[1] = currentUVIndex;
				}
				else
				{
					currentUVIndex = vertexData2Str[1];
				}
				if( vertexData2Str[2] == "" )
				{
					vertexData2Str[2] = currentNormalIndex;
				}
				else
				{
					currentNormalIndex = vertexData2Str[2];
				}



				std::vector<int> vertexData0Int;
				vertexData0Int.push_back( absInt( atoi(vertexData0Str[0].c_str()) ) );
				vertexData0Int.push_back( absInt( atoi(vertexData0Str[1].c_str()) ) );
				vertexData0Int.push_back( absInt( atoi(vertexData0Str[2].c_str()) ) );

				std::vector<int> vertexData1Int;
				vertexData1Int.push_back( absInt( atoi( vertexData1Str[0].c_str() ) ) );
				vertexData1Int.push_back( absInt( atoi( vertexData1Str[1].c_str() ) ) );
				vertexData1Int.push_back( absInt( atoi( vertexData1Str[2].c_str() ) ) );

				std::vector<int> vertexData2Int;
				vertexData2Int.push_back( absInt( atoi( vertexData2Str[0].c_str() ) ) );
				vertexData2Int.push_back( absInt( atoi( vertexData2Str[1].c_str() ) ) );
				vertexData2Int.push_back( absInt( atoi( vertexData2Str[2].c_str() ) ) );

				Vertex_PCUTBN vertex0 = Vertex_PCUTBN( vertexes[vertexData0Int[0] - 1], Rgba8::WHITE, uvs[vertexData0Int[1] - 1], normals[vertexData0Int[2] - 1] );
				Vertex_PCUTBN vertex1 = Vertex_PCUTBN( vertexes[vertexData1Int[0] - 1], Rgba8::WHITE, uvs[vertexData1Int[1] - 1], normals[vertexData1Int[2] - 1] );
				Vertex_PCUTBN vertex2 = Vertex_PCUTBN( vertexes[vertexData2Int[0] - 1], Rgba8::WHITE, uvs[vertexData2Int[1] - 1], normals[vertexData2Int[2] - 1] );
				
				if( options.m_invertWindingOrder )
				{
					masterVertexList.push_back( vertex2 );
					masterVertexList.push_back( vertex1 );
					masterVertexList.push_back( vertex0 );
				}
				else
				{
					masterVertexList.push_back( vertex2 );
					masterVertexList.push_back( vertex1 );
					masterVertexList.push_back( vertex0 );

				}

				masterIndexList.push_back( (uint)masterVertexList.size() - 3 );
				masterIndexList.push_back( (uint)masterVertexList.size() - 2 );
				masterIndexList.push_back( (uint)masterVertexList.size() - 1 );

			}
			else if( faces.size() == 4 )
			{
				std::vector<std::string> vertexData0Str = SplitStringOnDelimeter( faces[0].c_str(), '/' );
				std::vector<std::string> vertexData1Str = SplitStringOnDelimeter( faces[1].c_str(), '/' );
				std::vector<std::string> vertexData2Str = SplitStringOnDelimeter( faces[2].c_str(), '/' );
				std::vector<std::string> vertexData3Str = SplitStringOnDelimeter( faces[3].c_str(), '/' );

				if( vertexData0Str[0] == "" )
				{
					vertexData0Str[0] = currentVertexIndex;
				}
				else
				{
					currentVertexIndex = vertexData0Str[0];
				}
				if( vertexData0Str[1] == "" )
				{
					vertexData0Str[1] = currentUVIndex;
				}
				else
				{
					currentUVIndex = vertexData0Str[1];
				}
				if( vertexData0Str[2] == "" )
				{
					vertexData0Str[2] = currentNormalIndex;
				}
				else
				{
					currentNormalIndex = vertexData0Str[2];
				}

				if( vertexData1Str[0] == "" )
				{
					vertexData1Str[0] = currentVertexIndex;
				}
				else
				{
					currentVertexIndex = vertexData1Str[0];
				}
				if( vertexData1Str[1] == "" )
				{
					vertexData1Str[1] = currentUVIndex;
				}
				else
				{
					currentUVIndex = vertexData1Str[1];
				}
				if( vertexData1Str[2] == "" )
				{
					vertexData1Str[2] = currentNormalIndex;
				}
				else
				{
					currentNormalIndex = vertexData1Str[2];
				}

				if( vertexData2Str[0] == "" )
				{
					vertexData2Str[0] = currentVertexIndex;
				}
				else
				{
					currentVertexIndex = vertexData2Str[0];
				}
				if( vertexData2Str[1] == "" )
				{
					vertexData2Str[1] = currentUVIndex;
				}
				else
				{
					currentUVIndex = vertexData2Str[1];
				}
				if( vertexData2Str[2] == "" )
				{
					vertexData2Str[2] = currentNormalIndex;
				}
				else
				{
					currentNormalIndex = vertexData2Str[2];
				}

				if( vertexData3Str[0] == "" )
				{
					vertexData3Str[0] = currentVertexIndex;
				}
				else
				{
					currentVertexIndex = vertexData3Str[0];
				}
				if( vertexData3Str[1] == "" )
				{
					vertexData3Str[1] = currentUVIndex;
				}
				else
				{
					currentUVIndex = vertexData3Str[1];
				}
				if( vertexData3Str[2] == "" )
				{
					vertexData3Str[2] = currentNormalIndex;
				}
				else
				{
					currentNormalIndex = vertexData3Str[2];
				}

				std::vector<int> vertexData0Int;
				vertexData0Int.push_back( absInt( atoi( vertexData0Str[0].c_str() ) ) );
				vertexData0Int.push_back( absInt( atoi( vertexData0Str[1].c_str() ) ) );
				vertexData0Int.push_back( absInt( atoi( vertexData0Str[2].c_str() ) ) );

				std::vector<int> vertexData1Int;
				vertexData1Int.push_back( absInt( atoi( vertexData1Str[0].c_str() ) ) );
				vertexData1Int.push_back( absInt( atoi( vertexData1Str[1].c_str() ) ) );
				vertexData1Int.push_back( absInt( atoi( vertexData1Str[2].c_str() ) ) );

				std::vector<int> vertexData2Int;
				vertexData2Int.push_back( absInt( atoi( vertexData2Str[0].c_str() ) ) );
				vertexData2Int.push_back( absInt( atoi( vertexData2Str[1].c_str() ) ) );
				vertexData2Int.push_back( absInt( atoi( vertexData2Str[2].c_str() ) ) );

				std::vector<int> vertexData3Int;
				vertexData3Int.push_back( absInt( atoi( vertexData3Str[0].c_str() ) ) );
				vertexData3Int.push_back( absInt( atoi( vertexData3Str[1].c_str() ) ) );
				vertexData3Int.push_back( absInt( atoi( vertexData3Str[2].c_str() ) ) );

				Vertex_PCUTBN vertex0 = Vertex_PCUTBN( vertexes[vertexData0Int[0] - 1], Rgba8::WHITE, uvs[vertexData0Int[1] - 1], normals[vertexData0Int[2] - 1] );
				Vertex_PCUTBN vertex1 = Vertex_PCUTBN( vertexes[vertexData1Int[0] - 1], Rgba8::WHITE, uvs[vertexData1Int[1] - 1], normals[vertexData1Int[2] - 1] );
				Vertex_PCUTBN vertex2 = Vertex_PCUTBN( vertexes[vertexData2Int[0] - 1], Rgba8::WHITE, uvs[vertexData2Int[1] - 1], normals[vertexData2Int[2] - 1] );
				Vertex_PCUTBN vertex3 = Vertex_PCUTBN( vertexes[vertexData3Int[0] - 1], Rgba8::WHITE, uvs[vertexData3Int[1] - 1], normals[vertexData3Int[2] - 1] );
				
				if( options.m_invertWindingOrder )
				{
					masterVertexList.push_back( vertex2 );
					masterVertexList.push_back( vertex1 );
					masterVertexList.push_back( vertex0 );

					masterVertexList.push_back( vertex3 );
					masterVertexList.push_back( vertex2 );
					masterVertexList.push_back( vertex0 );

				}
				else
				{
					masterVertexList.push_back( vertex0 );
					masterVertexList.push_back( vertex1 );
					masterVertexList.push_back( vertex2 );

					masterVertexList.push_back( vertex0 );
					masterVertexList.push_back( vertex2 );
					masterVertexList.push_back( vertex3 );
				}

				masterIndexList.push_back( (uint)masterVertexList.size() - 6 );
				masterIndexList.push_back( (uint)masterVertexList.size() - 5 );
				masterIndexList.push_back( (uint)masterVertexList.size() - 4 );

				masterIndexList.push_back( (uint)masterVertexList.size() - 3 );
				masterIndexList.push_back( (uint)masterVertexList.size() - 2 );
				masterIndexList.push_back( (uint)masterVertexList.size() - 1 );
			}
			else
			{
				ERROR_AND_DIE("Invalid number of vertices in a face");
			}
		}

	}

	if( options.m_generateNormals )
	{
		GenerateNormalsForVertexArray( masterVertexList );
	}

	if( options.m_generateTangents )
	{
		GenerateTangentsForVertexArray( masterVertexList );
	}
}

int GetNumFaces( SMikkTSpaceContext const* context )
{
	std::vector<Vertex_PCUTBN>& vertices = *(std::vector<Vertex_PCUTBN>*)(context->m_pUserData);
	return (int)vertices.size()/3;
}

int GetNumberOfVerticesForFace( SMikkTSpaceContext const* pContext, const int iFace )
{
	UNUSED( pContext );
	UNUSED( iFace );
	return 3;
}

void GetPositionForFaceVert( SMikkTSpaceContext const* pContext, float fvPosOut[], const int iFace, const int iVert )
{
	std::vector<Vertex_PCUTBN>& vertices = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;

	//Get our position
	Vec3 ourPos = vertices[indexInVertexArray].position;
	fvPosOut[0] = ourPos.x;
	fvPosOut[1] = ourPos.y;
	fvPosOut[2] = ourPos.z;
}

void GetNormalForFaceVert( SMikkTSpaceContext const* pContext, float fvNormOut[], const int iFace, const int iVert )
{
	std::vector<Vertex_PCUTBN>& vertices = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;

	//Get our position
	Vec3 ourNorm = vertices[indexInVertexArray].normal;
	fvNormOut[0] = ourNorm.x;
	fvNormOut[1] = ourNorm.y;
	fvNormOut[2] = ourNorm.z;
}

void GetUVForFaceVert( SMikkTSpaceContext const* pContext, float fvTexcOut[], const int iFace, const int iVert )
{
	std::vector<Vertex_PCUTBN>& vertices = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;

	//Get our position
	Vec2 ourUVs = vertices[indexInVertexArray].uvTexCoords;
	fvTexcOut[0] = ourUVs.x;
	fvTexcOut[1] = ourUVs.y;
}

void SetTangent( SMikkTSpaceContext const* pContext, float const fvTangent[], float const fSign, int const iFace, int const iVert )
{
	std::vector<Vertex_PCUTBN>& vertices = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;

	Vec3 normal = vertices[indexInVertexArray].normal;
	
	Vec3 tangent = Vec3( fvTangent[0], fvTangent[1], fvTangent[2] );
	Vec3 bitangent = CrossProduct( normal, tangent ) * fSign;


	vertices[indexInVertexArray].tangent = tangent;
	vertices[indexInVertexArray].bitangent = bitangent;
}

void GenerateTangentsForVertexArray( std::vector<Vertex_PCUTBN>& vertices )
{
	SMikkTSpaceInterface interface;

	//How MikkT gets info
	interface.m_getNumFaces = GetNumFaces;
	interface.m_getNumVerticesOfFace = GetNumberOfVerticesForFace;
	interface.m_getPosition = GetPositionForFaceVert;
	interface.m_getNormal = GetNormalForFaceVert;
	interface.m_getTexCoord = GetUVForFaceVert;

	//How Mikkt gives us info
	interface.m_setTSpaceBasic = SetTangent;
	interface.m_setTSpace = nullptr;

	SMikkTSpaceContext context;
	context.m_pInterface = &interface;
	context.m_pUserData = &vertices;

	//Run algorithm
	genTangSpaceDefault( &context );
}

void GenerateNormalsForVertexArray( std::vector<Vertex_PCUTBN>& vertices )
{
	size_t vertexCount = vertices.size();
	for( size_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex += 3 )
	{
		Vec3 pos0 = vertices[vertexIndex].position;
		Vec3 pos1 = vertices[vertexIndex + 1].position;
		Vec3 pos2 = vertices[vertexIndex + 2].position;

		Vec3 dir0 = pos1 - pos0;
		Vec3 dir1 = pos2 - pos1;

		Vec3 normal = CrossProduct( dir0, dir1 );

		vertices[vertexIndex].normal = normal;
		vertices[vertexIndex + 1].normal = normal;
		vertices[vertexIndex + 2].normal = normal;
	}
}

