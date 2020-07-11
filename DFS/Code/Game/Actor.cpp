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
#include "Game/AudioDefinition.hpp"

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
		m_dodgeTimer.SetSeconds( Clock::GetMaster(), 0.5 );
	}
	else if( m_name == "Boss" )
	{
		m_entityType = ENTITY_TYPE_BOSS;
		m_health = 1000;
		m_isDead = false;
		m_firingTimer.SetSeconds( 0.1 );
		m_bossWaitTimer.SetSeconds( Clock::GetMaster(), 3.0 );
		m_dodgeTimer.SetSeconds( Clock::GetMaster(), 0.5 );
		m_currentBossAttack = CircleOfBullets;
		m_currentBossState = Moving;
	}
	else
	{
		m_entityType = ENTITY_TYPE_NPC_ENEMY;
		m_health = 50;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 1.0 );
		m_dodgeTimer.SetSeconds( Clock::GetMaster(), 0.2 );
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
		m_health = 100;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 0.25 );
		m_dodgeTimer.SetSeconds( Clock::GetMaster(), 0.5 );
	}
	else if( m_name == "Boss" )
	{
		m_entityType = ENTITY_TYPE_BOSS;
		m_health = 1000;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 0.1 );
		m_bossWaitTimer.SetSeconds( Clock::GetMaster(), 3.0 );
		m_dodgeTimer.SetSeconds( Clock::GetMaster(), 0.5 );
		m_currentBossAttack = CircleOfBullets;
		m_currentBossState = Moving;
	}
	else
	{
		m_entityType = ENTITY_TYPE_NPC_ENEMY;
		m_health = 50;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 1.0 );
		m_dodgeTimer.SetSeconds( Clock::GetMaster(), 0.2 );
	}
}

