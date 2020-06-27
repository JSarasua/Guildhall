#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Time/Clock.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Game/BulletDefinition.hpp"
#include "Engine/Renderer/DebugRender.hpp"

Actor::Actor( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, ActorDefinition const* actorDef ) :
	m_actorDef(actorDef),
	Entity(initialPosition, initialVelocity, initialOrientationDegrees, initialAngularVelocity)
{
	m_name = m_actorDef->m_name;
	m_canWalk = m_actorDef->m_canWalk;
	m_canSwim = m_actorDef->m_canSwim;
	m_canFly = m_actorDef->m_canFly;

	m_physicsRadius = m_actorDef->m_physicsRadius;

	if( m_name == "Player" )
	{
		m_entityType = ENTITY_TYPE_PLAYER;
		m_health = 100;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 0.25 );
	}
	else
	{
		m_entityType = ENTITY_TYPE_NPC_ENEMY;
		m_health = 50;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 1.0 );
	}
}

Actor::Actor( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, ActorDefinition const* actorDef, PlayerController playerController ):
	m_actorDef( actorDef ),
	m_playerController( playerController ),
	Entity( initialPosition, initialVelocity, initialOrientationDegrees, initialAngularVelocity )
{
	m_name = m_actorDef->m_name;
	m_canWalk = m_actorDef->m_canWalk;
	m_canSwim = m_actorDef->m_canSwim;
	m_canFly = m_actorDef->m_canFly;

	m_physicsRadius = m_actorDef->m_physicsRadius;

	if( m_name == "Player" )
	{
		m_entityType = ENTITY_TYPE_PLAYER;
		m_health = 50;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 0.25 );
	}
	else
	{
		m_entityType = ENTITY_TYPE_NPC_ENEMY;
		m_health = 50;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 1.0 );
	}
}

void Actor::Startup()
{

}

void Actor::Update( float deltaSeconds )
{
	if( !IsAlive() )
	{
		return;
	}
	m_timeSinceCreation += deltaSeconds;

	if( m_name == "Player" )
	{
		UpdateFromJoystick();
		UpdateFromKeyboard();
		m_position = TransformPosition2D(m_position, 1.f, 0.f, m_velocity * deltaSeconds);
	}
	else
	{
		UpdateNPC(deltaSeconds);
		Entity::Update(deltaSeconds);
	}

}

void Actor::Render() const
{
	if( !IsAlive() )
	{
		return;
	}

	const SpriteAnimSet& currentSpriteAnimSet = *m_actorDef->m_actorAnimations;

	std::string animationName = GetCurrentAnimationName();
	//Do logic on direction to find out what AnimDef to use

	const SpriteAnimDefinition& currentSpriteAnimDef = *currentSpriteAnimSet.GetSpriteAnimDefinition(animationName);

	const SpriteDefinition& currentSpriteDef = currentSpriteAnimDef.GetSpriteDefAtTime( m_timeSinceCreation );
	AABB2 actorUVs;
	currentSpriteDef.GetUVs( actorUVs.mins, actorUVs.maxs );

	AABB2 actorBounds = m_actorDef->m_drawBounds;
	actorBounds.Translate(m_position);
	const Rgba8& actorTint = m_actorDef->m_tint;
	//const AABB2& actorUVs = m_actorDef->m_spriteUVs;
	const Texture& actorTexture = g_actorSpriteSheet->GetTexture();

	if( !m_isWeaponInFront )
	{
		RenderWeapon();
	}

	g_theRenderer->BindTexture( &actorTexture );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->DrawRotatedAABB2Filled(actorBounds,actorTint,actorUVs.mins,actorUVs.maxs,0.f);

	if( m_isWeaponInFront )
	{
		RenderWeapon();
	}
}

