#pragma once
#include "Engine/Math/AABB2.hpp"

/*
 *	TYPES
 */
class Camera;
class Texture;
class RenderContext;
struct Mat44;

struct Vec3;
struct Rgba8;
struct LineSegment3;

// Only effects 'World' mode stuff
enum eDebugRenderMode
{
	DEBUG_RENDER_ALWAYS,
	DEBUG_RENDER_USE_DEPTH,
	DEBUG_RENDER_XRAY
};

enum eDebugRenderTo
{
	DEBUG_RENDER_TO_SCREEN,
	DEBUG_RENDER_TO_WORLD
};


/************************************************************************/
/* Function Prototypes                                                  */
/************************************************************************/


/************************************************************************/
/* System                                                                     */
/************************************************************************/

//Setup
void DebugRenderSystemStartup( RenderContext* context );
void DebugRenderSystemShutdown();

//control
void EnableDebugRendering();
void DisableDebugRendering();
void ClearDebugRendering();

// output
void DebugRenderBeginFrame();
void DebugRenderWorldToCamera( Camera* cam );
void DebugRenderScreenTo( Texture* output );
void DebugRenderEndFrame();

/************************************************************************/
/* World Rendering                                                                     */
/************************************************************************/
// points
void DebugAddWorldPoint( Vec3 const& pos, float size, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldPoint( Vec3 const& pos, float size, Rgba8 const& color, float duration = 0.f, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldPoint( Vec3 const& pos, Rgba8 const& color, float duration = 0.f, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

// lines
void DebugAddWorldLine( LineSegment3 const& line, 
	Rgba8 const& p0StartColor, Rgba8 const& p0EndColor, 
	Rgba8 const& p1StartColor, Rgba8 const& p1EndColor, 
	float duration, eDebugRenderMode mode );
void DebugAddWorldLine( LineSegment3 const& line, 
	Rgba8 const& color, float duration = 0.f, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

// line strip [EXTRA]

// arrows


// AABB3

// bounds

//basis

//text
//non-billboarded
void DebugAddWorldText( Mat44 const& basis, Vec2 pivot, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode, char const* text );
void DebugAddWorldTextf( Mat44 const& basis, Vec2 pivot, Rgba8 const& color, float duration, eDebugRenderMode mode, char const* format, ... );
void DebugAddWorldTextf( Mat44 const& basis, Vec2 pivot, Rgba8 const& color, char const* format, ... ); //assume DEBUG_RENDER_USE_DEPTH

																										//billboarded
void DebugAddWorldBillboardText( Vec3 const& origin, Vec2 const& pivot, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode, char const* text ); 
void DebugAddWorldBillboardTextf( Vec3 const& origin, Vec2 const& pivot, Rgba8 const& color, float duration, eDebugRenderMode mode, char const* format, ... );
void DebugAddWorldBillboardTextf( Vec3 const& origin, Vec2 const& pivot, Rgba8 const& color, char const* format, ... );
//grid [EXTRA]

//mesh [EXTRA]

/************************************************************************/
/* Screen Rendering                                                     */
/************************************************************************/
//point
void DebugAddScreenPoint( Vec2 const& pos, float size, Rgba8 const& startColor, Rgba8 const& endColor, float duration);
void DebugAddScreenPoint( Vec2 const& pos, float size, Rgba8 const& color, float duration = 0.f );
void DebugAddScreenPoint( Vec2 const& pos, Rgba8 const& color ); //assumed size

//line
void DebugAddScreenLine( Vec2 const& p0, Vec2 const& p1, Rgba8 const& startColor, Rgba8 const& endColor, float duration );
void DebugAddScreenLine( Vec2 const& p0, Vec2 const& p1, Rgba8 const& color, float duration = 0.f );

//Arrow
void DebugAddScreenArrow( Vec2 const& p0, Vec2 const& p1, Rgba8 const& startColor, Rgba8 const& endColor, float duration );
void DebugAddScreenArrow( Vec2 const& p0, Vec2 const& p1, Rgba8 const& color, float duration = 0.f );

//AABB2
void DebugAddScreenAABB2( AABB2 const& bounds, Rgba8 const& startColor, Rgba8 const& endColor, float duration );
void DebugAddScreenAABB2( AABB2 const& bounds, Rgba8 const& color, float duration = 0.f );


void DebugRenderSetScreenHeight( float height );
AABB2 DebugGetScreenBounds();


