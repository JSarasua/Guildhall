#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/vec2.hpp"

struct BufferAttribute;

struct Vertex_PCUTBN
{
public:
	Vec3 position;
	Rgba8 tint;
	Vec2 uvTexCoords;

	// Vec3 tangent;
	// Vec3 bitangent;
	Vec3 normal;

	static BufferAttribute const LAYOUT[];

public:
	~Vertex_PCUTBN() {}
	Vertex_PCUTBN() {}
	Vertex_PCUTBN( Vertex_PCUTBN const& copyFrom );
	explicit Vertex_PCUTBN( Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords, Vec3 const& normal );
};