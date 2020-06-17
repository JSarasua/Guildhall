#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Core/EngineCommon.hpp"


class Texture;
class RenderBuffer;
class RenderContext;
class RandomNumberGenerator;

struct Rgba8;

enum eCameraClearBitFlag : unsigned int
{
	NO_CLEAR				= 0,
	CLEAR_COLOR_BIT			= (1 << 0 ),
	CLEAR_DEPTH_BIT			= (1 << 1 ),
	CLEAR_STENCIL_BIT		= (1 << 2 ),
};

enum eCameraType
{
	WORLDCAMERA,
	SCREENCAMERA
};

struct CameraData
{
	Mat44 projection;
	Mat44 view;
	Vec3 cameraPosition;

	float pad00;
};


class Camera
{
public:
	Camera() {}
	~Camera();

	void SetColorTarget( Texture* texture );
	void SetColorTarget( uint index, Texture* texture );
	uint GetColorTargetCount() const;
	Texture* GetColorTarget( uint index ) const;
	
	void SetClearMode( unsigned int clearFlags, Rgba8 color, float depth = 0.f, unsigned int stencil = 0 );

	//void SetOrthoView( const Vec2& bottomLeft, const Vec2&topRight );
	
	Vec2 GetClientToWorldPosition( Vec2 clientPos ) const;
	void SetPosition( const Vec3& position );
	void Translate( const Vec3& translation );
	void TranslateRelativeToView( Vec3 const& translation );
	void TranslateRelativeToViewOnlyYaw( Vec3 const& translation );
	void RotatePitchRollYawDegrees( Vec3 const& rotator );
	void SetRotationPitchRollYawDegrees( Vec3 const& pitchRollYawDegrees );
	void UpdateCameraUBO();

	void UpdateScreenShake( RandomNumberGenerator& rand );
	void SetScreenShakeIntensity( float newIntensity );
	float GetCurrentScreenShakeIntensity() const;

	Vec3 GetPosition();
	Vec3 GetRotation();
	Vec3 GetDirection() const;

	float GetAspectRatio() const;

	void SetOutputSize( const Vec2& size );
	void SetDepthStencilTarget( Texture* texture );
	void CreateMatchingDepthStencilTarget( RenderContext* context );

	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	Vec2 GetOutputSize() const;
	Vec2 GetColorTargetSize() const;
	Mat44 GetProjection() const;

	void SetProjectionOrthographic( Vec2 const& size, float nearZ, float farZ );
	void SetProjectionPerspective( float fov, float nearZ, float farZ );

	Mat44 GetViewMatrix() const;
	Mat44 GetCameraModelMatrix() const;
	Mat44 GetModelRotationMatrix() const;
	Mat44 GetCameraScreenRotationMatrix() const;
	Mat44 GetProjectionMatrix() const;
	Transform GetTransform() const;

	Vec3 ClientToWorld( Vec2 client, float ndcZ ) const;
	Vec3 WorldToClient( Vec3 worldPos ) const;
	Texture* GetColorTarget() const;

public:
	Vec3 m_screenShakeOffset;
	float m_screenShakeIntensity = 0.f;

	Vec3 m_cameraOffset;

	Mat44 m_view;
	Mat44 m_projection;

	Rgba8 m_clearColor;
	unsigned int m_clearMode = 0;

	RenderBuffer* m_cameraUBO = nullptr;

	Transform m_transform;
	Vec2 m_outputSize;

	//Texture* m_colorTarget = nullptr;
	std::vector<Texture*> m_colorTargets;
	Texture* m_depthStencilTarget = nullptr;
	float m_clearDepth = 1.f;
	float m_clearStencil = 0.f;

	eCameraType m_cameraType = WORLDCAMERA;
};