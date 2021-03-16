#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vector>
#include <string>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"



AABB2 AABB2::GetTranslated( const Vec2& translateVector ) const
{
	AABB2 translatedAABB2(mins, maxs);
	translatedAABB2.Translate(translateVector);
	return translatedAABB2;
}

AABB2::AABB2( const Vec2& copyBottomLeft, const Vec2& copyTopRight )
{
	mins = copyBottomLeft;
	maxs = copyTopRight;
}

AABB2::AABB2( float minX, float minY, float maxX, float maxY ) :
	mins(minX, minY),
	maxs(maxX, maxY)
{
	
}

void AABB2::Translate( const Vec2& translator )
{
	mins += translator;
	maxs += translator;
}

bool AABB2::IsPointInside( const Vec2& point ) const
{
	if( point.x > mins.x && point.x < maxs.x && point.y > mins.y && point.y < maxs.y )
	{
		return true;
	}
	else
	{
		return false;
	}


}

void AABB2::StretchToIncludePoint( const Vec2& point )
{
	if( !IsPointInside( point ) )
	{
		if( point.x < mins.x )
		{
			mins.x = point.x;
		}
		else if( point.x > maxs.x )
		{
			maxs.x = point.x;
		}

		if( point.y < mins.y )
		{
			mins.y = point.y;
		}
		else if( point.y > maxs.y )
		{
			maxs.y = point.y;
		}

	}
}

const Vec2 AABB2::GetPointAtUV( const Vec2& UV ) const
{
	float PointX = RangeMap(0.f,1.f,mins.x, maxs.x, UV.x);
	float PointY = RangeMap(0.f,1.f,mins.y, maxs.y, UV.y);
	
	return Vec2(PointX, PointY);
}

const Vec2 AABB2::GetUVForPoint( const Vec2& point ) const
{
	float UVX = RangeMap( mins.x, maxs.x, 0.f, 1.f, point.x );
	float UVY = RangeMap( mins.y, maxs.y, 0.f, 1.f, point.y );

	return Vec2( UVX, UVY );
}

const Vec2 AABB2::GetCenter() const
{
	return GetPointAtUV(Vec2(0.5f, 0.5f));
}

const Vec2 AABB2::GetDimensions() const
{
	return maxs - mins;
}

const Vec2 AABB2::GetNearestPoint(const Vec2& point) const
{
	float nearestX = Clampf( point.x, mins.x, maxs.x );
	float nearestY = Clampf( point.y, mins.y, maxs.y );

	return Vec2( nearestX, nearestY );
}

void AABB2::SetCenter( const Vec2& newCenter )
{
	Vec2 displacement = newCenter - GetCenter();
	Translate(displacement);
}

void AABB2::SetDimensions( const Vec2& newDimensions )
{
	Vec2 currentCenter = GetCenter();
	mins = Vec2(0.f, 0.f);
	maxs = newDimensions;
	SetCenter(currentCenter);
}

void AABB2::ScaleDimensionsNonUniform( Vec2 const& newScale )
{
	Vec2 currentDimensions = GetDimensions();
	Vec2 newDimensions = currentDimensions * newScale;
	SetDimensions( newDimensions );
}

void AABB2::ScaleDimensionsUniform( float newScale )
{
	Vec2 currentDimensions = GetDimensions();
	Vec2 newDimensions = currentDimensions * newScale;
	SetDimensions( newDimensions );
}

void AABB2::MakeSquareFromBottomLeft( bool useHeight /*= false */ )
{
	float displacementToUse = 0.f;

	if( useHeight )
	{
		displacementToUse = maxs.y - mins.y;
		maxs.x = mins.x + displacementToUse;
	}
	else
	{
		displacementToUse = maxs.x - mins.x;
		maxs.y = mins.y + displacementToUse;
	}
}

AABB2 AABB2::CarveBoxOffLeft( float FractionOfWidth, float additionalWidth /*= 0.f*/ )
{
	AABB2 newAABB = *this;
	float newXDistance = (maxs.x - mins.x)*FractionOfWidth + additionalWidth;
	newAABB.maxs.x = newXDistance + mins.x;

	mins.x += newXDistance;


	return newAABB;
}

