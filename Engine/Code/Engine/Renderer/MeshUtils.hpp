#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>


void AppendIndexedVertsCube( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, float cubeHalfHeight = 1.f );
void AppendIndexedVertsSphere( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, Vec3 center, float sphereRadius = 1.f, uint horizontalCuts = 64, uint verticalCuts = 64, Rgba8 color = Rgba8::WHITE );