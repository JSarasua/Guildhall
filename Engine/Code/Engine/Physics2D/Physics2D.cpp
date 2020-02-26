#include "Engine/Physics2D/Physics2D.hpp"
#include "Engine/Physics2D/Collision2D.hpp"
#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Math/MathUtils.hpp"

void Physics2D::BeginFrame()
{

}

void Physics2D::Update( float deltaSeconds )
{
	ApplyEffectors();
	MoveRigidbodies( deltaSeconds );
	DetectCollisions();
	ResolveCollisions();
	CleanupDestroyedObjects();
}

void Physics2D::ApplyEffectors()
{
	for( size_t rigidBodyIndex = 0; rigidBodyIndex < m_rigidBodies.size(); rigidBodyIndex++ )
	{
		Rigidbody2D* rb = m_rigidBodies[rigidBodyIndex];
		
		if( nullptr == rb )
		{
			continue;
		}

		Vec2 gravity( 0.f, -1.f );
		gravity *= m_gravity;
		gravity *= rb->GetMass();
		rb->AddForce( gravity );
	}
}

void Physics2D::MoveRigidbodies( float deltaSeconds )
{
	for( size_t rigidBodyIndex = 0; rigidBodyIndex < m_rigidBodies.size(); rigidBodyIndex++ )
	{
		if( nullptr == m_rigidBodies[rigidBodyIndex] )
		{
			continue;
		}
		m_rigidBodies[rigidBodyIndex]->Update( deltaSeconds );
	}
}

void Physics2D::DetectCollisions()
{
	for( size_t colliderIndex = 0; colliderIndex < m_colliders.size(); colliderIndex++ )
	{
		if( nullptr == m_colliders[colliderIndex] )
		{
			continue;
		}
		if( !m_colliders[colliderIndex]->m_rigidbody->IsEnabled() )
		{
			continue;
		}
		for( size_t otherColliderIndex = colliderIndex + 1; otherColliderIndex < m_colliders.size(); otherColliderIndex++ )
		{
			if( nullptr == m_colliders[otherColliderIndex] )
			{
				continue;
			}
			if( !m_colliders[otherColliderIndex]->m_rigidbody->IsEnabled() )
			{
				continue;
			}

			Collider2D* myCollider = m_colliders[colliderIndex];
			Collider2D* otherCollider = m_colliders[otherColliderIndex];

			eSimulationMode mySimMode = myCollider->GetSimulationMode();
			eSimulationMode theirSimMode = otherCollider->GetSimulationMode();

			if( mySimMode == STATIC && theirSimMode == STATIC )
			{
				continue;
			}

			Manifold2D manifold;
			bool intersects = myCollider->GetManifold(otherCollider, &manifold );
			if( !intersects )
			{
				continue;
			}
			Collision2D collision;

			if( myCollider->m_type == COLLIDER2D_POLYGON && otherCollider->m_type == COLLIDER2D_DISC )
			{
				collision.me = otherCollider;
				collision.them = myCollider;
			}
			else
			{
				collision.me = myCollider;
				collision.them = otherCollider;
			}
// 				collision.me = myCollider;
// 				collision.them = otherCollider;
			collision.manifold = manifold;


			m_collisions.push_back(collision);
		}
	}
}

void Physics2D::ResolveCollisions()
{
	for( size_t collisionIndex = 0; collisionIndex < m_collisions.size(); collisionIndex++ )
	{
		ResolveCollision(m_collisions[collisionIndex]);
	}

	m_collisions.clear();
}

