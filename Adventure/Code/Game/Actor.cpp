#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"


Actor::Actor( Vec2 initialPosition, Vec2 initialVelocity, float initialOrientationDegrees, float initialAngularVelocity, ActorDefinition* actorDef ) :
	m_actorDef(actorDef),
	Entity(initialPosition, initialVelocity, initialOrientationDegrees, initialAngularVelocity)
{
	m_name = m_actorDef->m_name;
	m_canWalk = m_actorDef->m_canWalk;
	m_canSwim = m_actorDef->m_canSwim;
	m_canFly = m_actorDef->m_canFly;

	m_physicsRadius = m_actorDef->m_physicsRadius;
}

void Actor::Startup()
{

}

void Actor::Update( float deltaSeconds )
{
	if( m_name == "Player" )
	{
		UpdateFromJoystick(deltaSeconds);
		UpdateFromKeyboard(deltaSeconds);
		m_position = TransformPosition2D(m_position, 1.f, 0.f, m_velocity * deltaSeconds);
	}
	else
	{
		Entity::Update(deltaSeconds);
	}

}

void Actor::Render() const
{
	AABB2 actorBounds = m_actorDef->m_drawBounds;
	actorBounds.Translate(m_position);
	const Rgba8& actorTint = m_actorDef->m_tint;
	const AABB2& actorUVs = m_actorDef->m_spriteUVs;
	const Texture& actorTexture = g_actorSpriteSheet->GetTexture();


	g_theRenderer->BindTexture( &actorTexture );
	g_theRenderer->DrawRotatedAABB2Filled(actorBounds,actorTint,actorUVs.mins,actorUVs.maxs,m_orientationDegrees);

}

void Actor::UpdateFromJoystick( float deltaSeconds )
{
	const XboxController& controller = g_theInput->GetXboxController( 0 );
	if( controller.IsConnected() )
	{
		const float& joystickOrientation = controller.GetLeftJoystick().GetAngleDegrees();
		const float& joystickMagnitude = controller.GetLeftJoystick().GetMagnitude();

		const float& actorSpeed = m_actorDef->m_speed;
		const float& actorTurnSpeed = m_actorDef->m_turnSpeed;

		if( joystickMagnitude != 0 )
		{


			m_orientationDegrees = GetTurnedToward( m_orientationDegrees, joystickOrientation, actorTurnSpeed * deltaSeconds );
			m_velocity  = Vec2::MakeFromPolarDegrees( m_orientationDegrees, joystickMagnitude * actorSpeed );
			
		}
		else if( controller.GetLeftJoystick().GetMagnitude() == 0 )
		{
			m_velocity *= 0.f;
		}
	}
}

void Actor::UpdateFromKeyboard( float deltaSeconds )
{
	UNUSED( deltaSeconds );


	const float& actorSpeed = m_actorDef->m_speed;

	const KeyButtonState& leftArrow = g_theInput->GetKeyStates( 0x25 );
	const KeyButtonState& upArrow = g_theInput->GetKeyStates( 0x26 );
	const KeyButtonState& rightArrow = g_theInput->GetKeyStates( 0x27 );
	const KeyButtonState& downArrow = g_theInput->GetKeyStates( 0x28 );

	const KeyButtonState& sKey = g_theInput->GetKeyStates( 'S' );
	const KeyButtonState& eKey = g_theInput->GetKeyStates( 'E' );
	const KeyButtonState& fKey = g_theInput->GetKeyStates( 'F' );
	const KeyButtonState& dKey = g_theInput->GetKeyStates( 'D' );

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
}

