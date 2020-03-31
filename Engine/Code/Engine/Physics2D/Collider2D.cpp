#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Core/EngineCommon.hpp"


typedef bool (*collision_check_cb)( Collider2D const*, Collider2D const* );
typedef bool (*manifold_cb)( Collider2D const*, Collider2D const*, Manifold2D* );

static bool DiscVDiscCollisionCheck( Collider2D const* col0, Collider2D const* col1 );
static bool DiscVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 );
static bool PolygonVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 );
static bool DiscVDiscManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold );
static bool DiscVPolygonManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold );
static bool PolygonVPolygonManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold );

static collision_check_cb gCollisionChecks[NUM_COLLIDER_TYPE * NUM_COLLIDER_TYPE] ={
	/*             disc,                         polygon, */
	/*    disc */  DiscVDiscCollisionCheck,      nullptr,
	/* polygon */  DiscVPolygonCollisionCheck,   PolygonVPolygonCollisionCheck
};

static manifold_cb gManifoldChecks[NUM_COLLIDER_TYPE * NUM_COLLIDER_TYPE] ={
	/*             disc,                         polygon, */
	/*    disc */  DiscVDiscManifold,      nullptr,
	/* polygon */  DiscVPolygonManifold,   PolygonVPolygonManifold
};

bool Collider2D::GetManifold( Collider2D const* other, Manifold2D* manifold ) const
{
	eCollider2DType myType = m_type;
	eCollider2DType otherType = other->m_type;

	AABB2 const& myBounds = GetBounds();
	AABB2 const& theirBounds = other->GetBounds();
	if( !DoAABBsOverlap2D( myBounds, theirBounds ) )
	{
		return false;
	}

	if( myType <= otherType ) {
		int idx = otherType * NUM_COLLIDER_TYPE + myType;
		manifold_cb check = gManifoldChecks[idx];
		return check( this, other, manifold );
	}
	else {
		// flip the types when looking into the index.
		int idx = myType * NUM_COLLIDER_TYPE + otherType;
		manifold_cb check = gManifoldChecks[idx];
		return check( other, this, manifold );
	}
}

bool Collider2D::Intersects( Collider2D const* other ) const
{
	eCollider2DType myType = m_type;
	eCollider2DType otherType = other->m_type;

	AABB2 myBounds = GetBounds();
	AABB2 theirBounds = other->GetBounds();
	if( !DoAABBsOverlap2D( myBounds, theirBounds ) )
	{
		return false;
	}

	if( myType <= otherType ) {
		int idx = otherType * NUM_COLLIDER_TYPE + myType;
		collision_check_cb check = gCollisionChecks[idx];
		return check( this, other );
	}
	else {
		// flip the types when looking into the index.
		int idx = myType * NUM_COLLIDER_TYPE + otherType;
		collision_check_cb check = gCollisionChecks[idx];
		return check( other, this );
	}
}

float Collider2D::GetMass() const
{
	if( nullptr == m_rigidbody )
	{
		return 1.f;
	}
	else
	{
		return m_rigidbody->GetMass();
	}
}

eSimulationMode Collider2D::GetSimulationMode() const
{
	return m_rigidbody->GetSimulationMode();
}

PhysicsMaterial Collider2D::GetPhysicsMaterial() const
{
	return m_physicsMaterial;
}

float Collider2D::GetFriction() const
{
	return m_physicsMaterial.m_friction;
}

float Collider2D::GetResitution() const
{
	return m_physicsMaterial.m_restitution;
}

float Collider2D::GetBounceWith( Collider2D const* other ) const
{
	float myRestitution = m_physicsMaterial.m_restitution;
	float theirResitution = other->m_physicsMaterial.m_restitution;

	//Calculating Restitution by the produce. May want to change to allow for Min, Max, etc.
	return myRestitution * theirResitution;
}

float Collider2D::GetFrictionWith( Collider2D const* other ) const
{
	float myfriction = m_physicsMaterial.m_friction;
	float theirFriction = other->m_physicsMaterial.m_friction;

	return myfriction * theirFriction;
}

void Collider2D::SetRestitution( float newRestitution )
{
	float restitution = Clampf( newRestitution, 0.f, 1.f );
	m_physicsMaterial.m_restitution = restitution;
}