AABB2 AABB2::CarveBoxOffRight( float FractionOfWidth, float additionalWidth /*= 0.f*/ )
{
	AABB2 newAABB = *this;
	float newXDistance = (maxs.x - mins.x)*FractionOfWidth + additionalWidth;
	newAABB.mins.x = maxs.x - newXDistance;


	maxs.x -= newXDistance;

	return newAABB;
}

AABB2 AABB2::CarveBoxOffTop( float FractionOfWidth, float additionalWidth /*= 0.f*/ )
{
	AABB2 newAABB = *this;
	float newYDistance = (maxs.y - mins.y)*FractionOfWidth + additionalWidth;
	newAABB.mins.y = maxs.y - newYDistance;


	maxs.y -= newYDistance;

	return newAABB;
}

AABB2 AABB2::CarveBoxOffBottom( float FractionOfWidth, float additionalWidth /*= 0.f*/ )
{
	AABB2 newAABB = *this;
	float newYDistance = (maxs.y - mins.y)*FractionOfWidth + additionalWidth;
	newAABB.maxs.y = mins.y + newYDistance;


	mins.y += newYDistance;

	return newAABB;
}

AABB2 AABB2::GetBoxAtLeft( float FractionOfWidth, float additionalWidth /*= 0.f*/ )
{
	AABB2 newAABB = *this;
	float newXDistance = (maxs.x - mins.x)*FractionOfWidth + additionalWidth;
	newAABB.maxs.x = newXDistance + mins.x;


	return newAABB;
}

AABB2 AABB2::GetBoxAtRight( float FractionOfWidth, float additionalWidth /*= 0.f*/ )
{
	AABB2 newAABB = *this;
	float newXDistance = (maxs.x - mins.x)*FractionOfWidth + additionalWidth;
	newAABB.mins.x = maxs.x - newXDistance;


	return newAABB;
}

AABB2 AABB2::GetBoxAtTop( float FractionOfWidth, float additionalWidth /*= 0.f*/ )
{
	AABB2 newAABB = *this;
	float newYDistance = (maxs.y - mins.y)*FractionOfWidth + additionalWidth;
	newAABB.mins.y = maxs.y - newYDistance;


	return newAABB;
}

AABB2 AABB2::GetBoxAtBottom( float FractionOfWidth, float additionalWidth /*= 0.f*/ )
{
	AABB2 newAABB = *this;
	float newYDistance = (maxs.y - mins.y)*FractionOfWidth + additionalWidth;
	newAABB.maxs.y = mins.y + newYDistance;


	return newAABB;
}

AABB2 AABB2::GetInnerBoxWithAlignment( Vec2 const& uvPosition, Vec2 const& offset, Vec2 const& pivot, Vec2 const& innerBoxPercentDimensions )
{
	Vec2 myDimensions = GetDimensions();

	Vec2 dimensions = myDimensions * innerBoxPercentDimensions;
	Vec2 halfDimensions = 0.5f * dimensions;

	//Get pivot as a value from -1 to 1 then multiply with half dimension to get actual offset
	Vec2 pivotOffset = 2.f * (pivot - Vec2( 0.5f, 0.5f )) * halfDimensions;

	Vec2 newCenter = GetPointAtUV( uvPosition ) + offset;
	newCenter -= pivotOffset;

	AABB2 newInnerBox;
	newInnerBox.SetDimensions( dimensions );
	newInnerBox.SetCenter( newCenter );

	return newInnerBox;
}

AABB2 AABB2::GetInnerBoxWithAlignment( Vec2 const& uvPosition, Vec2 const& offset, Vec2 const& pivot, AABB2 const& innerBox )
{
	Vec2 dimensions = innerBox.GetDimensions();
	Vec2 innerCenter = innerBox.GetCenter();
	Vec2 innerPivot = innerBox.GetPointAtUV( pivot );
	Vec2 pivotOffset = innerPivot - innerCenter;

	Vec2 newCenter = GetPointAtUV( uvPosition ) + offset;
	newCenter -= pivotOffset;

	AABB2 newInnerBox = innerBox;
	newInnerBox.SetCenter( newCenter );

	return newInnerBox;
}