void Physics2D::ResolveCollision( Collision2D const& collision )
{
	Manifold2D manifold = collision.manifold;
	Vec2 normal = manifold.normal;
	Rigidbody2D* myRigidbody = collision.me->m_rigidbody;
	Rigidbody2D* theirRigidbody = collision.them->m_rigidbody;

	if( nullptr == myRigidbody || nullptr == theirRigidbody )
	{
		return;
	}


	float penetration = manifold.penetration;
	float myMassRatio = GetMassRatio( myRigidbody, theirRigidbody );
	float theirMassRatio = GetMassRatio( theirRigidbody, myRigidbody );

	Vec2 pushMe = normal * myMassRatio * penetration;
	Vec2 pushThem = -normal * theirMassRatio * penetration;

	collision.me->Move( pushMe );
	collision.them->Move( pushThem );

	/************************************************************************/
	/* Apply Impulses                                                       */
	/************************************************************************/
	float myMass = myRigidbody->GetMass();
	float theirMass = theirRigidbody->GetMass();
	Vec2 myVelocity = myRigidbody->GetVelocity();
	Vec2 theirVelocity = theirRigidbody->GetVelocity();

	float combinedRestituion = collision.me->GetBounceWith( collision.them );

	//Get Impulse direction


	
	eSimulationMode mySimMode = myRigidbody->GetSimulationMode();
	eSimulationMode theirSimMode = theirRigidbody->GetSimulationMode();

	if( mySimMode == DYNAMIC && (theirSimMode == KINEMATIC || theirSimMode == STATIC) )
	{
		Vec2 impulse = myMass * (1.f + combinedRestituion) * (theirVelocity - myVelocity);
		impulse = GetProjectedOnto2D( impulse, normal );
		myRigidbody->ApplyImpulseAt( Vec2(0.f, 0.f), impulse);
	}
	else if( (mySimMode == KINEMATIC || mySimMode == STATIC) && theirSimMode == DYNAMIC )
	{
		Vec2 impulse = theirMass * (1.f + combinedRestituion) * (theirVelocity - myVelocity);
		impulse = GetProjectedOnto2D( impulse, normal );
		theirRigidbody->ApplyImpulseAt( Vec2(0.f, 0.f), -impulse );
	}
	else
	{
		Vec2 impulse = ((myMass * theirMass)/(myMass + theirMass)) * (1.f + combinedRestituion) * (theirVelocity - myVelocity);
		impulse = GetProjectedOnto2D( impulse, normal );
		myRigidbody->ApplyImpulseAt( Vec2( 0.f, 0.f ), impulse );
		theirRigidbody->ApplyImpulseAt( Vec2( 0.f, 0.f ), -impulse );
	}



}

void Physics2D::CleanupDestroyedObjects()
{
	for( int colliderIndex = 0; colliderIndex < m_colliders.size(); colliderIndex++ )
	{
		if( nullptr == m_colliders[colliderIndex] )
		{
			continue;
		}

		bool isGarbage = m_colliders[colliderIndex]->m_isGarbage;
		if( isGarbage )
		{
			delete m_colliders[colliderIndex];
			m_colliders[colliderIndex] = nullptr;
		}
	}

	for( int rigidBodyIndex = 0; rigidBodyIndex < m_rigidBodies.size(); rigidBodyIndex++ )
	{
		if( nullptr == m_colliders[rigidBodyIndex] )
		{
			continue;
		}

		bool isGarbage = m_rigidBodies[rigidBodyIndex]->m_isGarbage;
		if( isGarbage )
		{
			delete m_rigidBodies[rigidBodyIndex];
			m_rigidBodies[rigidBodyIndex] = nullptr;
		}
	}
}

void Physics2D::EndFrame()
{
}


float Physics2D::GetMassRatio( Rigidbody2D* me, Rigidbody2D* them ) const
{
	float myMass = me->GetMass();
	float theirMass = them->GetMass();

	eSimulationMode mySimMode = me->GetSimulationMode();
	eSimulationMode theirSimMode = them->GetSimulationMode();
	if( mySimMode == STATIC )
	{
		return 0.f;
	}
	else if( theirSimMode == STATIC )
	{
		return 1.f;
	}
	else if( mySimMode == DYNAMIC && theirSimMode == KINEMATIC )
	{
		return 1.f;
	}
	else if( mySimMode == KINEMATIC && theirSimMode == DYNAMIC )
	{
		return 0.f;
	}
	else
	{
		return theirMass/(myMass + theirMass);
	}
}

Rigidbody2D* Physics2D::CreateRigidBody()
{
	Rigidbody2D* rb = new Rigidbody2D();
	m_rigidBodies.push_back(rb);
	return rb;
}

void Physics2D::DestroyRigidBody( Rigidbody2D* rb )
{
	rb->m_isGarbage = true;

	rb->m_collider->m_isGarbage = true;;
	rb->m_collider = nullptr;
	
}

DiscCollider2D* Physics2D::CreateDiscCollider( Vec2 localPosition, float radius )
{
	DiscCollider2D* dc = new DiscCollider2D(localPosition, Vec2(0.f, 0.f), radius );
	m_colliders.push_back(dc);
	return dc;
}

PolygonCollider2D* Physics2D::CreatePolygonCollider( Polygon2D const& poly, Vec2 localPosition )
{
	Vec2 worldPosition = poly.GetCenterOfMass();
	PolygonCollider2D* pc = new PolygonCollider2D(localPosition, worldPosition, poly );
	m_colliders.push_back( pc );
	return pc;
}

void Physics2D::DestroyCollider( Collider2D* collider )
{
	collider->m_isGarbage = true;
}

void Physics2D::SetSceneGravity( float newGravity )
{
	m_gravity = newGravity;
}

float Physics2D::GetSceneGravity() const
{
	return m_gravity;
}

