#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/OBB2.hpp"




void Vertex_PCU::TransformVertexArray( int NUM_SHIP_VERTS, Vertex_PCU* shipVerts, float scale, float orientationDegrees, Vec2 position )
{
	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; vertIndex++ )
	{
		shipVerts[vertIndex].position =  TransformPosition3DXY( shipVerts[vertIndex].position, scale, orientationDegrees, position );
	}

}

void Vertex_PCU::TransformVertexArray( std::vector<Vertex_PCU*>& shipVerts, float scale, float orientationDegrees, Vec2 position )
{
	TransformVertexArray( static_cast<int>(shipVerts.size()), shipVerts[0], scale, orientationDegrees, position );
}

void Vertex_PCU::AppendVerts( std::vector<Vertex_PCU>& masterVertexList, std::vector<Vertex_PCU>& vertexesToAppend )
{
	for( int vertexIndex = 0; vertexIndex < vertexesToAppend.size(); vertexIndex++ )
	{
		masterVertexList.push_back( vertexesToAppend[vertexIndex] );
	}
}



void Vertex_PCU::AppendVertsCapsule2D( std::vector<Vertex_PCU>& masterVertexList, const Capsule2& capsule, const Rgba8& color )
{
	Vec2 center = capsule.GetCenter();
	Vec2 fullDimensions = Vec2( capsule.GetBoneLength(), capsule.radius*2.f );
	float orientationDegrees = (capsule.endPosition - capsule.startPosition).GetAngleDegrees();
	//TODO CHANGE
	OBB2 innerBox = OBB2( center, fullDimensions, orientationDegrees );

	Vertex_PCU::AppendVertsOBB2D( masterVertexList, innerBox, color );

	//Add semicircle at end of capsule
	float orientationMinus90Degrees = orientationDegrees - 90.f;
	float increment = 180.f/32.f;
	float currentOrientation = 0.f;
	for( currentOrientation = orientationMinus90Degrees; currentOrientation < orientationMinus90Degrees + 180.f; currentOrientation += increment )
	{
		Vec2 firstVertex = Vec2::MakeFromPolarDegrees( currentOrientation, capsule.radius ) + capsule.endPosition;
		Vec2 secondVertex = Vec2::MakeFromPolarDegrees( currentOrientation+increment, capsule.radius ) + capsule.endPosition;

		masterVertexList.push_back( Vertex_PCU( Vec3( capsule.endPosition ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( firstVertex ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( secondVertex ), color, Vec2( 0.f, 0.f ) ) );
	}

	//Add semicircle at start of capsule
	for( currentOrientation; currentOrientation < orientationMinus90Degrees + 360.f; currentOrientation += increment )
	{
		Vec2 firstVertex = Vec2::MakeFromPolarDegrees( currentOrientation, capsule.radius ) + capsule.startPosition;
		Vec2 secondVertex = Vec2::MakeFromPolarDegrees( currentOrientation+increment, capsule.radius ) + capsule.startPosition;

		masterVertexList.push_back( Vertex_PCU( Vec3( capsule.startPosition ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( firstVertex ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( secondVertex ), color, Vec2( 0.f, 0.f ) ) );
	}
}


void Vertex_PCU::AppendVertsAABB2D( std::vector<Vertex_PCU>& masterVertexList, const AABB2& aabb, const Rgba8& color, const AABB2& uvs /*= AABB2() */ )
{
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.mins.x, aabb.mins.y, 0.f ), color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.maxs.x, aabb.mins.y, 0.f ), color, Vec2( uvs.maxs.x, uvs.mins.y ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.maxs.x, aabb.maxs.y, 0.f ), color, uvs.maxs ) );

	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.mins.x, aabb.mins.y, 0.f ), color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.maxs.x, aabb.maxs.y, 0.f ), color, uvs.maxs ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.mins.x, aabb.maxs.y, 0.f ), color, Vec2( uvs.mins.x, uvs.maxs.y ) ) );
}


void Vertex_PCU::AppendVertsOBB2D( std::vector<Vertex_PCU>& masterVertexList, const OBB2& obb, const Rgba8& color, const AABB2& uvs /*= AABB2() */ )
{
	Vec2* obbCornerPositions = new Vec2[4];
	obb.GetCornerPositions( obbCornerPositions );

	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[0] ), color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[1] ), color, Vec2( uvs.maxs.x, uvs.mins.y ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[2] ), color, uvs.maxs ) );

	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[0] ), color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[2] ), color, uvs.maxs ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[3] ), color, Vec2( uvs.mins.x, uvs.maxs.y ) ) );
}


void Vertex_PCU::AppendVertsLine2D( std::vector<Vertex_PCU>& masterVertexList, const LineSegment2& line, float thickness, const Rgba8& color )
{
	Vec2 normalizedDisplacement = (line.endPosition - line.startPosition).GetNormalized();
	float radius = thickness * 0.5f;
	Vec2 fwd = normalizedDisplacement * radius;
	Vec2 leftVec = fwd.GetRotated90Degrees();
	Vec2 endLeft = line.endPosition + fwd + leftVec;
	Vec2 endRight= line.endPosition + fwd - leftVec;
	Vec2 startLeft = line.startPosition - fwd + leftVec;
	Vec2 startRight = line.startPosition - fwd - leftVec;

	masterVertexList.push_back( Vertex_PCU( Vec3( startRight ), color, Vec2( 0.f, 0.f ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( endRight ), color, Vec2( 0.f, 0.f ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( endLeft ), color, Vec2( 0.f, 0.f ) ) );

	masterVertexList.push_back( Vertex_PCU( Vec3( startLeft ), color, Vec2( 0.f, 0.f ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( startRight ), color, Vec2( 0.f, 0.f ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( endLeft ), color, Vec2( 0.f, 0.f ) ) );
}


void Vertex_PCU::AppendVertsDisc2D( std::vector<Vertex_PCU>& masterVertexList, const Vec2& discCenter, const float discRadius, const Rgba8& color )
{
	//float orientationMinus90Degrees = orientationDegrees - 90.f;
	float increment = 180.f/32.f;
	for( float currentOrientation = 0.f; currentOrientation < 360.f; currentOrientation += increment )
	{
		Vec2 firstVertex = Vec2::MakeFromPolarDegrees( currentOrientation, discRadius ) + discCenter;
		Vec2 secondVertex = Vec2::MakeFromPolarDegrees( currentOrientation+increment, discRadius ) + discCenter;

		masterVertexList.push_back( Vertex_PCU( Vec3( discCenter ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( firstVertex ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( secondVertex ), color, Vec2( 0.f, 0.f ) ) );
	}
}

Vertex_PCU::Vertex_PCU( const Vertex_PCU& copy )
	: position( copy.position )
	, tint( copy.tint )
	, uvTexCoords( copy.uvTexCoords )
{
}


Vertex_PCU::Vertex_PCU( const Vec3& initialPosition, const Rgba8& initialTint, const Vec2& initialuvTexCoords )
	: position( initialPosition )
	, tint( initialTint )
	, uvTexCoords( initialuvTexCoords )
{
}