std::vector<AABB2> AABB2::GetBoxAsRows( int numberOfRows )
{
	std::vector<AABB2> rows;
	if( numberOfRows == 1 )
	{
		rows.push_back( *this );
		return rows;
	}

	float minY = mins.y;
	float maxY = maxs.y;
	float heightOfRow = (maxY - minY) / (float)numberOfRows;
	for( int rowIndex = 1; rowIndex < numberOfRows; rowIndex++ )
	{
		float bottomOfRowY = minY + (heightOfRow * (float)(rowIndex - 1) );
		float topOfRowY = minY + (heightOfRow * (float)(rowIndex) );
		AABB2 row = AABB2( mins.x, bottomOfRowY, maxs.x, topOfRowY );
		rows.push_back( row );

		if( rowIndex + 1 == numberOfRows )
		{
			bottomOfRowY = minY + (heightOfRow * (float)(rowIndex));
			topOfRowY = minY + (heightOfRow * (float)(rowIndex + 1));
			AABB2 lastRow = AABB2( mins.x, bottomOfRowY, maxs.x, topOfRowY );
			rows.push_back( lastRow );
		}
	}

	return rows;
}

std::vector<AABB2> AABB2::GetBoxAsColumns( int numberOfColumns )
{
	std::vector<AABB2> columns;
	if( numberOfColumns == 1 )
	{
		columns.push_back( *this );
		return columns;
	}

	float minX = mins.x;
	float maxX = maxs.x;
	float width = (maxX - minX) / (float)numberOfColumns;
	for( int columnIndex = 1; columnIndex < numberOfColumns; columnIndex++ )
	{
		float leftOfColumnX = minX + (width * (float)(columnIndex - 1));
		float rightOfColumnX = minX + (width * (float)(columnIndex));
		AABB2 column = AABB2( leftOfColumnX, mins.y, rightOfColumnX, maxs.y );
		columns.push_back( column );

		if( columnIndex + 1 == numberOfColumns )
		{
			leftOfColumnX = minX + (width * (float)(columnIndex));
			rightOfColumnX = minX + (width * (float)(columnIndex + 1));
			AABB2 lastColumn = AABB2( leftOfColumnX, mins.y, rightOfColumnX, maxs.y );
			columns.push_back( lastColumn );
		}
	}

	return columns;
}

AABB2 AABB2::GetGridBoxFromGridDimensions( IntVec2 const& gridDimensions, IntVec2 const& gridIndex )
{
	std::vector<AABB2> columns = GetBoxAsColumns( gridDimensions.x );
	std::vector<AABB2> rows = GetBoxAsRows( gridDimensions.y );

	GUARANTEE_OR_DIE( columns.size() > gridIndex.x && rows.size() > gridIndex.y, "Grid indices don't match grid dimensions")

	AABB2 column = columns[gridIndex.x];
	AABB2 row = rows[gridIndex.y];

	AABB2 gridBox = AABB2( column.mins.x, row.mins.y, column.maxs.x, row.maxs.y );

	return gridBox;
}

float AABB2::GetOuterRadius() const
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;

	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;

	float outerRadius = halfWidth + halfHeight;

	return outerRadius;
}

float AABB2::GetInnerRadius() const
{
	float halfWidth = maxs.x - mins.x;
	float halfHeight = maxs.y - mins.y;

	float innerRadius = Min(halfHeight,halfWidth);
	return innerRadius;
}

void AABB2::GetCornerPositions( Vec2* out_fourPoints ) const
{
	out_fourPoints[0] = mins;
	out_fourPoints[1] = Vec2(maxs.x,mins.y);
	out_fourPoints[2] = maxs;
	out_fourPoints[3] = Vec2(mins.x,maxs.y);
}

void AABB2::SetFromText( const char* attribute )
{
	std::string strText = attribute;

	std::vector<std::string> splitStrings = SplitStringOnDelimeter( attribute, ',' );
	GUARANTEE_OR_DIE( splitStrings.size() == 4, Stringf( "Vec2 can't construct from improper string \"%s\"", attribute ) );

	mins.x = (float)atof( splitStrings[0].c_str() );
	mins.y = (float)atof( splitStrings[1].c_str() );
	maxs.x = (float)atof( splitStrings[2].c_str() );
	maxs.y = (float)atof( splitStrings[3].c_str() );
}

