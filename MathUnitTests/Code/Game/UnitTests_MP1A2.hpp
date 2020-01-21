//-----------------------------------------------------------------------------------------------
// UnitTests_MP1A2.hpp
//
#include "Game/GameCommon.hpp"
#include "Game/UnitTests_MP1A1.hpp" // Uses any or all #defines and dependencies from MP1-A1


//-----------------------------------------------------------------------------------------------
void RunTests_MP1A2();


//-----------------------------------------------------------------------------------------------
// YOU MAY COMMENT THESE OUT TEMPORARILY to disable certain test sets while you work.
// For every assignment submission, all test sets must be enabled.
//
#define ENABLE_TestSet_MP1A2_MathUtils_Angles
#define ENABLE_TestSet_MP1A2_MathUtils_Queries
#define ENABLE_TestSet_MP1A2_Vec2_Methods
#define ENABLE_TestSet_MP1A2_Vec3_Basics
#define ENABLE_TestSet_MP1A2_Vec3_Methods
#define ENABLE_TestSet_MP1A2_RandomNumberGenerator
#define ENABLE_TestSet_MP1A2_MathUtils_Transforms


//-----------------------------------------------------------------------------------------------
// YOU MAY CHANGE any of these #includes to match your engine filenames (e.g. Vector2D.hpp, etc.)
//
#include "Engine/Math/Vec3.hpp"						// #include for your Vec3 struct/class
#include "Engine/Math/MathUtils.hpp"				// #include for your math utilities header
#include "Engine/Math/RandomNumberGenerator.hpp"	// #include for your RNG class header


//-----------------------------------------------------------------------------------------------
// YOU MAY CHANGE the "Your Name" column of these #defines to match your own classes / functions
//
//		MathUnitTests name					Your name
//		~~~~~~~~~~~~~~~~~~~~~~				~~~~~~~~~~~~~~~~~~~~~~
#define Vector3Class						Vec3
#define Vertex3DPCUClass					Vertex_PCU
#define RngClass							RandomNumberGenerator

#define Function_ConvertDegreesToRadians	ConvertDegreesToRadians
#define Function_ConvertRadiansToDegrees	ConvertRadiansToDegrees
#define Function_CosDegrees					CosDegrees
#define Function_SinDegrees					SinDegrees
#define Function_Atan2Degrees				Atan2Degrees

#define Function_GetDistance2D				GetDistance2D
#define Function_GetDistanceSquared2D		GetDistanceSquared2D

#define Function_GetDistance3D				GetDistance3D
#define Function_GetDistanceXY3D			GetDistanceXY3D
#define Function_GetDistanceSquared3D		GetDistanceSquared3D
#define Function_GetDistanceXYSquared3D		GetDistanceXYSquared3D

#define Function_DoDiscsOverlap				DoDiscsOverlap
#define Function_DoSpheresOverlap			DoSpheresOverlap

#define Vec2_MakeFromPolarDegrees			MakeFromPolarDegrees
#define Vec2_MakeFromPolarRadians			MakeFromPolarRadians

#define Vec2_GetLength						GetLength
#define Vec2_GetLengthSquared				GetLengthSquared
#define Vec2_GetAngleDegrees				GetAngleDegrees
#define Vec2_GetAngleRadians				GetAngleRadians

#define Vec2_GetRotated90Degrees			GetRotated90Degrees
#define Vec2_GetRotatedMinus90Degrees		GetRotatedMinus90Degrees
#define Vec2_GetRotatedDegrees				GetRotatedDegrees
#define Vec2_GetRotatedRadians				GetRotatedRadians
#define Vec2_SetAngleDegrees				SetAngleDegrees
#define Vec2_SetAngleRadians				SetAngleRadians
#define Vec2_SetPolarDegrees				SetPolarDegrees
#define Vec2_SetPolarRadians				SetPolarRadians
#define Vec2_RotateDegrees					RotateDegrees
#define Vec2_RotateRadians					RotateRadians
#define Vec2_Rotate90Degrees				Rotate90Degrees
#define Vec2_RotateMinus90Degrees			RotateMinus90Degrees

#define Vec2_GetClamped						GetClamped
#define Vec2_GetNormalized					GetNormalized
#define Vec2_SetLength						SetLength
#define Vec2_ClampLength					ClampLength
#define Vec2_Normalize						Normalize
#define Vec2_NormalizeAndGetLength			NormalizeAndGetPreviousLength

#define Vec3_GetLength						GetLength
#define Vec3_GetLengthXY					GetLengthXY
#define Vec3_GetLengthSquared				GetLengthSquared
#define Vec3_GetLengthXYSquared				GetLengthXYSquared
#define Vec3_GetAngleAboutZDegrees			GetAngleAboutZDegrees
#define Vec3_GetAngleAboutZRadians			GetAngleAboutZRadians
#define Vec3_GetRotatedAboutZDegrees		GetRotatedAboutZDegrees
#define Vec3_GetRotatedAboutZRadians		GetRotatedAboutZRadians

#define Rng_GetRandomIntLessThan			RollRandomIntLessThan
#define Rng_GetRandomIntInRange				RollRandomIntInRange
#define Rng_GetRandomFloatZeroToOne			RollRandomFloatZeroToOneInclusive
#define Rng_GetRandomFloatInRange			RollRandomFloatInRange

#define Function_TransformPosition2D		TransformPosition2D
#define Function_TransformPosition3DXY		TransformPosition3DXY
#define Function_TransformVertexPCUXY		TransformVertex
#define Function_TransformVertexPCUArrayXY	TransformVertexArray