void Actor::RenderWeapon() const
{
	WeaponDefinition const* currentWeapon = m_weapons[m_currentWeaponIndex];
	Vec2 weaponCenter = m_weaponOffset + m_position;
	AABB2 weaponUVs;
	currentWeapon->GetWeaponSpriteDef()->GetUVs( weaponUVs.mins, weaponUVs.maxs );
	Texture const& weaponTexture = currentWeapon->GetWeaponSpriteDef()->GetTexture();
	Rgba8 const& weaponTint = currentWeapon->GetWeaponTint();
	
	if( m_isWeaponFlipped )
	{
		float weaponUVMinsX = weaponUVs.mins.x;
		weaponUVs.mins.x = weaponUVs.maxs.x;
		weaponUVs.maxs.x = weaponUVMinsX;
	}
	else
	{
		Vec2 uvMins = weaponUVs.mins;
		weaponUVs.mins = weaponUVs.maxs;
		weaponUVs.maxs = uvMins;
	}


	Vec2 pivotPoint = currentWeapon->GetTriggerPositionUV();
	AABB2 weaponBounds = currentWeapon->GetWeaponDrawBounds();
	Vec2 debugPivot = weaponBounds.GetPointAtUV( pivotPoint );
	weaponBounds.Translate( weaponCenter );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->BindTexture( &weaponTexture );
	if( m_name == "Player" )
	{
		g_theRenderer->DrawRotatedAABB2Filled( weaponBounds, weaponTint, weaponUVs.mins, weaponUVs.maxs, Entity::GetWeaponOrientationDegrees(), pivotPoint );
	}
	else
	{
		g_theRenderer->DrawRotatedAABB2Filled( weaponBounds, weaponTint, weaponUVs.mins, weaponUVs.maxs, m_orientationDegrees, pivotPoint );
	}


	g_theRenderer->DrawDisc( m_position, 0.02f, Rgba8::GREEN, Rgba8::RED, 0.02f );
	g_theRenderer->DrawDisc( m_position + m_weaponOffset + debugPivot, 0.02f, Rgba8::GREEN, Rgba8::RED, 0.02f );
	g_theRenderer->DrawDisc( GetMuzzlePosition(), 0.02f, Rgba8::GREEN, Rgba8::RED, 0.02f );

	//DebugAddWorldPoint( Vec3( pivotPoint ), Rgba8::RED, 0.f, DEBUG_RENDER_ALWAYS );
}

void Actor::SetEnemy( Entity* enemy )
{
	m_enemyActor = enemy;
}

void Actor::IncrementActiveWeapon()
{
	m_currentWeaponIndex++;
	if( m_currentWeaponIndex >= m_weapons.size() )
	{
		m_currentWeaponIndex = 0;
	}

	float shotsPerSecond = m_weapons[m_currentWeaponIndex]->GetShotsPerSecond();
	float fireRate = 1.f / shotsPerSecond;
	m_firingTimer.SetSeconds( (double)fireRate );
}

void Actor::DecrementActiveWeapon()
{
	m_currentWeaponIndex--;
	if( m_currentWeaponIndex < 0 )
	{
		m_currentWeaponIndex = (int)m_weapons.size() - 1;
	}

	float shotsPerSecond = m_weapons[m_currentWeaponIndex]->GetShotsPerSecond();
	float fireRate = 1.f / shotsPerSecond;
	m_firingTimer.SetSeconds( (double)fireRate );
}

void Actor::AddWeapon( WeaponDefinition const* newWeapon )
{
	float shotsPerSecond = newWeapon->GetShotsPerSecond();
	float fireRate = 1.f / shotsPerSecond;
	m_weapons.push_back( newWeapon );

	if( m_weapons.size() == 1 )
	{
		m_firingTimer.SetSeconds( (double)fireRate );
	}
}

int Actor::GetBulletsPerShot() const
{
	return m_weapons[m_currentWeaponIndex]->GetBulletsPerShot();
}

float Actor::GetBulletSpreadDegrees() const
{
	return m_weapons[m_currentWeaponIndex]->GetBulletSpreadDegrees();
}

Vec2 Actor::GetMuzzlePosition() const
{
	AABB2 drawBounds = m_weapons[m_currentWeaponIndex]->GetWeaponDrawBounds();
	Vec2 pivot = drawBounds.GetPointAtUV( m_weapons[m_currentWeaponIndex]->GetTriggerPositionUV() );

	return m_position + m_weaponOffset + pivot + m_weapons[m_currentWeaponIndex]->GetMuzzlePosition( m_weaponOrientationDegrees );
}

BulletDefinition const* Actor::GetBulletDefinition() const
{
	return m_weapons[m_currentWeaponIndex]->GetBulletDefinition();
}

