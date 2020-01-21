//-----------------------------------------------------------------------------------------------
// UnitTests_MP1A4.hpp
//
#include "Game/GameCommon.hpp"
#include "Game/UnitTests_MP1A3.hpp" // Uses any or all #defines and dependencies from MP1-A1


//-----------------------------------------------------------------------------------------------
void RunTests_MP1A4();


//-----------------------------------------------------------------------------------------------
// YOU MAY COMMENT THESE OUT TEMPORARILY to disable certain test sets while you work.
// For every assignment submission, all test sets must be enabled.
//
#define ENABLE_TestSet_MP1A4_PushOutOf
#define ENABLE_TestSet_MP1A4_ProjectAndReflect
#define ENABLE_TestSet_MP1A4_TransformUsingIJ
#define ENABLE_TestSet_MP1A4_MiscGeometricUtilities


//-----------------------------------------------------------------------------------------------
// YOU MAY CHANGE any of these #includes to match your engine filenames (e.g. Vector2D.hpp, etc.)
//


//-----------------------------------------------------------------------------------------------
// YOU MAY CHANGE the "Your Name" column of these #defines to match your own classes / functions
//
//		MathUnitTests name					Your name
//		~~~~~~~~~~~~~~~~~~~~~~				~~~~~~~~~~~~~~~~~~~~~~
#define Vec2_Reflect						Reflect
#define Vec2_GetReflected					GetReflected

#define Function_GetNearestPointOnDisc2D	GetNearestPointOnDisc2D
#define Function_PushDiscOutOfPoint2D		PushDiscOutOfPoint2D
#define Function_PushDiscOutOfDisc2D		PushDiscOutOfDisc2D
#define Function_PushDiscsOutOfEachOther2D	PushDiscsOutOfEachOther2D
#define Function_PushDiscOutOfAABB2D		PushDiscOutOfAABB2D

#define Function_GetProjectedLength2D		GetProjectedLength2D
#define Function_GetProjectedOnto2D			GetProjectedOnto2D
#define Function_GetDegreesBetweenVectors2D	GetAngleDegreesBetweenVectors2D

#define Function_GetTaxicabDistance2D		GetTaxicabDistance2D
#define Function_IsPointInForwardSector2D	IsPointInForwardSector2D

#define Function_TransformPosition2D_IJ		TransformPosition2D
#define Function_TransformPosition3DXY_IJ	TransformPosition3DXY



