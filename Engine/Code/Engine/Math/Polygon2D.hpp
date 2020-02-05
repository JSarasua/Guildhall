#pragma once
#include "Engine/Math/vec2.hpp"
#include <vector>

class Polygon2D
{
public:
	Polygon2D( Vec2 const* points, unsigned int pointCount );



	bool IsValid() const; //must have at least 3 points to be considered a polygon

	bool IsConvex() const;
	bool Contains( Vec2 const& point ) const;
	float GetDistance( Vec2 const& point ) const;
	Vec2 GetClosestPoint( Vec2 const& point ) const;

	//accessors
	int GetVertexCount() const;
	int GetEdgeCount() const;
	void GetEdge( Vec2* outStart, Vec2* outEnd, size_t edgeIndex ) const;
	void GetEdgeNormal( Vec2* edgeNormal, size_t edgeIndex ) const;			//Pointing inwards


public:
	static Polygon2D MakeFromLineLoop( Vec2 const* points, unsigned int pointCount );

	static Polygon2D MakeConvexFromPointCloud( Vec2 const* points, unsigned int pointCount );

private:


private:
	std::vector<Vec2> m_points;
};