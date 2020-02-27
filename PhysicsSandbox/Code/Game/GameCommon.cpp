#pragma once
#include "GameCommon.hpp"
#include "App.hpp"

extern RenderContext* g_theRenderer;
Window* g_theWindow = nullptr;




void DrawLine( Vec2 startPoint, Vec2 endPoint, Rgba8 color, float thickness )
{
	Vec2 normalizedDisplacement = (endPoint - startPoint).GetNormalized();
	float radius = thickness * 0.5f;
	Vec2 fwd = normalizedDisplacement * radius;
	Vec2 leftVec = fwd.GetRotated90Degrees();
	Vec2 endLeft = endPoint + fwd + leftVec;
	Vec2 endRight= endPoint + fwd - leftVec;
	Vec2 startLeft = startPoint - fwd + leftVec;
	Vec2 startRight = startPoint - fwd - leftVec;

	Vertex_PCU vertexes[6] = 
	{
		Vertex_PCU(Vec3(startRight.x,startRight.y,0.f),color,Vec2(0.f,0.f)),
		Vertex_PCU(Vec3(endRight.x,endRight.y,0.f),color,Vec2(0.f,0.f)),
		Vertex_PCU(Vec3(endLeft.x,endLeft.y,0.f),color,Vec2(0.f,0.f)),

		Vertex_PCU(Vec3(startLeft.x,startLeft.y,0.f),color,Vec2(0.f,0.f)),
		Vertex_PCU(Vec3(startRight.x,startRight.y,0.f),color,Vec2(0.f,0.f)),
		Vertex_PCU(Vec3(endLeft.x,endLeft.y,0.f),color,Vec2(0.f,0.f))
	};
	g_theRenderer->DrawVertexArray(6, vertexes);
}

void DrawRing( Vec2 center, float radius, Rgba8 color, float thickness )
{
	Vec2 ringArray[64];
	for( int thetaIndex = 0; thetaIndex < 64; thetaIndex++ )
	{
		ringArray[thetaIndex] = Vec2::MakeFromPolarDegrees(360.f/64.f * thetaIndex, radius);
		ringArray[thetaIndex] += center;
	}
	for( int ringIndex = 0; ringIndex < 64; ringIndex++ )
	{
		if( ringIndex != 63 )
		{
			DrawLine( ringArray[ringIndex], ringArray[ringIndex+1], color, thickness );

		}
		else {
			DrawLine( ringArray[ringIndex], ringArray[0], color, thickness );

		}
	}

}




