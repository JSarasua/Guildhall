#include "Beetle.hpp"
#include "App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;

Beetle::Beetle( Game* game ): Entity(game)
{

}

Beetle::Beetle( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, Game* game ) :
	Entity(initialPosition, initialVelocity,initialOrientationDegrees, initialAngularVelocity, game)
{

}

void Beetle::Startup()
{

}

void Beetle::Update( float deltaSeconds )
{
	m_position = TransformPosition2D( m_position, 1.f, 0.f, m_velocity * deltaSeconds );
	Vec2 playerDisplacement = m_game->GetPlayerPosition() - m_position;
	m_orientationDegrees = playerDisplacement.GetAngleDegrees();
	m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, BEETLE_SPEED);

}

void Beetle::Render() const
{
	Vertex_PCU beetleVertexes[] =
	{
		//Body Bottom
		Vertex_PCU( Vec3( -2.f,1.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -2.f,-1.f,0.f ),Rgba8( 255,255,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,-1.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Body Top
		Vertex_PCU( Vec3( -2.0f,1.f,0.f ),Rgba8( 255,0,0,0 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,-1.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,1.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Front
		Vertex_PCU( Vec3( 1.f,-1.f,0.f ),Rgba8( 255,0,0,0 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 2.f,0.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,1.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Leg Top Left
		Vertex_PCU( Vec3( -2.0f,1.f,0.f ),Rgba8( 255,0,0,0 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -1.f,1.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -1.5f,1.5f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Leg Top Middle
		Vertex_PCU( Vec3( -1.0f,1.f,0.f ),Rgba8( 255,0,0,0 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -0.5f,1.5f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Leg Top Right
		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 255,0,0,0 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,1.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.5f,1.5f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Leg Bottom Left
		Vertex_PCU( Vec3( -2.0f,-1.f,0.f ),Rgba8( 255,0,0,0 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -1.f,-1.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -1.5f,-1.5f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Leg Bottom Middle
		Vertex_PCU( Vec3( -1.0f,-1.f,0.f ),Rgba8( 255,0,0,0 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-1.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -0.5f,-1.5f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),

		//Leg Bottom Right
		Vertex_PCU( Vec3( 0.f,-1.f,0.f ),Rgba8( 255,0,0,0 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,-1.f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.5f,-1.5f,0.f ),Rgba8( 255,0,0,255 ),Vec2( 0.f,0.f ) )

	};
	Vertex_PCU::TransformVertexArray( 27, beetleVertexes, 1.f, m_orientationDegrees, m_position );

	g_theRenderer->DrawVertexArray( 27, beetleVertexes );
}

void Beetle::ActivateBeetle( Vec2 initialPosition, float initialOrientationDegrees )
{
	SetAlive();
	m_position = initialPosition;
	m_orientationDegrees = initialOrientationDegrees;
	m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees, BEETLE_SPEED );
	m_health = BEETLE_HEALTH;

	m_color = Rgba8( 255,0,0,255 );
}

