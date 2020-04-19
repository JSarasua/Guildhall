#include "Game/Golem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SkeletalMeshBone.hpp"
#include "Engine/Math/MathUtils.hpp"

extern RenderContext* g_theRenderer;

Golem::Golem()
{
	m_golemMesh = new SkeletalMesh();

	CreateMeshes();
	CreateSkeleton();

	m_jumpTimer.SetSeconds( 0.5f );
	m_jumpTimer.Stop();

}

Golem::~Golem()
{
	delete m_golemMesh;
	m_golemMesh = nullptr;

	delete m_headMesh;
	m_headMesh = nullptr;

	delete m_chestMesh;
	m_chestMesh = nullptr;

	delete m_shoulderMesh;
	m_shoulderMesh = nullptr;

	delete m_hipMesh;
	m_hipMesh = nullptr;

	delete m_armMesh;
	m_armMesh = nullptr;

	delete m_legMesh;
	m_legMesh = nullptr;
}

void Golem::Update( float deltaSeconds, Transform chestTransform )
{
	Vec3 oldChestPosition = m_golemMesh->m_rootBone->m_transform.m_position;
	m_golemMesh->m_rootBone->m_transform = chestTransform;

	Vec3 chestPosition = chestTransform.m_position;
	Vec3 verletVelocity = chestPosition - oldChestPosition;
	float distanceTraveledSinceLastFrame = verletVelocity.GetLength();
	currentDistanceTraveled += distanceTraveledSinceLastFrame;
	float armRotation = 45.f * SinDegrees(  45.f * currentDistanceTraveled );
	float legRotation = 30.f * SinDegrees( 45.f * currentDistanceTraveled );
	float leftElbowRotation = 45.f * SinDegrees( 45.f * currentDistanceTraveled ) + 45.f;
	float rightElbowRotation = 45.f * SinDegrees( -45.f * currentDistanceTraveled ) + 45.f;

	Vec3 newLeftShoulderRotator = m_leftShoulder->m_transform.m_rotationPitchRollYawDegrees;
	newLeftShoulderRotator.x = armRotation;

	Vec3 newRightShoulderRotator = m_rightShoulder->m_transform.m_rotationPitchRollYawDegrees;
	newRightShoulderRotator.x = -armRotation;

	Vec3 newLeftElbowRotator = m_leftElbow->m_transform.m_rotationPitchRollYawDegrees;
	newLeftElbowRotator.x = leftElbowRotation;

	Vec3 newRightElbowRotator = m_rightElbow->m_transform.m_rotationPitchRollYawDegrees;
	newRightElbowRotator.x = rightElbowRotation;

	Vec3 newLeftHipRotator = m_leftHip->m_transform.m_rotationPitchRollYawDegrees;
	newLeftHipRotator.x = -legRotation;

	Vec3 newRightHipRotator = m_rightHip->m_transform.m_rotationPitchRollYawDegrees;
	newRightHipRotator.x = legRotation;


	if( m_jumpTimer.IsRunning() )
	{
		if( m_jumpTimer.HasElapsed() )
		{
			m_jumpTimer.Stop();
			newLeftShoulderRotator.z = 0.f;
			newRightShoulderRotator.z = 0.f;
		}
		else
		{
			float elapsedTime = (float)m_jumpTimer.GetElapsedSeconds();
			newLeftShoulderRotator.z = -90.f * SinDegrees( elapsedTime * 360.f );
			newRightShoulderRotator.z = 90.f * SinDegrees( elapsedTime * 360.f );
		}
	}

	m_leftShoulder->m_transform.SetRotationFromPitchRollYawDegrees( newLeftShoulderRotator );
	m_rightShoulder->m_transform.SetRotationFromPitchRollYawDegrees( newRightShoulderRotator );
	m_leftElbow->m_transform.SetRotationFromPitchRollYawDegrees( newLeftElbowRotator );
	m_rightElbow->m_transform.SetRotationFromPitchRollYawDegrees( newRightElbowRotator );
	m_leftHip->m_transform.SetRotationFromPitchRollYawDegrees( newLeftHipRotator );
	m_rightHip->m_transform.SetRotationFromPitchRollYawDegrees( newRightHipRotator );

}

void Golem::Render()
{
	m_golemMesh->Render();
}

Mat44 Golem::GetHeadMatrix() const
{
	return m_golemMesh->m_headBone->GetRelativeModelMatrix();
}

Transform Golem::GetHeadTransform() const
{
	Transform headTransform = m_golemMesh->m_headBone->m_transform;
	headTransform.m_position += m_golemMesh->m_rootBone->m_transform.m_position;
	return headTransform;
}

