//-----------------------------------------------------------------------------------------------
// UnitTests_Custom.hpp
//
#include "Game/GameCommon.hpp"


//My libraries to test
#include "Game/UnitTests_MP1A1.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/OBB2.hpp"


bool IsMostlyEqual( float a, float b, float epsilon=0.001f );
bool IsMostlyEqual( const Vector2Class& vec2, float x, float y );
bool IsMostlyEqual( const Vector2Class& vec2a, const Vector2Class& vec2b );
bool IsMostlyEqual( int a, int b, int epsilon=0 );


//-----------------------------------------------------------------------------------------------
void RunTests_Custom();

