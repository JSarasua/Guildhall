#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <vector>

struct BufferAttribute;

struct Vertex_PCUTBN
{
public:
	Vec3 position;
	Rgba8 tint;
	Vec2 uvTexCoords;

	Vec3 tangent;
	Vec3 bitangent;
	Vec3 normal;

	static BufferAttribute const LAYOUT[];

public:
	static void AppendIndexedVertsCube( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList, float cubeHalfHeight = 1.f, Vec3 nonUniformScale = Vec3( 1.f, 1.f, 1.f ) );
	static void AppendIndexedVertsSphere( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList, float sphereRadius = 1.f );
	static void AppendVerts4Points( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList, Vec3 const& p0, Vec3 const& p1, Vec3 const& p2, Vec3 const& p3, Rgba8 const& color, AABB2 const& uvs = AABB2() );

public:
	~Vertex_PCUTBN() {}
	Vertex_PCUTBN() {}
	Vertex_PCUTBN( Vertex_PCUTBN const& copyFrom );
	explicit Vertex_PCUTBN( Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords, Vec3 const& normal );
	explicit Vertex_PCUTBN( Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords, Vec3 const& normal, Vec3 const& tangent, Vec3 const& bitangent );
};