Actor::~Actor()
{
	return;
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
	else if( m_name == "Boss" )
	{
		UpdateBoss( deltaSeconds );
		Entity::Update( deltaSeconds );
	}
	else
	{
		UpdateNPC(deltaSeconds);
		Entity::Update(deltaSeconds);
	}

	if( m_dodgeTimer.HasElapsed() )
	{
		m_isDodging = false;
		
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

	float spriteTime = m_timeSinceCreation;
	if( m_isDodging )
	{
		spriteTime = (float)m_dodgeTimer.GetElapsedSeconds();
	}
	const SpriteDefinition& currentSpriteDef = currentSpriteAnimDef.GetSpriteDefAtTime( spriteTime );
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


// 	g_theRenderer->DrawDisc( m_position, 0.02f, Rgba8::GREEN, Rgba8::RED, 0.02f );
// 	g_theRenderer->DrawDisc( m_position + m_weaponOffset + debugPivot, 0.02f, Rgba8::GREEN, Rgba8::RED, 0.02f );
// 	g_theRenderer->DrawDisc( GetMuzzlePosition(), 0.02f, Rgba8::GREEN, Rgba8::RED, 0.02f );

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
	for( size_t weaponIndex = 0; weaponIndex < m_weapons.size(); weaponIndex++ )
	{
		if( newWeapon == m_weapons[weaponIndex] )
		{
			return;
		}
	}

	float shotsPerSecond = newWeapon->GetShotsPerSecond();
	float fireRate = 1.f / shotsPerSecond;
	m_weapons.push_back( newWeapon );

	if( m_weapons.size() == 1 )
	{
		if( m_entityType != ENTITY_TYPE_BOSS )
		{
			m_firingTimer.SetSeconds( (double)fireRate );
		}
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

WeaponDefinition const* Actor::GetCurrentWeapon() const
{
	return m_weapons[m_currentWeaponIndex];
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
	const KeyButtonState& rightMouseButton = g_theInput->GetMouseButton( RightMouseButton );
	
	Vec2 mousePos = g_theGame->GetMousePositionOnMainCamera();
	Vec2 weaponDirection = mousePos - GetPosition();

	WeaponDefinition const* weaponDef = m_weapons[m_currentWeaponIndex];
	if( weaponDirection.x >= 0.f )
	{
		//m_weaponOffset = weaponDef->GetWeaponOffsetRight();
		m_isWeaponFlipped = true;
	}
	else
	{
		//m_weaponOffset = weaponDef->GetWeaponOffsetLeft();
		m_isWeaponFlipped = false;
	}

	float weaponDirectionDegrees = weaponDirection.GetAngleDegrees();
	bool isMoving = !m_velocity.IsAlmostEqual( Vec2( 0.f, 0.f ) );
	m_weaponOffset = weaponDef->GetWeaponOffset( weaponDirectionDegrees, isMoving );
	if( GetShortestAngularDistance( 112.5f, weaponDirectionDegrees ) < 90.f )
	{
		m_isWeaponInFront = false;
	}
	else
	{
		m_isWeaponInFront = true;
	}
	m_weaponOrientationDegrees = weaponDirection.GetAngleDegrees();



	if( rightMouseButton.WasJustPressed() )
	{
		if( m_dodgeTimer.HasElapsed() )
		{
			m_dodgeTimer.Reset();
			m_isDodging = true;
			m_velocity *= 1.75f;
		}
	}

	if( !m_isDodging )
	{
		if( leftMouseButton.IsPressed() )
		{
			m_isFiring = (bool)m_firingTimer.CheckAndDecrementAll();
			if( m_isFiring )
			{
				float screenShakeIncrement = weaponDef->GetScreenShakeIncremenet();
				g_theGame->AddScreenShake( screenShakeIncrement );
				AudioDefinition* shootSound = m_weapons[m_currentWeaponIndex]->GetAudioDefinition();
				shootSound->StopSound();
				shootSound->PlaySound();
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

}

void Actor::UpdateNPC( float deltaSeconds )
{
	UNUSED( deltaSeconds );

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

void Actor::UpdateBoss( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if( m_currentBossState == Moving )
	{
		m_goalPosition = m_enemyActor->GetPosition();

		Vec2 goalDirection = (m_goalPosition - m_position).GetNormalized();
		m_orientationDegrees = goalDirection.GetAngleDegrees();
		m_weaponOrientationDegrees = m_orientationDegrees;
		m_velocity = goalDirection * m_actorDef->m_speed;


		if( m_bossWaitTimer.HasElapsed() )
		{
			m_currentBossState = Attacking;
			m_bossWaitTimer.Reset();
			m_firingTimer.Reset();
		}
	}
	else if( m_currentBossState == Attacking )
	{
		m_velocity *= 0.f;

		if( m_currentBossAttack == CircleOfBullets )
		{
			if( (bool)m_firingTimer.CheckAndDecrementAll() )
			{
				m_weaponOrientationDegrees += 20.f;
				m_isFiring = true;
			}
		}
		else if( m_currentBossAttack == RandomFire )
		{
			m_goalPosition = m_enemyActor->GetPosition();

			Vec2 goalDirection = (m_goalPosition - m_position).GetNormalized();
			m_orientationDegrees = goalDirection.GetAngleDegrees();
			m_weaponOrientationDegrees = m_orientationDegrees;

			if( (bool)m_firingTimer.CheckAndDecrementAll() )
			{
				m_weaponOrientationDegrees += g_theGame->m_rand.RollRandomFloatInRange(-30.f, 30.f );
				m_isFiring = true;
			}
		}


		if( m_bossWaitTimer.HasElapsed() )
		{
			m_currentBossState = Moving;

			if( m_currentBossAttack == CircleOfBullets )
			{
				m_firingTimer.SetSeconds( 0.1 );
				m_currentBossAttack = RandomFire;
			}
			else if( m_currentBossAttack == RandomFire )
			{
				m_firingTimer.SetSeconds( 0.075 );
				m_currentBossAttack = CircleOfBullets;
			}
			m_bossWaitTimer.Reset();
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

	if( m_isDodging )
	{
		float velocityOrientation = m_velocity.GetAngleDegrees();

		if( GetShortestAngularDistance( 0.f, velocityOrientation ) < 45.f )
		{
			currentAnimationName = "DodgeEast";
		}
		else if( GetShortestAngularDistance( 90.f, velocityOrientation ) < 45.f )
		{
			currentAnimationName = "DodgeNorth";
		}
		else if( GetShortestAngularDistance( 180.f, velocityOrientation ) < 45.f )
		{
			currentAnimationName = "DodgeWest";
		}
		else
		{
			currentAnimationName = "DodgeSouth";
		}
	}
	else if( m_velocity.GetLength() == 0.f )
	{
		if( GetShortestAngularDistance( 0.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "IdleEast";
		}
		else if( GetShortestAngularDistance( 45.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "IdleNorthEast";
		}
		else if( GetShortestAngularDistance( 90.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "IdleNorth";
		}
		else if( GetShortestAngularDistance( 135.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "IdleNorthWest";
		}
		else if( GetShortestAngularDistance( 180.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "IdleWest";
		}
		else if( GetShortestAngularDistance( 225.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "IdleSouthWest";
		}
		else if( GetShortestAngularDistance( 270.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "IdleSouth";
		}
		else
		{
			currentAnimationName = "IdleSouthEast";
		}
	}
	else
	{
		if( GetShortestAngularDistance( 0.f, m_weaponOrientationDegrees ) < 45.f )
		{
			currentAnimationName = "MoveEast";
		}
		else if( GetShortestAngularDistance( 90.f, m_weaponOrientationDegrees ) < 45.f )
		{
			currentAnimationName = "MoveNorth";
		}
		else if( GetShortestAngularDistance( 180.f, m_weaponOrientationDegrees ) < 45.f )
		{
			currentAnimationName = "MoveWest";
		}
		else
		{
			currentAnimationName = "MoveSouth";
		}
	}

	return currentAnimationName;
}

