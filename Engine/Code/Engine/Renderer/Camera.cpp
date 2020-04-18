#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


Camera::~Camera()
{
	delete m_cameraUBO;
	m_cameraUBO = nullptr;

	delete m_depthStencilTarget;
	m_depthStencilTarget = nullptr;
}

void Camera::SetPosition( const Vec3& position )
{
	m_transform.m_position = position;
}

void Camera::Translate( const Vec3& translation )
{
	m_transform.Translate( translation );
}

void Camera::TranslateRelativeToView( Vec3 const& translation )
{
	Mat44 translationMatrix;
	translationMatrix.RotateZDegrees( m_transform.m_rotationPitchRollYawDegrees.z );
	translationMatrix.RotateYDegrees( m_transform.m_rotationPitchRollYawDegrees.y );
	translationMatrix.RotateXDegrees( m_transform.m_rotationPitchRollYawDegrees.x );

	Vec3 absoluteTranslation = translationMatrix.TransformPosition3D( translation );
	Translate( absoluteTranslation );
}

void Camera::RotatePitchRollYawDegrees( Vec3 const& rotator )
{
	Vec3 rotationPitchRollYaw = m_transform.m_rotationPitchRollYawDegrees;
	rotationPitchRollYaw += rotator;

	float pitch = Clampf( rotationPitchRollYaw.x, -85.f, 85.f );

	m_transform.SetRotationFromPitchRollYawDegrees( pitch, rotationPitchRollYaw.y, rotationPitchRollYaw.z );
}

void Camera::SetScreenShakeIntensity( float newIntensity )
{
	m_screenShakeIntensity = newIntensity;
	m_screenShakeIntensity = Max( m_screenShakeIntensity, 0.f );
}

float Camera::GetCurrentScreenShakeIntensity() const
{
	return m_screenShakeIntensity;
}

Vec3 Camera::GetPosition()
{
	return m_transform.m_position;
}

Vec3 Camera::GetDirection() const
{
	Mat44 cameraRotationMatrix = m_transform.ToRotationMatrix();
	Vec3 cameraDirection = cameraRotationMatrix.TransformVector3D( Vec3( 0.f, 0.f, -1.f ) );
	cameraDirection.Normalize();
	return cameraDirection;
}

float Camera::GetAspectRatio() const
{
	if( nullptr == m_colorTarget )
	{
		return m_outputSize.x / m_outputSize.y;
	}
	else
	{
		return m_colorTarget->GetAspectRatio();
	}
}


Vec2 Camera::GetOrthoBottomLeft() const
{
	Vec3 bottomLeft = ClientToWorld( Vec2(0.f, 0.f), 0.f );
	Vec2 bottomLeft2D = Vec2( bottomLeft.x, bottomLeft.y );
	return bottomLeft2D;
}

Vec2 Camera::GetOrthoTopRight() const
{
	Vec3 topRight = ClientToWorld( Vec2( 1.f, 1.f ), 0.f );
	Vec2 topRight2D = Vec2( topRight.x, topRight.y );
	return topRight2D;
}

Vec2 Camera::GetColorTargetSize() const
{
	if( nullptr != m_colorTarget )
	{
		return Vec2(m_colorTarget->GetTexelSize());
	}
	else
	{
		UNIMPLEMENTED();
		//Implement with 
		//M_renderContext->GetBackBuffer().GetSize();
	}

	return Vec2(0.f,0.f);
}

Vec2 Camera::GetClientToWorldPosition( Vec2 clientPos ) const
{
	float worldX = RangeMap( 0.f, 1.f, GetOrthoBottomLeft().x, GetOrthoTopRight().x, clientPos.x );
	float worldY = RangeMap( 0.f, 1.f, GetOrthoBottomLeft().y, GetOrthoTopRight().y, clientPos.y );
	Vec2 worldPos = Vec2(worldX, worldY);
	return worldPos;
}

void Camera::SetOutputSize( const Vec2& size )
{
	m_outputSize = size;
}

void Camera::SetDepthStencilTarget( Texture* texture )
{
	m_depthStencilTarget = texture;
}

void Camera::CreateMatchingDepthStencilTarget( RenderContext* context )
{
	m_depthStencilTarget = context->CreateDepthStencilTarget();
}

Mat44 Camera::GetProjection() const
{
	return m_projection;
}