void Actor::UpdateFromJoystick()
{
	const XboxController& controller = g_theInput->GetXboxController( m_playerController );
	if( controller.IsConnected() )
	{
		const float& joystickOrientation = controller.GetLeftJoystick().GetAngleDegrees();
		const float& joystickMagnitude = controller.GetLeftJoystick().GetMagnitude();

		const float& actorSpeed = m_actorDef->m_speed;
		//const float& actorTurnSpeed = m_actorDef->m_turnSpeed;

		if( joystickMagnitude != 0 )
		{
			m_orientationDegrees = joystickOrientation;
			//m_orientationDegrees = GetTurnedToward( m_orientationDegrees, joystickOrientation, actorTurnSpeed * deltaSeconds );
			m_velocity  = Vec2::MakeFromPolarDegrees( m_orientationDegrees, joystickMagnitude * actorSpeed );
		}
		else if( controller.GetLeftJoystick().GetMagnitude() == 0 )
		{
			m_velocity *= 0.f;
		}
	}
}

void Actor::UpdateFromKeyboard()
{
	const float& actorSpeed = m_actorDef->m_speed;

	const KeyButtonState& leftArrow = g_theInput->GetKeyStates( 0x25 );
	const KeyButtonState& upArrow = g_theInput->GetKeyStates( 0x26 );
	const KeyButtonState& rightArrow = g_theInput->GetKeyStates( 0x27 );
	const KeyButtonState& downArrow = g_theInput->GetKeyStates( 0x28 );

	const KeyButtonState& sKey = g_theInput->GetKeyStates( 'S' );
	const KeyButtonState& eKey = g_theInput->GetKeyStates( 'E' );
	const KeyButtonState& fKey = g_theInput->GetKeyStates( 'F' );
	const KeyButtonState& dKey = g_theInput->GetKeyStates( 'D' );
	const KeyButtonState& wKey = g_theInput->GetKeyStates( 'W' );
	const KeyButtonState& rKey = g_theInput->GetKeyStates( 'R' );
	const KeyButtonState& leftMouseButton = g_theInput->GetMouseButton( LeftMouseButton );
	
	Vec2 mousePos = g_theGame->GetMousePositionOnMainCamera();
	Vec2 weaponDirection = mousePos - GetPosition();

	WeaponDefinition const* weaponDef = m_weapons[m_currentWeaponIndex];
	if( weaponDirection.x >= 0.f )
	{
		m_weaponOffset = weaponDef->GetWeaponOffsetRight();
		m_isWeaponFlipped = true;
	}
	else
	{
		m_weaponOffset = weaponDef->GetWeaponOffsetLeft();
		m_isWeaponFlipped = false;
	}


	float weaponDirectionDegrees = weaponDirection.GetAngleDegrees();
	if( GetShortestAngularDistance( 90.f, weaponDirectionDegrees ) < 45.f )
	{
		m_isWeaponInFront = false;
	}
	else
	{
		m_isWeaponInFront = true;
	}
	m_weaponOrientationDegrees = weaponDirection.GetAngleDegrees();

	if( leftMouseButton.IsPressed() )
	{
		m_isFiring = (bool)m_firingTimer.CheckAndDecrementAll();
		if( m_isFiring )
		{
			float screenShakeIncrement = weaponDef->GetScreenShakeIncremenet();
			g_theGame->AddScreenShake( screenShakeIncrement );
		}
	}

	if( leftArrow.IsPressed() && !rightArrow.IsPressed() )
	{
		m_velocity.x = -actorSpeed;
	}
	else if( rightArrow.IsPressed() && !leftArrow.IsPressed() )
	{
		m_velocity.x = actorSpeed;
	}
	else if( leftArrow.IsPressed() && rightArrow.IsPressed() )
	{
		m_velocity.x = 0.f;
	}


	if( downArrow.IsPressed() && !upArrow.IsPressed() )
	{
		m_velocity.y = -actorSpeed;
	}
	else if( upArrow.IsPressed() && !downArrow.IsPressed() )
	{
		m_velocity.y = actorSpeed;
	}
	else if( upArrow.IsPressed() && downArrow.IsPressed() )
	{
		m_velocity.y = 0.f;
	}

	if( leftArrow.WasJustReleased() || rightArrow.WasJustReleased() )
	{
		m_velocity.x = 0.f;
	}
	if( upArrow.WasJustReleased() || downArrow.WasJustReleased() )
	{
		m_velocity.y = 0.f;
	}



	if( sKey.IsPressed() && !fKey.IsPressed() )
	{
		m_velocity.x = -actorSpeed;
	}
	else if( fKey.IsPressed() && !sKey.IsPressed() )
	{
		m_velocity.x = actorSpeed;
	}
	else if( sKey.IsPressed() && fKey.IsPressed() )
	{
		m_velocity.x = 0.f;
	}

	if( dKey.IsPressed() && !eKey.IsPressed() )
	{
		m_velocity.y = -actorSpeed;
	}
	else if( eKey.IsPressed() && !dKey.IsPressed() )
	{
		m_velocity.y = actorSpeed;
	}
	else if( eKey.IsPressed() && dKey.IsPressed() )
	{
		m_velocity.y = 0.f;
	}

	if( sKey.WasJustReleased() || fKey.WasJustReleased() )
	{
		m_velocity.x = 0.f;
	}
	if( eKey.WasJustReleased() || dKey.WasJustReleased() )
	{
		m_velocity.y = 0.f;
	}

	if( wKey.WasJustPressed() )
	{
		DecrementActiveWeapon();
	}

	if( rKey.WasJustPressed() )
	{
		IncrementActiveWeapon();
	}
}

