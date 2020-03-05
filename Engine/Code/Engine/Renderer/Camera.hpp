#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"

#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Transform.hpp"

class Texture;
class RenderBuffer;
class RenderContext;

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

	void SetColorTarget( Texture* texture );
	void SetClearMode( unsigned int clearFlags, Rgba8 color, float depth = 0.f, unsigned int stencil = 0 );

	//void SetOrthoView( const Vec2& bottomLeft, const Vec2&topRight );
	
	Vec2 GetClientToWorldPosition( Vec2 clientPos ) const;
	void SetPosition( const Vec3& position );
	void Translate( const Vec3& translation );
	void TranslateRelativeToView( Vec3 const& translation );
	void RotatePitchRollYawDegrees( Vec3 const& rotator );
	void Translate2D( const Vec2& cameraDisplacement ); //Should be replaced soon
	void UpdateCameraUBO();



	Vec3 GetPosition();

	float GetAspectRatio() const;

	void SetOutputSize( const Vec2& size );
	void SetDepthStencilTarget( Texture* texture );
	void CreateMatchingDepthStencilTarget( RenderContext* context );

	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	Vec2 GetColorTargetSize() const;
	Mat44 GetProjection() const;

	void SetProjectionOrthographic( Vec2 const& size, float nearZ, float farZ );
	void SetProjectionPerspective( float fov, float nearZ, float farZ );

	Mat44 GetViewMatrix() const;
	Mat44 GetProjectionMatrix() const;

	Vec3 ClientToWorld( Vec2 client, float ndcZ ) const;
	Vec3 WorldToClient( Vec3 worldPos ) const;
	Texture* GetColorTarget() const;

public:
	Mat44 m_view;
	Mat44 m_projection;

	Rgba8 m_clearColor;
	unsigned int m_clearMode = 0;

	RenderBuffer* m_cameraUBO = nullptr;

	Transform m_transform;
	Vec2 m_outputSize;

	Texture* m_colorTarget = nullptr;
	Texture* m_depthStencilTarget = nullptr;
	float m_clearDepth = 1.f;
	float m_clearStencil = 0.f;

	Vec3 m_position;
};