void Camera::SetProjectionOrthographic( Vec2 const& size, float nearZ, float farZ )
{
	m_outputSize = size;

	Vec2 halfDimensions = size * 0.5f;
	Vec3 center = m_transform.m_position;
	Vec3 mins = m_transform.m_position - halfDimensions; 
	mins.z = nearZ;
	Vec3 maxs = m_transform.m_position + halfDimensions;
	maxs.z = farZ;
	m_projection = Mat44::CreateOrthographicProjection( mins, maxs );
}

void Camera::SetProjectionPerspective( float fov, float nearZ, float farZ )
{
	m_projection = MakePerspectiveProjectMatrixD3D( fov, GetAspectRatio(), nearZ, farZ );
}

Mat44 Camera::GetViewMatrix() const
{
	return m_view;
}

Mat44 Camera::GetCameraModelMatrix() const
{
	return m_transform.ToMatrix();
}

Mat44 Camera::GetModelRotationMatrix() const
{
	return m_transform.ToRotationMatrix();
}

Mat44 Camera::GetProjectionMatrix() const
{
	return m_projection;
}

Transform Camera::GetTransform() const
{
	return m_transform;
}

Vec3 Camera::ClientToWorld( Vec2 client, float ndcZ ) const
{
	float x = RangeMap( 0.f, 1.f, -1.f, 1.f, client.x );
	float y = RangeMap( 0.f, 1.f, -1.f, 1.f, client.y );

	Vec3 ndc = Vec3(x, y, ndcZ );
	Mat44 proj = GetProjectionMatrix();
	Mat44 worldToClip = proj;
	worldToClip.TransformBy( GetViewMatrix() );
	
	Mat44 clipToWorld = worldToClip;
	MatrixInvert( clipToWorld );
	Vec4 worldHomogeneousPoint = clipToWorld.TransformHomogeneousPoint3D( Vec4(ndc.x, ndc.y, ndc.z, 1) );
	Vec4 worldPos = worldHomogeneousPoint/ worldHomogeneousPoint.w;
	Vec3 worldPos3D = Vec3( worldPos.x, worldPos.y, worldPos.z );


	return worldPos3D;
}

Vec3 Camera::WorldToClient( Vec3 worldPos ) const
{
	UNIMPLEMENTED();
	return Vec3( 0.f, 0.f, 0.f );
}

void Camera::SetColorTarget( Texture* texture )
{
	if( nullptr != texture )
	{
		m_colorTarget = texture;
	}
}

Texture* Camera::GetColorTarget() const
{
	return m_colorTarget;
}

void Camera::SetClearMode( unsigned int clearFlags, Rgba8 color, float depth /*= 0.f*/, unsigned int stencil /*= 0 */ )
{
	UNUSED(depth);
	UNUSED(stencil);

	m_clearMode = clearFlags;
	m_clearColor = color;
}

void Camera::UpdateCameraUBO()
{
	Mat44 cameraOffset = Mat44::CreateTranslation3D( m_cameraOffset );
	Mat44 screenShakeOffset = Mat44::CreateTranslation3D( m_screenShakeOffset );
	Mat44 cameraModel = m_transform.ToMatrix();
	cameraModel.TransformBy( screenShakeOffset );
	cameraModel.TransformBy( cameraOffset );
	m_view = cameraModel;
	MatrixInvertOrthoNormal( m_view );


	//CameraData
	CameraData camData;
	camData.projection = GetProjection();
	camData.view = GetViewMatrix();
	camData.cameraPosition = GetPosition();

	m_cameraUBO->Update( &camData, sizeof( camData ), sizeof( camData ) );
}

void Camera::UpdateScreenShake( RandomNumberGenerator& rand )
{
	float newOffsetX = rand.RollRandomFloatZeroToOneInclusive();
	float newOffsetY = rand.RollRandomFloatZeroToOneInclusive();
	float newOffsetZ = rand.RollRandomFloatZeroToOneInclusive();

	Vec3 newScreenShakeOffset = Vec3( newOffsetX, newOffsetY, newOffsetZ );
	newScreenShakeOffset -= Vec3( 0.5f, 0.5f, 0.5f );
	newScreenShakeOffset *= 0.1f;
	newScreenShakeOffset *= m_screenShakeIntensity;

	m_screenShakeOffset = newScreenShakeOffset;
}
