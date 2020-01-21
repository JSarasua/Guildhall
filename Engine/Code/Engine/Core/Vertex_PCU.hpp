#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>
#pragma once

struct Capsule2;
struct AABB2;
struct OBB2;
struct LineSegment2;
struct Rgba8;


//-----------------------------------------------------------------------------------------------
struct Vertex_PCU
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	Vec3 position;
	Rgba8 tint;
	Vec2 uvTexCoords;

	static void TransformVertexArray(int NUM_SHIP_VERTS, Vertex_PCU* shipVerts, float scale, float orientationDegrees, Vec2 position);
	static void TransformVertexArray(std::vector<Vertex_PCU*>& shipVerts, float scale, float orientationDegrees, Vec2 position);

	static void AppendVerts( std::vector<Vertex_PCU>& masterVertexList, std::vector<Vertex_PCU>& vertexesToAppend);
	static void AppendVertsCapsule2D( std::vector<Vertex_PCU>& masterVertexList, const Capsule2& capsule, const Rgba8& color);
	static void AppendVertsAABB2D( std::vector<Vertex_PCU>& masterVertexList, const AABB2& aabb, const Rgba8& color, const AABB2& uvs = AABB2() );
	static void AppendVertsOBB2D( std::vector<Vertex_PCU>& masterVertexList, const OBB2& obb, const Rgba8& color, const AABB2& uvs = AABB2() );
	static void AppendVertsLine2D( std::vector<Vertex_PCU>& masterVertexList, const LineSegment2& line, float thickness, const Rgba8& color );
	static void AppendVertsDisc2D( std::vector<Vertex_PCU>& masterVertexList, const Vec2& discCenter, const float discRadius, const Rgba8& color );

public:
	// Construction/Destruction
	~Vertex_PCU() {}												// destructor (do nothing)
	Vertex_PCU() {}
	Vertex_PCU( const Vertex_PCU& copyFrom );							// copy constructor (from another Vertex_PCU)
	explicit Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTextCoords );
	//TODO ADD DEFAULT UV CONSTRUCTOR
	//TODO ADD VEC2 CONSTRUCTOR

};
