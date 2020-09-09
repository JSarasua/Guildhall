#pragma once
#include "Engine/Math/vec2.hpp"

struct OBB2;

struct Capsule2
{
public:
	Vec2 startPosition = Vec2(0.f, 0.f);
	Vec2 endPosition = Vec2(0.f, 0.f);
	float radius = 0.f;

public:
	//Construction/Destruction
	~Capsule2() {}
	Capsule2() {}
	Capsule2( const Capsule2& copyFrom );
	Capsule2( const Vec2& initialStart, const Vec2& initialEnd, float initialRadius );

	Vec2	GetCenter() const;
	float	GetBoneLength() const;


	Vec2	GetNearestPoint( const Vec2& point ) const;
	void	GetCornerPositions( Vec2* verticePositions ) const;
	void	GetCapsuleBasisVectors( Vec2* capsuleBasisVectors ) const;
	int		GetNumberOfVertices() const;
	OBB2	GetTightlyFitOBB2() const;
	OBB2	GetInnerBox() const;
	Vec2	GetIBasisNormal() const;
	bool	IsPointInside( const Vec2& point ) const;
};

