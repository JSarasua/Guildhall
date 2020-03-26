#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EventSystem.hpp"

/*
 *	TYPES
 */
class Camera;
class Texture;
class RenderContext;
class Transform;

struct Mat44;
struct Vec3;
struct Vec4;
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
	Rgba8 const& startColor, Rgba8 const& endColor, 
	float duration, eDebugRenderMode mode );
void DebugAddWorldLine( LineSegment3 const& line, 
	Rgba8 const& color, float duration = 0.f, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

// line strip [EXTRA]

// arrows


// AABB3

// bounds
//Assuming an AABB3 bounds of -0.5 to 0.5
void DebugAddWorldWireBounds( Transform const& transform, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

void DebugAddWorldWireSphere( Vec3 const& pos, float radius, Rgba8 const& startColor, Rgba8 const& endColor, float duration, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
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

//Textured AABB2
void DebugAddScreenTexturedQuad( AABB2 const& bounds, Texture* tex, AABB2 const& uvs, Rgba8 const& startTint, Rgba8 const& endTint, float duration = 0.f );
void DebugAddScreenTexturedQuad( AABB2 const& bounds, Texture* tex, AABB2 const& uvs, Rgba8 const& tint, float duration = 0.f );
void DebugAddScreenTexturedQuad( AABB2 const& bounds, Texture* tex ); //UVs assumed to be 0 to 1

void DebugAddScreenText( Vec4 const& pos, Vec2 const& pivot, float textSize, Rgba8 const& startColor, Rgba8 const& endColor, float duration, char const* text );
void DebugAddScreenTextf( Vec4 const& pos, Vec2 const& pivot, float textSize, Rgba8 const& startColor, Rgba8 const& endColor, float duration, char const* format, ... );
void DebugAddScreenTextf( Vec4 const& pos, Vec2 const& pivot, float textSize, Rgba8 const& color, float duration, char const* format, ... );
void DebugAddScreenTextf( Vec4 const& pos, Vec2 const& pivot, float textSize, Rgba8 const& color, char const* format, ... );
void DebugAddScreenTextf( Vec4 const& pos, Vec2 const& pivot, Rgba8 const& color, char const* format, ... );

void DebugRenderSetScreenHeight( float height );
AABB2 DebugGetScreenBounds();


bool SetDebugRenderEnabled( const EventArgs* args );
bool DebugAddWorldPoint( const EventArgs* args );
bool DebugAddWorldWireSphere( const EventArgs* args );
bool DebugAddWorldWireBounds( const EventArgs* args );
bool DebugAddWorldBillboardText( const EventArgs* args );
bool DebugAddScreenPoint( const EventArgs* args );
bool DebugAddScreenQuad( const EventArgs* args );
bool DebugAddScreenText( const EventArgs* args );

