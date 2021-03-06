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
		m_health = PLAYER_HEALTH;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 0.25 );
		m_dodgeTimer.SetSeconds( Clock::GetMaster(), 0.5 );
		m_playerInvulnerabilityTimer.SetSeconds( Clock::GetMaster(), 0.5 );
	}
	else if( m_name == "Boss" )
	{
		m_entityType = ENTITY_TYPE_BOSS;
		m_health = BOSS_HEALTH;
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
		m_health = ENEMY_HEALTH;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 1.0 );
		m_dodgeTimer.SetSeconds( Clock::GetMaster(), 0.2 );
		m_aiCooldownTimer.SetSeconds( Clock::GetMaster(), 1.0 );
		m_aiStartupTimer.SetSeconds( Clock::GetMaster(), 1.0 );
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
		m_health = PLAYER_HEALTH;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 0.25 );
		m_dodgeTimer.SetSeconds( Clock::GetMaster(), 0.5 );
		m_playerInvulnerabilityTimer.SetSeconds( Clock::GetMaster(), 0.5 );
	}
	else if( m_name == "Boss" )
	{
		m_entityType = ENTITY_TYPE_BOSS;
		m_health = BOSS_HEALTH;
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
		m_health = ENEMY_HEALTH;
		m_isDead = false;
		m_firingTimer.SetSeconds( Clock::GetMaster(), 1.0 );
		m_dodgeTimer.SetSeconds( Clock::GetMaster(), 0.2 );
		m_aiCooldownTimer.SetSeconds( Clock::GetMaster(), 1.0 );
		m_aiStartupTimer.SetSeconds( Clock::GetMaster(), 1.0 );
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
		if( m_playerInvulnerabilityTimer.HasElapsed() )
		{
			m_didJustTookDamage = false;
		}
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
	Rgba8 actorTint = m_actorDef->m_tint;
	const Texture& actorTexture = g_actorSpriteSheet->GetTexture();

	if( !m_isWeaponInFront && !m_isDodging && !m_isDead )
	{
		RenderWeapon();
	}

	//Uncomment if flipping actor
// 	if( !m_isWeaponFlipped )
// 	{
// 		float minX = actorUVs.mins.x;
// 		actorUVs.mins.x = actorUVs.maxs.x;
// 		actorUVs.maxs.x = minX;
// 	}
	if( m_didJustTookDamage )
	{
		 actorTint.a =(unsigned char)( 128.f + 128.f * CosDegrees( 360.f * (float)m_playerInvulnerabilityTimer.GetElapsedSeconds() ) );
	}

	g_theRenderer->BindTexture( &actorTexture );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->DrawRotatedAABB2Filled(actorBounds,actorTint,actorUVs.mins,actorUVs.maxs,0.f);

	if( m_isWeaponInFront && !m_isDodging && !m_isDead )
	{
		RenderWeapon();
	}
}

void Actor::LoseHealth( int damage )
{
	if( m_name == "Player" )
	{
		if( m_playerInvulnerabilityTimer.HasElapsed() )
		{
			m_didJustTookDamage = true;
			Entity::LoseHealth( damage );
			g_theGame->AddScreenShake( 1.f );
			m_playerInvulnerabilityTimer.Reset();
		}
	}
	else
	{
		Entity::LoseHealth( damage );
	}

}

