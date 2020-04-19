#include "Engine/Renderer/SkeletalMeshBone.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"

SkeletalMeshBone::SkeletalMeshBone()
{

}

SkeletalMeshBone::SkeletalMeshBone( GPUMesh* mesh, SkeletalMeshBone* parentBone, Transform localTransform )
	: m_mesh( mesh ),
	m_parentBone( parentBone ),
	m_transform( localTransform )
{

}

SkeletalMeshBone::~SkeletalMeshBone()
{

}

Mat44 SkeletalMeshBone::GetRelativeModelMatrix() const
{
	Mat44 parentMatrix;
	Mat44 myLocalMatrix = m_transform.ToMatrix();
	if( nullptr != m_parentBone )
	{
		parentMatrix = m_parentBone->GetRelativeModelMatrix();
/*		myLocalMatrix.TransformBy( parentMatrix );*/
	}
	parentMatrix.TransformBy( myLocalMatrix );

	//return myLocalMatrix;
	return parentMatrix;
}

void SkeletalMeshBone::Render()
{
	if( nullptr != m_mesh )
	{
		RenderContext* context = m_mesh->m_renderContext;
		if( nullptr != context )
		{
			Mat44 modelMatrix = GetRelativeModelMatrix();
			context->SetModelMatrix( modelMatrix );
			context->DrawMesh( m_mesh );
		}
	}

	for( size_t boneIndex = 0; boneIndex < m_childBones.size(); boneIndex++ )
	{
		if( nullptr != m_childBones[boneIndex] )
		{
			m_childBones[boneIndex]->Render();
		}
	}
}

