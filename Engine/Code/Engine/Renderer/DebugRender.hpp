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
//billboarded
void DebugAddWorldBillboardText( Vec3 const& origin, Vec2 const& pivot, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode, char const* text ); 
//grid [EXTRA]

//mesh [EXTRA]

/************************************************************************/
/* Screen Rendering                                                     */
/************************************************************************/
void DebugAddScreenPoint( Vec2 const& pos, float size, Rgba8 const& startColor, Rgba8 const& endColor, float duration);



void DebugRenderSetScreenHeight( float height );
AABB2 DebugGetScreenBounds();


