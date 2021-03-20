#pragma once

#include "Engine/Math/Vec2.hpp"
#include <vector>


struct AABB2
{
public:
	Vec2 mins;
	Vec2 maxs;

	AABB2 GetTranslated( const Vec2& translateVector ) const;
public:
	~AABB2() {}
	AABB2() {}
	AABB2(const Vec2& copyBottomLeft, const Vec2& copyTopRight);
	AABB2( float minX, float minY, float maxX, float maxY);

	void Translate(const Vec2& translator);
	bool IsPointInside(const Vec2& point) const;
	void StretchToIncludePoint(const Vec2& point);
	const Vec2 GetPointAtUV(const Vec2& UV) const;
	const Vec2 GetUVForPoint(const Vec2& point) const;
	const Vec2 GetCenter() const;
	const Vec2 GetDimensions() const;
	const Vec2 GetNearestPoint(const Vec2& point) const;

	void SetCenter(const Vec2& newCenter);
	void SetDimensions(const Vec2& newDimensions);
	void ScaleDimensionsNonUniform( Vec2 const& newScale );
	void ScaleDimensionsUniform( float newScale );
	void MakeSquareFromBottomLeft( bool useHeight = false );

	AABB2 CarveBoxOffLeft(float FractionOfWidth, float additionalWidth = 0.f);
	AABB2 CarveBoxOffRight(float FractionOfWidth, float additionalWidth = 0.f);
	AABB2 CarveBoxOffTop(float FractionOfWidth, float additionalWidth = 0.f);
	AABB2 CarveBoxOffBottom(float FractionOfWidth, float additionalWidth = 0.f);

	AABB2 GetBoxAtLeft(float FractionOfWidth, float additionalWidth = 0.f) const;
	AABB2 GetBoxAtRight(float FractionOfWidth, float additionalWidth = 0.f) const;
	AABB2 GetBoxAtTop(float FractionOfWidth, float additionalWidth = 0.f) const;
	AABB2 GetBoxAtBottom(float FractionOfWidth, float additionalWidth = 0.f) const;

	AABB2 GetInnerBoxWithAlignment( Vec2 const& uvPosition, Vec2 const& offset, Vec2 const& pivot, Vec2 const& innerBoxPercentDimensions ) const;
	AABB2 GetInnerBoxWithAlignment( Vec2 const& uvPosition, Vec2 const& offset, Vec2 const& pivot, AABB2 const& innerBox ) const;


	std::vector<AABB2> GetBoxAsRows( int numberOfRows );
	std::vector<AABB2> GetBoxAsColumns( int numberOfColumns );

	AABB2 GetGridBoxFromGridDimensions( IntVec2 const& gridDimensions, IntVec2 const& gridIndex );


	float GetOuterRadius() const;
	float GetInnerRadius() const;
	void GetCornerPositions( Vec2* out_fourPoints ) const; //place 4 corner positions into this float[4] array


	void SetFromText( const char* attribute );
};