void Collider2D::SetFriction( float newFriction )
{
	m_physicsMaterial.m_friction = newFriction;
}

Collider2D::Collider2D( float restitution /*= 1.f */ )
{
	m_physicsMaterial.m_restitution = restitution;
}

AABB2 Collider2D::GetBounds() const
{
	return m_bounds;
}

void Collider2D::Move( Vec2 const& translator )
{
	if( nullptr == m_rigidbody )
	{
		return;
	}

	m_rigidbody->Translate( translator );
}

Collider2D::~Collider2D()
{
	
}

static bool DiscVDiscCollisionCheck( Collider2D const* col0, Collider2D const* col1 )
{
	DiscCollider2D const* disc0 = (DiscCollider2D const*)col0;
	DiscCollider2D const* disc1 = (DiscCollider2D const*)col1;

	Vec2 const& disc0Center = disc0->m_worldPosition + disc0->m_localPosition;
	float disc0Radius = disc0->m_radius;
	Vec2 const& disc1Center = disc1->m_worldPosition + disc1->m_localPosition;
	float disc1Radius = disc1->m_radius;

	bool isColliding = DoDiscsOverlap( disc0Center, disc0Radius, disc1Center, disc1Radius );

	return isColliding;
}

static bool DiscVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 )
{
	DiscCollider2D const* disc = (DiscCollider2D const*)col0;
	PolygonCollider2D const* poly = (PolygonCollider2D const*)col1;

	Vec2 const& discCenter = disc->m_worldPosition + disc->m_localPosition;
	float discRadius = disc->m_radius;
	Polygon2D const& polygon = poly->m_polygon;

	if( DoDiscAndAABBOverlap2D( discCenter, discRadius, poly->GetBounds() ) )
	{
		bool isColliding = DoPolygonAndDiscOverlap2D( polygon, discCenter, discRadius );
		return isColliding;
	}
	else
	{
		return false;
	}

}


static bool PolygonVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 )
{
	Vec2 rightDir = Vec2( 1.f , 0.f );
	Vec2 leftDir = Vec2( -1.f, 0.f );
	Vec2 origin = Vec2( 0.f, 0.f );

	PolygonCollider2D const* polyCol0 = (PolygonCollider2D const*)col0;
	PolygonCollider2D const* polyCol1 = (PolygonCollider2D const*)col1;

	Polygon2D const& poly0 = polyCol0->GetPolygon();
	Polygon2D const& poly1 = polyCol1->GetPolygon();


	//Create First Simplex
	Polygon2D simplex;
	Polygon2D::CreateInitialGJKSimplex( poly0, poly1,  &simplex );

	if( simplex.Contains( origin ) )
	{
		return true;
	}

	while( Polygon2D::EvolveNormalDir( poly0, poly1, &simplex ) )
	{
		if( simplex.Contains( origin ) )
		{
			return true;
		}
	}

	return false;

	//EvolveNormalDir
	//Loop
		//Call CheckInside

		//Call Evolve



// 	Vec2 C = poly0.GetGJKSupport( poly1, rightDir ); //
// 	//Vec2 origin = Vec2( 0.f, 0.f );
// 	Vec2 CO = C - origin; // CO = -OC
// 
// 	Vec2 B = poly0.GetGJKSupport( poly1, -CO );
// 	Vec2 BO = B - origin;
// 
// 	if( DotProduct2D( CO, -BO ) < 0.f )
// 	{
// 		return false;
// 	}
// 
// 	Vec2 CB = C - B;
// 
// 	//Do Triple cross product expansion
// 	Vec2 normalDir = Vec2( CrossProduct( CB, CrossProduct( CO, CB ) ) );
// 	Vec2 A = poly0.GetGJKSupport( poly1, normalDir );
// 
// 
// 	std::vector<Vec2> vertexes;
// 	vertexes.push_back( A );
// 	vertexes.push_back( B );
// 	vertexes.push_back( C );
// 	Polygon2D GJKTriangle = Polygon2D( vertexes );
// 
// 	bool isTriangleFlipped = false;
// 
// 	if( !GJKTriangle.IsConvex() )
// 	{
// 		isTriangleFlipped = true;
// 		vertexes.clear();
// 		vertexes.push_back( C );
// 		vertexes.push_back( B );
// 		vertexes.push_back( A );
// 		GJKTriangle = Polygon2D( vertexes );
// 		
// 		if( !GJKTriangle.IsConvex() )
// 		{
// 			//Should not occur
// 			return false;
// 		}
// 	}
// 
// 	if( GJKTriangle.Contains( origin ) )
// 	{
// 		return true;
// 	}
// 
// 	size_t edgeIndex = 0;
// 	while( edgeIndex < 3 ) //always triangles
// 	{
// 
// 		Vec2 D;
// 		Vec2 E;
// 		Vec2 edgeNormal;
// 		GJKTriangle.GetEdge( &D, &E, edgeIndex );
// 		GJKTriangle.GetEdgeNormal( &edgeNormal, edgeIndex );
// 		edgeNormal *= -1.f;
// 
// 		Vec2 OD = origin - D;
// 
// 		float ODdotNormal = DotProduct2D( OD, edgeNormal );
// 		if( ODdotNormal > 0.f )
// 		{
// 
// 			C = D;
// 			B = E;
// 			
// 			CO = C - origin; // CO = -OC
// 			CB = C - B;
// 
// 			Vec2 tempNormalDir = Vec2( CrossProduct( CB, CrossProduct( CO, CB ) ) );
// 			Vec2 F = poly0.GetGJKSupport( poly1, tempNormalDir );
// 
// 
// 			if( F == C || F == B )
// 			{
// 				//The normal points outside of the polygon
// 				return false;
// 			}
// 			A = F;
// 
// 			break;
// 		}
// 
// 
// 	}
// 	
// 
// 	//Check if inside
// 	
// 	//Create Simplex in counterclockwise winding order
// 	//Add GetEdgeNormalInward and GetEdgeNormalOutward
// 	//Add a flip poly points if clockwise
// 	//Add a make next simplex method given current simplex poly, poly0, and poly1
// 
// 
// 	
// 
// 
// 	return false;
}


