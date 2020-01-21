//-----------------------------------------------------------------------------------------------
// UnitTests_MP1A5.hpp
//
#include "Game/GameCommon.hpp"
#include "Game/UnitTests_MP1A4.hpp" // Uses any or all #defines and dependencies from MP1-A1


//-----------------------------------------------------------------------------------------------
void RunTests_MP1A5();


//-----------------------------------------------------------------------------------------------
// YOU MAY COMMENT THESE OUT TEMPORARILY to disable certain test sets while you work.
// For every assignment submission, all test sets must be enabled.
//
#define ENABLE_TestSet_MP1A5_MatrixConstruction
#define ENABLE_TestSet_MP1A5_MatrixStaticCreates
#define ENABLE_TestSet_MP1A5_MatrixAccessors
#define ENABLE_TestSet_MP1A5_MatrixMutators
#define ENABLE_TestSet_MP1A5_MatrixAppends
#define ENABLE_TestSet_MP1A5_MatrixTransforms


//-----------------------------------------------------------------------------------------------
// YOU MAY CHANGE any of these #includes to match your engine filenames (e.g. Vector2D.hpp, etc.)
//
#include "Engine/Math/Vec4.hpp"				// #include for your Vec4 struct/class
#include "Engine/Math/Mat44.hpp"			// #include for your Mat44 matrix struct/class


//-----------------------------------------------------------------------------------------------
// YOU MAY CHANGE the "Your Name" column of these #defines to match your own classes / functions
//
//		MathUnitTests name					Your name
//		~~~~~~~~~~~~~~~~~~~~~~				~~~~~~~~~~~~~~~~~~~~~~
#define Vector4Class						Vec4
#define Matrix44Class						Mat44

#define Mat44_TransformVector2D				TransformVector2D
#define Mat44_TransformVector3D				TransformVector3D
#define Mat44_TransformPosition2D			TransformPosition2D
#define Mat44_TransformPosition3D			TransformPosition3D
#define Mat44_TransformHomogeneousPoint3D	TransformHomogeneousPoint3D

#define Mat44_CreateTranslationXY			CreateTranslationXY
#define Mat44_CreateTranslation3D			CreateTranslation3D
#define Mat44_CreateUniformScaleXY			CreateUniformScaleXY
#define Mat44_CreateNonUniformScaleXY		CreateNonUniformScaleXY
#define Mat44_CreateUniformScale3D			CreateUniformScale3D
#define Mat44_CreateNonUniformScale3D		CreateNonUniformScale3D
#define Mat44_CreateZRotationDegrees		CreateZRotationDegrees
#define Mat44_CreateYRotationDegrees		CreateYRotationDegrees
#define Mat44_CreateXRotationDegrees		CreateXRotationDegrees

#define Mat44_RotateZDegrees				RotateZDegrees
#define Mat44_RotateYDegrees				RotateYDegrees
#define Mat44_RotateXDegrees				RotateXDegrees
#define Mat44_Translate2D					Translate2D
#define Mat44_Translate3D					Translate3D
#define Mat44_ScaleUniform2D				ScaleUniform2D
#define Mat44_ScaleUniform3D				ScaleUniform3D
#define Mat44_ScaleNonUniform2D				ScaleNonUniform2D
#define Mat44_ScaleNonUniform3D				ScaleNonUniform3D
#define Mat44_TransformBy					TransformBy

#define Mat44_GetAsFloatArray				GetAsFloatArray
#define Mat44_GetAsConstFloatArray			GetAsFloatArray
#define Mat44_GetIBasis2D					GetIBasis2D
#define Mat44_GetJBasis2D					GetJBasis2D
#define Mat44_GetTranslation2D				GetTranslation2D
#define Mat44_GetIBasis3D					GetIBasis3D
#define Mat44_GetJBasis3D					GetJBasis3D
#define Mat44_GetKBasis3D					GetKBasis3D
#define Mat44_GetTranslation3D				GetTranslation3D
#define Mat44_GetIBasis4D					GetIBasis4D
#define Mat44_GetJBasis4D					GetJBasis4D
#define Mat44_GetKBasis4D					GetKBasis4D
#define Mat44_GetTranslation4D				GetTranslation4D

#define Mat44_SetTranslation2D				SetTranslation2D
#define Mat44_SetTranslation3D				SetTranslation3D
#define Mat44_SetIJ2D						SetBasisVectors2D
#define Mat44_SetIJT2D						SetBasisVectors2D
#define Mat44_SetIJK3D						SetBasisVectors3D
#define Mat44_SetIJKT3D						SetBasisVectors3D
#define Mat44_SetIJKT4D						SetBasisVectors4D


