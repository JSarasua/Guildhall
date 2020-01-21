//-----------------------------------------------------------------------------------------------
// UnitTests_MP1A3.hpp
//
#include "Game/GameCommon.hpp"
#include "Game/UnitTests_MP1A2.hpp" // Uses any or all #defines and dependencies from MP1-A1


//-----------------------------------------------------------------------------------------------
void RunTests_MP1A3();


//-----------------------------------------------------------------------------------------------
// YOU MAY COMMENT THESE OUT TEMPORARILY to disable certain test sets while you work.
// For every assignment submission, all test sets must be enabled.
//
#define ENABLE_TestSet_MP1A3_AABB2Basics
#define ENABLE_TestSet_MP1A3_AABB2Methods
#define ENABLE_TestSet_MP1A3_IntVec2Basics
#define ENABLE_TestSet_MP1A3_IntVec2Methods
#define ENABLE_TestSet_MP1A3_LerpAndClamp
#define ENABLE_TestSet_MP1A3_Turning2D
#define ENABLE_TestSet_MP1A3_GeometricQueries2D
#define ENABLE_TestSet_MP1A3_DotProduct2D


//-----------------------------------------------------------------------------------------------
// YOU MAY CHANGE any of these #includes to match your engine filenames (e.g. Vector2D.hpp, etc.)
//
#include "Engine/Math/IntVec2.hpp"				// #include for your IntVec2 struct/class
#include "Engine/Math/AABB2.hpp"				// #include for your AABB2 struct/class


//-----------------------------------------------------------------------------------------------
// YOU MAY CHANGE the "Your Name" column of these #defines to match your own classes / functions
//
//		MathUnitTests name					Your name
//		~~~~~~~~~~~~~~~~~~~~~~				~~~~~~~~~~~~~~~~~~~~~~
#define IntVec2Class						IntVec2
#define AABB2Class							AABB2
#define AABB2_Mins							mins		// e.g. "mins" if your AABB2 is used as "myBox.mins"
#define AABB2_Maxs							maxs		// e.g. "maxs" if your AABB2 is used as "myBox.maxs"

#define AABB2_IsPointInside					IsPointInside
#define AABB2_GetCenter						GetCenter
#define AABB2_GetDimensions					GetDimensions
#define AABB2_GetNearestPoint				GetNearestPoint
#define AABB2_GetPointAtUV					GetPointAtUV
#define AABB2_GetUVForPoint					GetUVForPoint
#define AABB2_Translate						Translate
#define AABB2_SetCenter						SetCenter
#define AABB2_SetDimensions					SetDimensions
#define AABB2_StretchToIncludePoint			StretchToIncludePoint

#define IntVec2_GetLength					GetLength
#define IntVec2_GetLengthSquared			GetLengthSquared
#define IntVec2_GetTaxicabLength			GetTaxicabLength
#define IntVec2_GetOrientationRadians		GetOrientationRadians
#define IntVec2_GetOrientationDegrees		GetOrientationDegrees
#define IntVec2_GetRotated90Degrees			GetRotated90Degrees
#define IntVec2_GetRotatedMinus90Degrees	GetRotatedMinus90Degrees
#define IntVec2_Rotate90Degrees				Rotate90Degrees
#define IntVec2_RotateMinus90Degrees		RotateMinus90Degrees

#define Function_InterpolateFloat			Interpolate
#define Function_RangeMapFloat				RangeMap
#define Function_ClampFloat					Clampf
#define Function_ClampFloatZeroToOne		ClampZeroToOne
#define Function_RoundDownToInt				RoundDownToInt

#define Function_GetShortAngDisp			GetShortestAngularDisplacement
#define Function_GetTurnedToward			GetTurnedToward

#define Function_DotProduct2D				DotProduct2D

