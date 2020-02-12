#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Mat44.hpp"

class Texture;
class RenderBuffer;
struct Rgba8;

enum eCameraClearBitFlag : unsigned int
{
	CLEAR_COLOR_BIT			= (1 << 0 ),
	CLEAR_DEPTH_BIT			= (1 << 1 ),
	CLEAR_STENCIL_BIT		= (1 << 2 ),
};

struct CameraData
{
	Mat44 projection;
	Mat44 view;
};


class Camera
{
public:
	Camera() {}
	~Camera();

	void SetPosition( const Vec3& position );
	void Translate( const Vec3& translation );

	Vec3 GetPosition();

	void SetOrthoView( const Vec2& bottomLeft, const Vec2&topRight );
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	Mat44 GetProjection() const;


	void SetColorTarget( Texture* texture );
	Texture* GetColorTarget() const;
	void SetClearMode( unsigned int clearFlags, Rgba8 color, float depth = 0.f, unsigned int stencil = 0 );

public:
	Rgba8 m_clearColor;
	unsigned int m_clearMode = 0;

	RenderBuffer* m_cameraUBO = nullptr;

private:
	Vec2 bLeft;
	Vec2 tRight;
	Texture* m_texture = nullptr;

	Vec3 m_position;

	Mat44 m_view;
	Mat44 m_projection;

public:
	void Translate2D( const Vec2& cameraDisplacement );
};