bool DiscVDiscManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold )
{
	DiscCollider2D const* disc0 = (DiscCollider2D const*)col0;
	DiscCollider2D const* disc1 = (DiscCollider2D const*)col1;

	Vec2 const& disc0Center = disc0->m_worldPosition + disc0->m_localPosition;
	float disc0Radius = disc0->m_radius;
	Vec2 const& disc1Center = disc1->m_worldPosition + disc1->m_localPosition;
	float disc1Radius = disc1->m_radius;

	Vec2 normal = disc0Center - disc1Center;
	float penetration = (disc0Radius + disc1Radius) - GetDistance2D(disc0Center, disc1Center);

	if( penetration < 0.f )
	{
		return false;
	}
	
	normal.Normalize();
	Vec2 contact = disc0Center - normal*(disc0Radius - 0.5f*penetration);

	manifold->normal = normal;
	manifold->penetration = penetration;
	manifold->contactPoint = contact;

	return manifold;
}
bool DiscVPolygonManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold )
{
	DiscCollider2D const* disc = (DiscCollider2D const*)col0;
	PolygonCollider2D const* poly = (PolygonCollider2D const*)col1;

	Vec2 const& discCenter = disc->m_worldPosition + disc->m_localPosition;
	float discRadius = disc->m_radius;
	Polygon2D const& polygon = poly->m_polygon;

	Vec2 closestPoint = polygon.GetClosestPoint( discCenter );
	Vec2 normal = discCenter - closestPoint;
	normal.Normalize();
	float distanceToCollision = GetDistance2D(discCenter, closestPoint);
	float penetration = discRadius - distanceToCollision;

	if( penetration < 0.f )
	{
		return false;
	}

	if( polygon.Contains( discCenter ) )
	{
		normal *= -1.f;
		penetration = discRadius + distanceToCollision;
	}
	Vec2 contactPoint = discCenter - normal*( discRadius - 0.5f*penetration );


	manifold->contactPoint = contactPoint;
	manifold->normal = normal;
	manifold->penetration = penetration;

	return true;
	//TODO if on edge of polygon
}
bool PolygonVPolygonManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold )
{
	UNUSED(col0);
	UNUSED(col1);
	UNUSED(manifold);

	return false;
}
