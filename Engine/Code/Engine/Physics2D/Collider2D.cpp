#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Core/EngineCommon.hpp"


typedef bool (*collision_check_cb)( Collider2D const*, Collider2D const* );

static bool DiscVDiscCollisionCheck( Collider2D const* col0, Collider2D const* col1 );
static bool DiscVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 );
static bool PolygonVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 );

static collision_check_cb gCollisionChecks[NUM_COLLIDER_TYPE * NUM_COLLIDER_TYPE] ={
	/*             disc,                         polygon, */
	/*    disc */  DiscVDiscCollisionCheck,      nullptr,
	/* polygon */  DiscVPolygonCollisionCheck,   PolygonVPolygonCollisionCheck
};

bool Collider2D::Intersects( Collider2D const* other ) const
{
	eCollider2DType myType = m_type;
	eCollider2DType otherType = other->m_type;

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

	bool isColliding = DoPolygonAndDiscOverlap2D( polygon, discCenter, discRadius );

	return isColliding;
}


static bool PolygonVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 )
{
	UNUSED( col0 );
	UNUSED( col1 );



	return false;
}