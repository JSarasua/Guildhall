#include "Wasp.hpp"
#include "App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game.hpp"
#include "CommonColors.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;


Wasp::Wasp( Game* game ): Entity(game)
{

}

Wasp::Wasp( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, Game* game ) :
	Entity( initialPosition, initialVelocity, initialOrientationDegrees, initialAngularVelocity, game )
{

}

void Wasp::Startup()
{

}

void Wasp::Update( float deltaSeconds )
{
	m_position = TransformPosition2D( m_position, 1.f, 0.f, m_velocity * deltaSeconds );
	Vec2 playerDisplacement = m_game->GetPlayerPosition() - m_position;
	m_orientationDegrees = playerDisplacement.GetAngleDegrees();
	m_velocity += Vec2::MakeFromPolarDegrees( m_orientationDegrees, WASP_ACCELERATION * deltaSeconds );

	m_velocity.ClampLength(WASP_MAXSPEED);
}

void Wasp::Render() const
{

	Vertex_PCU waspVertexes[] =
	{
		//LWing
		Vertex_PCU( Vec3( 0.0f,0.5f,0.f ), DARKVIOLET,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,0.5f,0.f ), DARKVIOLET,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -1.f,2.5f,0.f ), FUCHSIA,Vec2( 0.f,0.f ) ),

		//RWing
		Vertex_PCU( Vec3( 0.0f,-0.5f,0.f ), SEAGREEN,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,-0.5f,0.f ), SEAGREEN,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -1.f,-2.5f,0.f ), SPRINGGREEN,Vec2( 0.f,0.f ) ),

		//Tail
		Vertex_PCU( Vec3( -2.f,0.f,0.f ), SALMON,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -1.f,-0.5f,0.f ), CRIMSON,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -1.f,0.5f,0.f ), CRIMSON,Vec2( 0.f,0.f ) ),

		//Back Bottom
		Vertex_PCU( Vec3( -1.f,0.5f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -1.f,-0.5f,0.f ),Rgba8( 255,255,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-.25f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Back Top
		Vertex_PCU( Vec3( -1.0f,0.5f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-0.25f,0.f ),Rgba8( 255,255,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,.25f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Middle Bottom
		Vertex_PCU( Vec3( 0.f,0.25f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-0.25f,0.f ),Rgba8( 255,255,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,-.25f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Middle Top
		Vertex_PCU( Vec3( 0.f,0.25f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,-0.25f,0.f ),Rgba8( 255,255,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,.25f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Front Top
		Vertex_PCU( Vec3( 1.0f,0.25f,0.f ), NAVY,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 2.f,0.f,0.f ), DEEPSKYBLUE,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.5f,.5f,0.f ), NAVY,Vec2( 0.f,0.f ) ),

		//Front Middle
		Vertex_PCU( Vec3( 1.0f,0.25f,0.f ), NAVY,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,-0.25f,0.f ), NAVY,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 2.f,0.f,0.f ), DEEPSKYBLUE,Vec2( 0.f,0.f ) ),

		//Front Bottom
		Vertex_PCU( Vec3( 1.0f,0.25f,0.f ), NAVY,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.5f,-0.5f,0.f ), NAVY,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 2.f,0.f,0.f ), DEEPSKYBLUE,Vec2( 0.f,0.f ) )

	};
	Vertex_PCU::TransformVertexArray( 30, waspVertexes, 1.f, m_orientationDegrees, m_position );

	g_theRenderer->DrawVertexArray( 30, waspVertexes );
}

void Wasp::ActivateWasp( Vec2 initialPosition, float initialOrientationDegrees )
{
	SetAlive();
	m_position = initialPosition;
	m_orientationDegrees = initialOrientationDegrees;
	m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees, 0.f );
	m_health = WASP_HEALTH;
	m_color = NAVY;
}

