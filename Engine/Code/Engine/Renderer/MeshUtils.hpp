#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "ThirdParty/MikkT/mikktspace.h"
#include <vector>

struct Vertex_PCUTBN;

struct MeshImportOptions_t
{
public:
	Transform m_transform;
	bool m_hasNormals = false;
};

void AppendIndexedVertsCube( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, float cubeHalfHeight = 1.f );
void AppendIndexedVertsSphere( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, Vec3 center, float sphereRadius = 1.f, uint horizontalCuts = 64, uint verticalCuts = 64, Rgba8 color = Rgba8::WHITE );
void AppendIndexedVertsCylinder( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, Vec3 const& startPos, float startRadius, Vec3 const& endPos, float endRadius, uint cuts = 16, Rgba8 color = Rgba8::WHITE );
void AppendIndexedVertsCone( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, Vec3 const& basePos, float baseRadius, Vec3 const& endPos, uint cuts = 16, Rgba8 color = Rgba8::WHITE );

void LoadOBJToVertexArray( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList, char const* filename, MeshImportOptions_t const& options );

//MikkT
static int GetNumFaces( SMikkTSpaceContext const* context );
static int GetNumberOfVerticesForFace( SMikkTSpaceContext const* pContext, const int iFace );
static void GetPositionForFaceVert( SMikkTSpaceContext const* pContext, float fvPosOut[], const int iFace, const int iVert );
static void GetNormalForFaceVert( SMikkTSpaceContext const* pContext, float fvNormOut[], const int iFace, const int iVert );
static void GetUVForFaceVert( SMikkTSpaceContext const* pContext, float fvTexcOut[], const int iFace, const int iVert );
static void SetTangent( SMikkTSpaceContext const* pContext,
	float const fvTangent[],
	float const fSign,
	int const iFace,
	int const iVert);
static void GenerateTangentsForVertexArray( std::vector<Vertex_PCUTBN>& vertices );