void Actor::RenderWeapon() const
{
	WeaponDefinition const* currentWeapon = m_weapons[m_currentWeaponIndex];
	Vec2 weaponCenter = m_weaponOffset + m_position;
	float spriteTime = m_timeSinceCreation;
	if( !m_velocity.IsAlmostEqual( Vec2( 0.f, 0.f ) ) )
	{
		float weaponBounceX = 0.025f * SinDegrees( spriteTime * 360.f * 2.f + 45.f );
		float weaponBounceY = 0.01f * SinDegrees( spriteTime * 360.f * 2.f + 135.f );
		Vec2 weaponBounce = Vec2( weaponBounceX, weaponBounceY );
		weaponBounce.RotateDegrees( m_weaponOrientationDegrees );
		weaponCenter += weaponBounce;
	}
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

	float weaponOrientation = Entity::GetWeaponOrientationDegrees();


	Vec2 pivotPoint = currentWeapon->GetTriggerPositionUV();
	AABB2 weaponBounds = currentWeapon->GetWeaponDrawBounds();
	Vec2 debugPivot = weaponBounds.GetPointAtUV( pivotPoint );
	weaponBounds.Translate( weaponCenter );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->BindTexture( &weaponTexture );
	if( m_name == "Player" )
	{
		g_theRenderer->DrawRotatedAABB2Filled( weaponBounds, weaponTint, weaponUVs.mins, weaponUVs.maxs, weaponOrientation, pivotPoint );
	}
	else
	{
		g_theRenderer->DrawRotatedAABB2Filled( weaponBounds, weaponTint, weaponUVs.mins, weaponUVs.maxs, m_orientationDegrees, pivotPoint );
	}

//Debug drawing
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
	if( m_weapons.size() > 1 )
	{
		m_weapons[m_currentWeaponIndex]->GetAudioDefinition()->StopSound();
	}

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
	if( m_weapons.size() > 1 )
	{
		m_weapons[m_currentWeaponIndex]->GetAudioDefinition()->StopSound();
	}

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
	const KeyButtonState& spaceKey = g_theInput->GetKeyStates( SPACE_KEY );
	const KeyButtonState& leftMouseButton = g_theInput->GetMouseButton( LeftMouseButton );
	const KeyButtonState& rightMouseButton = g_theInput->GetMouseButton( RightMouseButton );
	float deltaMouseWheelScroll = g_theInput->GetDeltaMouseWheelScroll();
	
	Vec2 mousePos = g_theGame->GetMousePositionOnMainCamera();
	Vec2 weaponDirection = mousePos - GetPosition();

	WeaponDefinition const* weaponDef = m_weapons[m_currentWeaponIndex];
	std::string weaponName = weaponDef->GetWeaponName();
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



	if( rightMouseButton.WasJustPressed() || spaceKey.WasJustPressed() )
	{
		if( m_dodgeTimer.HasElapsed() )
		{
			m_dodgeTimer.Reset();
			m_isDodging = true;
			m_weapons[m_currentWeaponIndex]->GetAudioDefinition()->StopSound();
		}
	}

	if( !m_isDodging )
	{
		if( leftMouseButton.IsPressed() )
		{
			m_isFiring = m_firingTimer.HasElapsed();
			if( m_isFiring )
			{
				float screenShakeIncrement = weaponDef->GetScreenShakeIncremenet();
				g_theGame->AddScreenShake( screenShakeIncrement );
				AudioDefinition* shootSound = m_weapons[m_currentWeaponIndex]->GetAudioDefinition();

				if( weaponName == "Flamethrower" || weaponName == "LaserGun" )
				{
					if( shootSound->IsPlaying() )
					{

					}
					else
					{
						shootSound->PlaySound();
					}
				}
				else
				{
					shootSound->StopSound();
					shootSound->PlaySound();
				}

				m_firingTimer.Reset();
			}
		}
		if( leftMouseButton.WasJustReleased() )
		{
			if( weaponName == "Flamethrower" || weaponName == "LaserGun" )
			{
				AudioDefinition* shootSound = m_weapons[m_currentWeaponIndex]->GetAudioDefinition();
				shootSound->StopSound();
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

		if( deltaMouseWheelScroll > 0.f )
		{
			IncrementActiveWeapon();
		}
		else if( deltaMouseWheelScroll < 0.f )
		{
			DecrementActiveWeapon();
		}
	}

}

void Actor::UpdateNPC( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	if( !m_aiStartupTimer.HasElapsed() )
	{
		return;
	}

	if( m_enemyActor )
	{
		m_goalPosition = m_enemyActor->GetPosition();
	}
	
	float goalDistance = GetDistance2D( m_goalPosition, m_position );
	if( goalDistance < 5.f && (bool)m_firingTimer.CheckAndDecrementAll() )
	{
		if( m_aiCooldownTimer.HasElapsed() )
		{
			m_aiIsFiringState = !m_aiIsFiringState;
			m_aiCooldownTimer.Reset();
		}
		if( m_aiIsFiringState )
		{
			m_isFiring = true;
		}

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

	if( m_isDead )
	{
		currentAnimationName = "Dead";
		return currentAnimationName;
	}

// 	if( !m_isWeaponFlipped )
// 	{
// 		if( m_isDodging )
// 		{
// 			float velocityOrientation = m_velocity.GetAngleDegrees();
// 
// 			if( GetShortestAngularDistance( 0.f, velocityOrientation ) < 45.f )
// 			{
// 				currentAnimationName = "DodgeEast";
// 			}
// 			else if( GetShortestAngularDistance( 90.f, velocityOrientation ) < 45.f )
// 			{
// 				currentAnimationName = "DodgeNorth";
// 			}
// 			else if( GetShortestAngularDistance( 180.f, velocityOrientation ) < 45.f )
// 			{
// 				currentAnimationName = "DodgeEast";
// 			}
// 			else
// 			{
// 				currentAnimationName = "DodgeSouth";
// 			}
// 		}
// 		else if( m_velocity.GetLength() == 0.f )
// 		{
// 			if( GetShortestAngularDistance( 0.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleEast";
// 			}
// 			else if( GetShortestAngularDistance( 45.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleNorthEast";
// 			}
// 			else if( GetShortestAngularDistance( 90.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleNorth";
// 			}
// 			else if( GetShortestAngularDistance( 135.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleNorthEast";
// 			}
// 			else if( GetShortestAngularDistance( 180.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleEast";
// 			}
// 			else if( GetShortestAngularDistance( 225.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleSouthEast";
// 			}
// 			else if( GetShortestAngularDistance( 270.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleSouth";
// 			}
// 			else
// 			{
// 				currentAnimationName = "IdleSouthEast";
// 			}
// 		}
// 		else
// 		{
// 			if( GetShortestAngularDistance( 0.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveEast";
// 			}
// 			else if( GetShortestAngularDistance( 45.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveNorthEast";
// 			}
// 			else if( GetShortestAngularDistance( 90.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveNorth";
// 			}
// 			else if( GetShortestAngularDistance( 135.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveNorthEast";
// 			}
// 			else if( GetShortestAngularDistance( 180.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveEast";
// 			}
// 			else if( GetShortestAngularDistance( 225.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveSouthEast";
// 			}
// 			else if( GetShortestAngularDistance( 270.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveSouth";
// 			}
// 			else
// 			{
// 				currentAnimationName = "MoveSouthEast";
// 			}
// 		}
// 	}
// 	else
// 	{
// 		if( m_isDodging )
// 		{
// 			float velocityOrientation = m_velocity.GetAngleDegrees();
// 
// 			if( GetShortestAngularDistance( 0.f, velocityOrientation ) < 45.f )
// 			{
// 				currentAnimationName = "DodgeEast";
// 			}
// 			else if( GetShortestAngularDistance( 90.f, velocityOrientation ) < 45.f )
// 			{
// 				currentAnimationName = "DodgeNorth";
// 			}
// 			else if( GetShortestAngularDistance( 180.f, velocityOrientation ) < 45.f )
// 			{
// 				currentAnimationName = "DodgeWest";
// 			}
// 			else
// 			{
// 				currentAnimationName = "DodgeSouth";
// 			}
// 		}
// 		else if( m_velocity.GetLength() == 0.f )
// 		{
// 			if( GetShortestAngularDistance( 0.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleEast";
// 			}
// 			else if( GetShortestAngularDistance( 45.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleNorthEast";
// 			}
// 			else if( GetShortestAngularDistance( 90.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleNorth";
// 			}
// 			else if( GetShortestAngularDistance( 135.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleNorthWest";
// 			}
// 			else if( GetShortestAngularDistance( 180.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleWest";
// 			}
// 			else if( GetShortestAngularDistance( 225.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleSouthWest";
// 			}
// 			else if( GetShortestAngularDistance( 270.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "IdleSouth";
// 			}
// 			else
// 			{
// 				currentAnimationName = "IdleSouthEast";
// 			}
// 		}
// 		else
// 		{
// 			if( GetShortestAngularDistance( 0.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveEast";
// 			}
// 			else if( GetShortestAngularDistance( 45.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveNorthEast";
// 			}
// 			else if( GetShortestAngularDistance( 90.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveNorth";
// 			}
// 			else if( GetShortestAngularDistance( 135.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveNorthWest";
// 			}
// 			else if( GetShortestAngularDistance( 180.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveWest";
// 			}
// 			else if( GetShortestAngularDistance( 225.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveSouthWest";
// 			}
// 			else if( GetShortestAngularDistance( 270.f, m_weaponOrientationDegrees ) < 22.5f )
// 			{
// 				currentAnimationName = "MoveSouth";
// 			}
// 			else
// 			{
// 				currentAnimationName = "MoveSouthEast";
// 			}
// 		}
// 	}

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
		if( GetShortestAngularDistance( 0.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "MoveEast";
		}
		else if( GetShortestAngularDistance( 45.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "MoveNorthEast";
		}
		else if( GetShortestAngularDistance( 90.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "MoveNorth";
		}
		else if( GetShortestAngularDistance( 135.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "MoveNorthWest";
		}
		else if( GetShortestAngularDistance( 180.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "MoveWest";
		}
		else if( GetShortestAngularDistance( 225.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "MoveSouthWest";
		}
		else if( GetShortestAngularDistance( 270.f, m_weaponOrientationDegrees ) < 22.5f )
		{
			currentAnimationName = "MoveSouth";
		}
		else
		{
			currentAnimationName = "MoveSouthEast";
		}
	}

	return currentAnimationName;
}