void Actor::UpdateNPC( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	//static float timeUntilNextGoalPosition = 2.f;

// 
// 	if( m_goalPosition == Vec2( -1.f, -1.f ) ) //Unitialized
// 	{
// 		GetNewGoalPosition();
// 	}
// 	if( GetDistance2D( m_goalPosition, m_position ) < 0.1f )	//Reached goal location
// 	{
// 		m_goalPosition = m_position;
// 		//GetNewGoalPosition();
// 	}
// 	if( m_timeUntilNextGoalPosition < 0.f )	//
// 	{
// 		const FloatRange& actorWaitingTime = m_actorDef->m_waitingTime;
// 		m_timeUntilNextGoalPosition = actorWaitingTime.GetRandomInRange(g_theGame->m_rand);
// 		GetNewGoalPosition();
// 	}
// 	else if( m_hasCollided )
// 	{
// 		m_goalPosition = m_position;
// 		m_hasCollided = false;
// 	}
// 
// 	m_timeUntilNextGoalPosition -= deltaSeconds;

	if( m_enemyActor )
	{
		m_goalPosition = m_enemyActor->GetPosition();
	}
	
	float goalDistance = GetDistance2D( m_goalPosition, m_position );
	if( goalDistance < 5.f && (bool)m_firingTimer.CheckAndDecrementAll() )
	{
		m_isFiring = true;
	}

	if( m_goalPosition == m_position )
	{
		m_velocity *= 0.f;
	}
	else
	{
		Vec2 goalDirection = (m_goalPosition - m_position).GetNormalized();
		m_orientationDegrees = goalDirection.GetAngleDegrees();
		m_weaponOrientationDegrees = m_orientationDegrees;
		m_velocity = goalDirection * m_actorDef->m_speed;

		WeaponDefinition const* weaponDef = m_weapons[m_currentWeaponIndex];
		if( goalDirection.x >= 0.f )
		{
			m_weaponOffset = weaponDef->GetWeaponOffsetRight();
			m_isWeaponFlipped = true;
		}
		else
		{
			m_weaponOffset = weaponDef->GetWeaponOffsetLeft();
			m_isWeaponFlipped = false;
		}

		if( goalDirection.y >= 0.f )
		{
			m_isWeaponInFront = false;
		}
		else
		{
			m_isWeaponInFront = true;
		}
	}

}

void Actor::GetNewGoalPosition()
{
	Vec2 deltaPosition;
	deltaPosition.x = g_theGame->m_rand.RollRandomFloatInRange( -10.f, 10.f );
	deltaPosition.y = g_theGame->m_rand.RollRandomFloatInRange( -10.f, 10.f );

	m_goalPosition = m_position + deltaPosition;
}

std::string Actor::GetCurrentAnimationName() const
{
	std::string currentAnimationName;

// 	if( m_velocity.GetLength() == 0.f )
// 	{
// 		currentAnimationName = "Idle";
// 	}
// 	else if( GetShortestAngularDistance(0.f, m_weaponOrientationDegrees) < 45.f )
	if( GetShortestAngularDistance(0.f, m_weaponOrientationDegrees) < 45.f )
	{
		currentAnimationName = "MoveEast";
	}
	else if( GetShortestAngularDistance(90.f, m_weaponOrientationDegrees) < 45.f )
	{
		currentAnimationName = "MoveNorth";
	}
	else if( GetShortestAngularDistance(180.f, m_weaponOrientationDegrees) < 45.f )
	{
		currentAnimationName = "MoveWest";
	}
	else if( GetShortestAngularDistance(270.f, m_weaponOrientationDegrees) < 45.f )
	{
		currentAnimationName = "MoveSouth";
	}
	else
	{
		currentAnimationName = "Idle";
	}

	return currentAnimationName;
}

