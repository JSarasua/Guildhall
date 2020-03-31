#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>

struct LineSegment2;

class Polygon2D
{
public:
	Polygon2D() {}
	Polygon2D( Vec2 const* points, unsigned int pointCount );
	Polygon2D( std::vector<Vec2> points );


	bool IsValid() const; //must have at least 3 points to be considered a polygon

	bool IsConvex() const;
	bool Contains( Vec2 const& point ) const;

	//accessors
	float GetArea() const;
	float GetDistance( Vec2 const& point ) const;
	Vec2 GetClosestPoint( Vec2 const& point ) const;
	Vec2 GetCenterOfMass() const;
	int GetVertexCount() const;
	int GetEdgeCount() const;
	void GetEdge( Vec2* outStart, Vec2* outEnd, size_t edgeIndex ) const;
	void GetClosestEdge( LineSegment2* edge, Vec2 const& point, size_t* edgeIndex );
	void GetEdgeNormal( Vec2* edgeNormal, size_t edgeIndex ) const;			//Pointing inwards
	void GetEdgeNormalOutward( Vec2* edgeNormal, size_t edgeIndex ) const;
	void GetTriangle( Vec2* outA, Vec2* outB, Vec2* outC, size_t triangleIndex ) const; //Gets a triangle for rendering from the triangle index
	float GetAreaOfTriangle( size_t triangleIndex ) const;
	Vec2 GetCenterOfTriangle( size_t triangleIndex ) const;
	size_t GetTriangleCount() const;	//Gets number of triangles polygon is made of
	AABB2 GetTightlyFixBox() const;

	Vec2 GetFurthestPointInDirection( Vec2 const& direction ) const; //Farthest point in a direction
	Vec2 GetGJKSupport( Polygon2D const& otherPoly, Vec2 const& dir ) const;

	void Translate( Vec2 const& translator );
	void RotateAroundCenter( float rotationRadians );

public:
	static Polygon2D MakeFromLineLoop( Vec2 const* points, unsigned int pointCount );
	static Polygon2D MakeConvexFromPointCloud( Vec2 const* points, unsigned int pointCount );
	static void CreateInitialGJKSimplex( Polygon2D const& poly0, Polygon2D const& poly1,  Polygon2D* simplex );
	static bool EvolveGJK( Polygon2D const& poly0, Polygon2D const& poly1, Polygon2D* simplex ); //Returns true if it successfully evolved the simplex, else returns false
	static bool GetGJKContainingSimplex( Polygon2D const& poly0, Polygon2D const& poly1, Polygon2D* containingSimplex ); //Returns the simplex that contains the origin of the minkowski space, will return false if can't
	static bool ExpandPenetration( Polygon2D const& poly0, Polygon2D const& poly1, Polygon2D* simplex );
	static void GetRangeNearInfiniteLine( LineSegment2* edge, Vec2 const& ref0, Vec2 const& ref1, Polygon2D const& poly, float epsilon = 0.01f ); //Gets min/max points on edge within an epsilon
	static void GetGJKContactEdgeFromPoly( LineSegment2* contactEdge, LineSegment2 const& refEdge, Vec2 const& normal, Polygon2D const& polyToClip );
	static bool ClipSegmentToSegment( LineSegment2& toClip, LineSegment2 const& refEdge );
private:


private:
	std::vector<Vec2> m_points;
};