void Golem::CreateMeshes()
{
	m_headMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> sphereVerts;
	std::vector<uint> sphereIndices;
	Vertex_PCUTBN::AppendIndexedVertsSphere( sphereVerts, sphereIndices, 1.f );
	m_headMesh->UpdateVertices( sphereVerts );
	m_headMesh->UpdateIndices( sphereIndices );

	m_chestMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> cubeVerts;
	std::vector<uint> cubeIndices;
	Vertex_PCUTBN::AppendIndexedVertsCube( cubeVerts, cubeIndices, 1.f );
	m_chestMesh->UpdateVertices( cubeVerts );
	m_chestMesh->UpdateIndices( cubeIndices );

	m_shoulderMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> shoulderVerts;
	std::vector<uint> shoulderIndices;
	Vertex_PCUTBN::AppendIndexedVertsCube( shoulderVerts, shoulderIndices, 0.5f, Vec3( 1.f, 0.5f, 1.f ) );
	m_shoulderMesh->UpdateVertices( shoulderVerts );
	m_shoulderMesh->UpdateIndices( shoulderIndices );

	m_hipMesh = new GPUMesh( g_theRenderer );
	m_hipMesh->UpdateVertices( shoulderVerts );
	m_hipMesh->UpdateIndices( shoulderIndices );

	m_armMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> armVerts;
	std::vector<uint> armIndices;
	Vertex_PCUTBN::AppendIndexedVertsCube( armVerts, armIndices, 0.5f, Vec3( 1.f, 1.5f, 1.f ) );
	m_armMesh->UpdateVertices( armVerts );
	m_armMesh->UpdateIndices( armIndices );

	m_legMesh = new GPUMesh( g_theRenderer );
	m_legMesh->UpdateVertices( armVerts );
	m_legMesh->UpdateIndices( armIndices );
}

void Golem::CreateSkeleton()
{
	Transform rootTransform;
	Transform headTransform;
	headTransform.m_position.y += 2.f;

	Transform leftShoulderTransform;
	leftShoulderTransform.m_position.x -= 1.75f;
	leftShoulderTransform.m_position.y += 1.f;

	Transform rightShoulderTransform;
	rightShoulderTransform.m_position.x += 1.75f;
	rightShoulderTransform.m_position.y += 1.f;
	
	Transform upperArmTransform;
	upperArmTransform.m_position.y -= 1.25f;
	
	Transform leftHipTransform;
	leftHipTransform.m_position.x -= 0.75f;
	leftHipTransform.m_position.y -= 1.75f;
	
	Transform rightHipTransform;
	rightHipTransform.m_position.x += 0.75f;
	rightHipTransform.m_position.y -= 1.75f;

	Transform elbowTransform;
	elbowTransform.m_position.y -= 0.75f;

	Transform lowerArmTransform;
	lowerArmTransform.m_position.y -= 1.f;


	SkeletalMeshBone* root			= new SkeletalMeshBone( m_chestMesh, nullptr, rootTransform );
	SkeletalMeshBone* head			= new SkeletalMeshBone( m_headMesh, root, headTransform );
	SkeletalMeshBone* leftShoulder	= new SkeletalMeshBone( m_shoulderMesh, root, leftShoulderTransform );
	SkeletalMeshBone* rightShoulder = new SkeletalMeshBone( m_shoulderMesh, root, rightShoulderTransform );
	SkeletalMeshBone* leftArm		= new SkeletalMeshBone( m_armMesh, leftShoulder, upperArmTransform );
	SkeletalMeshBone* rightArm		= new SkeletalMeshBone( m_armMesh, rightShoulder, upperArmTransform );
	SkeletalMeshBone* leftElbow		= new SkeletalMeshBone( nullptr, leftArm, elbowTransform );
	SkeletalMeshBone* rightElbow	= new SkeletalMeshBone( nullptr, rightArm, elbowTransform );
	SkeletalMeshBone* leftLowerArm	= new SkeletalMeshBone( m_armMesh, leftElbow, lowerArmTransform );
	SkeletalMeshBone* rightLowerArm	= new SkeletalMeshBone( m_armMesh, rightElbow, lowerArmTransform );
	SkeletalMeshBone* leftHip		= new SkeletalMeshBone( m_hipMesh, root, leftHipTransform );
	SkeletalMeshBone* rightHip		= new SkeletalMeshBone( m_hipMesh, root, rightHipTransform );
	SkeletalMeshBone* leftLeg		= new SkeletalMeshBone( m_legMesh, leftHip, upperArmTransform );
	SkeletalMeshBone* rightLeg		= new SkeletalMeshBone( m_legMesh, rightHip, upperArmTransform );

	m_golemMesh->m_rootBone = root;
	m_golemMesh->m_headBone = head;

	m_golemMesh->m_rootBone->m_childBones.push_back( head );
	m_golemMesh->m_rootBone->m_childBones.push_back( leftShoulder );
	m_golemMesh->m_rootBone->m_childBones.push_back( rightShoulder );
	m_golemMesh->m_rootBone->m_childBones.push_back( leftHip );
	m_golemMesh->m_rootBone->m_childBones.push_back( rightHip );

	leftShoulder->m_childBones.push_back( leftArm );
	rightShoulder->m_childBones.push_back( rightArm );
	leftArm->m_childBones.push_back( leftElbow );
	rightArm->m_childBones.push_back( rightElbow );
	leftElbow->m_childBones.push_back( leftLowerArm );
	rightElbow->m_childBones.push_back( rightLowerArm );
	leftHip->m_childBones.push_back( leftLeg );
	rightHip->m_childBones.push_back( rightLeg );

	m_leftShoulder = leftShoulder;
	m_rightShoulder = rightShoulder;
	m_leftElbow = leftElbow;
	m_rightElbow = rightElbow;
	m_leftHip = leftHip;
	m_rightHip = rightHip;
}

