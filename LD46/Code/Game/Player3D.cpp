#include "Game/Player3D.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Golem.hpp"
#include "Engine/Renderer/SkeletalMeshBone.hpp"
#include "Engine/Renderer/SkeletalMesh.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include <vector>

extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;

Player3D::Player3D()
{
	m_mesh = new GPUMesh( g_theRenderer );

	std::vector<Vertex_PCUTBN> sphereVerts;
	std::vector<uint> sphereIndices;
	Vertex_PCUTBN::AppendIndexedVertsSphere( sphereVerts, sphereIndices, 1.f );
	m_mesh->UpdateVertices( sphereVerts );
	m_mesh->UpdateIndices( sphereIndices );

	m_transform.m_position = Vec3( 0.f, 0.f, 1.f );

	m_golem = new Golem();
}

Player3D::~Player3D()
{
	delete m_mesh;
	m_mesh = nullptr;

	delete m_golem;
	m_golem = nullptr;
}

void Player3D::Update( float deltaSeconds )
{
	CheckButtonPresses( deltaSeconds );
	m_transform.m_position += m_velocity * deltaSeconds;
	m_transform.SetUniformScale( m_golemScale );
	m_golem->Update( deltaSeconds, m_transform );

	m_golemScale -= 0.25f * m_golemScale * deltaSeconds;
	if( m_golemScale < 0.1f )
	{
		m_golemScale = 0.1f;
		m_isDead = true;
	}
}

void Player3D::Render()
{
	m_golem->Render();
	//DebugAddWorldWireSphere( m_transform.m_position, GetRadius(), Rgba8::WHITE, 0.f );
}

float Player3D::GetRadius() const
{
	//Vec3 scale = m_transform.m_scale;
	float radius = 5.f * m_golemScale;
	return radius;
}

Vec3 const& Player3D::GetPosition() const
{
	return m_transform.m_position;
}

void Player3D::CheckButtonPresses( float deltaSeconds )
{
// 	const KeyButtonState& leftArrow = g_theInput->GetKeyStates( 0x25 );
// 	const KeyButtonState& upArrow = g_theInput->GetKeyStates( 0x26 );
// 	const KeyButtonState& rightArrow = g_theInput->GetKeyStates( 0x27 );
// 	const KeyButtonState& downArrow = g_theInput->GetKeyStates( 0x28 );

	const KeyButtonState& wKey = g_theInput->GetKeyStates( 'W' );
	const KeyButtonState& aKey = g_theInput->GetKeyStates( 'A' );
	const KeyButtonState& sKey = g_theInput->GetKeyStates( 'S' );
	const KeyButtonState& dKey = g_theInput->GetKeyStates( 'D' );
	const KeyButtonState& spaceKey = g_theInput->GetKeyStates( SPACE_KEY );


	Vec3 translator;
	float chestRotation = 0.f;

	if( wKey.IsPressed() )
	{
		translator.z -=  30.f * deltaSeconds;
	}
	if( sKey.IsPressed() )
	{
		translator.z +=  30.f * deltaSeconds;
	}
	if( aKey.IsPressed() )
	{
		//translator.x -=  10.f * deltaSeconds;
		chestRotation += 180.f * deltaSeconds;
	}
	if( dKey.IsPressed() )
	{
		chestRotation -= 180.f * deltaSeconds;
		//translator.x +=  10.f * deltaSeconds;
	}
	if( spaceKey.WasJustPressed() )
	{
		Vec3 playerPosition = m_transform.m_position;
		if( AlmostEqualsFloat( playerPosition.y, GetRadius(), 2.f ) )
		{
			m_velocity.y = 50.f;
			m_golem->m_jumpTimer.Reset();
		}
	}

	m_transform.RotatePitchRollYawDegrees( 0.f, chestRotation, 0.f );
	//Transform chestTransform = m_golem->m_golemMesh->m_rootBone->m_transform;

	Mat44 translationMatrix;
	translationMatrix.RotateZDegrees( m_transform.m_rotationPitchRollYawDegrees.z );
	translationMatrix.RotateYDegrees( m_transform.m_rotationPitchRollYawDegrees.y );
	translationMatrix.RotateXDegrees( m_transform.m_rotationPitchRollYawDegrees.x );

	Vec3 absoluteTranslation = translationMatrix.TransformPosition3D( translator );
	//Translate( absoluteTranslation );

	m_transform.m_position += absoluteTranslation;
	//m_camera.TranslateRelativeToView( translator );


	Vec2 mouseChange = g_theInput->GetMouseDeltaPos();

	Vec3 rotator;
	rotator.x -= mouseChange.y * 0.1f;
	rotator.y -= mouseChange.x * 0.1f;

	Vec3 rotationPitchRollYaw = m_golem->m_golemMesh->m_headBone->m_transform.m_rotationPitchRollYawDegrees;
	rotationPitchRollYaw += rotator;

	float pitch = Clampf( rotationPitchRollYaw.x, -85.f, 85.f );

	m_golem->m_golemMesh->m_headBone->m_transform.SetRotationFromPitchRollYawDegrees( pitch, rotationPitchRollYaw.y, rotationPitchRollYaw.z );

	//m_transform.SetRotationFromPitchRollYawDegrees( pitch, rotationPitchRollYaw.y, rotationPitchRollYaw.z );


}

Transform const& Player3D::GetPlayerTransform() const
{
	return m_transform;
}

Mat44 Player3D::GetPlayerHeadMatrix() const
{
	return m_golem->GetHeadMatrix();
}

Transform Player3D::GetPlayerHeadTransform() const
{
	return m_golem->GetHeadTransform();
}

Transform Player3D::GetPlayerChestTransform() const
{
	return m_golem->GetChestTransform();
}

void Player3D::SetPosition( Vec3 const& position )
{
	m_transform.m_position = position;
}

