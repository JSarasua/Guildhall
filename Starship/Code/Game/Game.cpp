#pragma once
#include "Game/Game.hpp"
#include "App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;




Game::Game()
{
	m_camera = Camera();
	m_UICamera = Camera();
	m_player = PlayerShip( Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ), this );
	m_player.SetAlive();

	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		m_bullets[bulletIndex] = Bullet(this);
	}

	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		m_asteroids[asteroidIndex] = Asteroid(this);
	}
	for( int asteroidIndex = 0; asteroidIndex < NUM_STARTING_ASTEROIDS; asteroidIndex++ )
	{
		m_asteroids[asteroidIndex].SetAlive();
		m_asteroids[asteroidIndex].ActivateAsteroid();
	}
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		m_beetles[beetleIndex] = Beetle(this);
	}
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		m_wasps[waspIndex] = Wasp( this );
	}
	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		m_debris[debrisIndex] = Debris( this );
	}

	for( int powerUpIndex = 0; powerUpIndex < MAX_POWERUPS; powerUpIndex++ )
	{
		m_powerUps[powerUpIndex] = PowerUp( this );
		m_activePowerUps[powerUpIndex] = false;
	}
}

Game::~Game(){}

void Game::Startup()
{
	m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_SIZE_X,WORLD_SIZE_Y));
	m_UICamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_SIZE_X,WORLD_SIZE_Y));
	m_player.Startup();

	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		m_bullets[bulletIndex].Startup();
	}

	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		m_asteroids[asteroidIndex].Startup();
	}
}

void Game::Shutdown(){}

void Game::RunFrame(){}

void Game::Update( float deltaSeconds )
{
	if( m_currentLevel == 0 )
	{

	}
	else
	{
		CheckCollisions();
		UpdateEntities( deltaSeconds );
		UpdateCamera( deltaSeconds );


		if( IsLevelOver() )
		{
			AddAPowerUp();
			SpawnEnemies();
			m_currentLevel += 1;

			if( m_currentLevel == 7 )
			{
				g_theApp->RestartGame();
			}

		}		
	}

	if( m_player.GetLivesCount() == 0 )
	{
		static float twoSecondTimer = 0.f;
			twoSecondTimer += deltaSeconds;
			if( twoSecondTimer > 3.f )
			{
				g_theApp->RestartGame();
				twoSecondTimer = 0.f;

			}
	}

	CheckButtonPresses( deltaSeconds );


}

void Game::Render()
{

	if( m_currentLevel == 0 )
	{
		RenderLogo();
	}
	else 
	{
		g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
		g_theRenderer->BeginCamera( m_camera );
		RenderGame();
		g_theRenderer->EndCamera( m_camera );


		g_theRenderer->BeginCamera( m_UICamera );
		RenderUI();
		g_theRenderer->EndCamera( m_UICamera );
	}

}

int Game::CollidingWithWall( Vec2 m_position, float radius, bool isPlayer )
{
	if( isPlayer )
	{
		if( WORLD_SIZE_X < m_position.x + radius )
		{
			return 1;
		}
		else if( WORLD_SIZE_Y < m_position.y + radius )
		{
			return 2;
		}
		else if( 0.f > m_position.x - radius )
		{
			return 3;
		}
		else if( 0.f > m_position.y - radius )
		{
			return 4;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if( WORLD_SIZE_X < m_position.x - radius )
		{
			return 1;
		}
		else if( WORLD_SIZE_Y < m_position.y - radius )
		{
			return 2;
		}
		else if( 0.f > m_position.x + radius )
		{
			return 3;
		}
		else if( 0.f > m_position.y + radius )
		{
			return 4;
		}
		else
		{
			return 0;
		}
	}

}


void Game::ShootBullet()
{
	if( m_player.IsAlive() )
	{

		if( m_activePowerUps[SCATTER_SHOT] )
		{

			for( int scatterBulletIndex = 0; scatterBulletIndex < 3; scatterBulletIndex++ )
			{
				int bulletIndex = 0;
				while( m_bullets[bulletIndex].IsAlive() )
				{
					bulletIndex++;
				}
				if( bulletIndex >= MAX_BULLETS )
				{
					ERROR_RECOVERABLE( Stringf("Attempted to shoot more than the max of %i bullets\n",MAX_BULLETS));	
					return;
				}
				if( scatterBulletIndex == 0 )
				{
					float leftBulletOrientation = m_player.getOrientationDegrees() + 30.f;
					Vec2 leftBulletPosition = m_player.GetPosition() + Vec2::MakeFromPolarDegrees(leftBulletOrientation);

					m_bullets[bulletIndex].ActivateBullet( leftBulletPosition, leftBulletOrientation, m_activePowerUps[BOUNCING_BULLET] );

				}
				else if( scatterBulletIndex == 1 )
				{
					m_bullets[bulletIndex].ActivateBullet( m_player.GetNosePosition(), m_player.getOrientationDegrees() , m_activePowerUps[BOUNCING_BULLET]);

				}
				else
				{
					float rightBulletOrientation = m_player.getOrientationDegrees() - 30.f;
					Vec2 rightBulletPosition = m_player.GetPosition() + Vec2::MakeFromPolarDegrees( rightBulletOrientation );

					m_bullets[bulletIndex].ActivateBullet( rightBulletPosition, rightBulletOrientation, m_activePowerUps[BOUNCING_BULLET] );
				}
			}
		}
		else 
		{
			int bulletIndex = 0;
			while( m_bullets[bulletIndex].IsAlive() )
			{
				bulletIndex++;
			}
			if( bulletIndex >= MAX_BULLETS )
			{
				ERROR_RECOVERABLE( Stringf("Attempted to shoot more than the max of %i bullets\n",MAX_BULLETS));	
				return;
			}

			m_bullets[bulletIndex].ActivateBullet( m_player.GetNosePosition(), m_player.getOrientationDegrees(), m_activePowerUps[BOUNCING_BULLET] );



		}

	}
}

void Game::ShootBulletFromPosition( Vec2 initialPosition )
{
	int bulletIndex = 0;
	while( m_bullets[bulletIndex].IsAlive() )
	{
		bulletIndex++;
	}
	if( bulletIndex >= MAX_BULLETS )
	{
		ERROR_RECOVERABLE( Stringf("Attempted to shoot more than the max of %i bullets\n",MAX_BULLETS));	
		return;
	}
	m_bullets[bulletIndex].ActivateBullet( initialPosition, m_rand.RollRandomFloatInRange(0.f,359.f), m_activePowerUps[BOUNCING_BULLET]);
	m_bullets[bulletIndex].setColor(Rgba8(255,255,255));

}

void Game::CreateAsteroid()
{
	int asteroidIndex = 0;
	while( m_asteroids[asteroidIndex].IsAlive() )
	{
		asteroidIndex++;
	}
	if( asteroidIndex >= MAX_ASTEROIDS )
	{
		ERROR_RECOVERABLE( Stringf("Attempted to create more than the max of %i asteroids\n",MAX_ASTEROIDS) );	
		return;
	}
	m_asteroids[asteroidIndex].ActivateAsteroid();
}

void Game::ResurrectPlayer()
{
	if( m_player.IsAlive() )
	{

		return;
	}
	m_player.ResurrectPlayer();
	

}

void Game::CheckCollisions()
{
	float distBetweenEntities = 0.f;

	for( int powerUpIndex = 0; powerUpIndex < MAX_POWERUPS; powerUpIndex++ )
	{
		if( m_powerUps[powerUpIndex].IsAlive() && m_player.IsAlive() )
		{
			distBetweenEntities = GetDistance2D( m_player.GetPosition(), m_powerUps[powerUpIndex].GetPosition() );

			if( distBetweenEntities < (PLAYER_SHIP_PHYSICS_RADIUS+POWERUP_PHYSICALRADIUS) )
			{
				m_activePowerUps[m_powerUps[powerUpIndex].GetPowerUp()] = true;
				m_powerUps[powerUpIndex].LoseAllHealth();
			}
		}
	}

	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		if( m_asteroids[asteroidIndex].IsAlive() && m_player.IsAlive() )
		{
			distBetweenEntities = GetDistance2D( m_player.GetPosition(), m_asteroids[asteroidIndex].GetPosition() );

			if( distBetweenEntities < (PLAYER_SHIP_PHYSICS_RADIUS+ASTEROID_PHYSICS_RADIUS) )
			{
				m_maxCameraShake+= 5.f;

				m_player.LoseAllHealth();
				DeactivatePowerUps();
				CreateDebris(m_player.GetPosition(), m_player.GetColor(), m_player.GetForwardVector(), 50);
				m_asteroids[asteroidIndex].LoseAllHealth();
				CreateDebris(m_asteroids[asteroidIndex].GetPosition(), m_asteroids[asteroidIndex].GetColor(), m_asteroids[asteroidIndex].GetForwardVector(), 50);

			}
		}
	}
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		if( m_bullets[bulletIndex].IsAlive() )
		{
			for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
			{
				if( m_asteroids[asteroidIndex].IsAlive() )
				{
					distBetweenEntities = GetDistance2D( m_bullets[bulletIndex].GetPosition(), m_asteroids[asteroidIndex].GetPosition() );

					if( distBetweenEntities < (BULLET_PHYSICS_RADIUS+ASTEROID_PHYSICS_RADIUS) )
					{
						m_maxCameraShake+= .2f;


						m_asteroids[asteroidIndex].Lose1Health();
						m_bullets[bulletIndex].Lose1Health();
						if( m_bullets[bulletIndex].IsAlive() )
						{
							m_bullets[bulletIndex].Bounce();
							ShootBulletFromPosition( m_bullets[bulletIndex].GetPosition() );
							ShootBulletFromPosition( m_bullets[bulletIndex].GetPosition() );

						}

						CreateDebris(m_bullets[bulletIndex].GetPosition(),m_bullets[bulletIndex].GetColor(), m_bullets[bulletIndex].GetForwardVector(), 3);
						CreateDebris(m_asteroids[asteroidIndex].GetPosition(), m_asteroids[asteroidIndex].GetColor(), m_asteroids[asteroidIndex].GetForwardVector(), 15);
					}
				}
			}
		}

	}
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		if( m_beetles[beetleIndex].IsAlive() )
		{
			if( m_player.IsAlive() )
			{
				distBetweenEntities = GetDistance2D( m_player.GetPosition(), m_beetles[beetleIndex].GetPosition() );

				if( distBetweenEntities < PLAYER_SHIP_PHYSICS_RADIUS + BEETLE_PHYSICS_RADIUS )
				{
					m_maxCameraShake+= 5.f;
					m_beetles[beetleIndex].Lose1Health();
					m_player.LoseAllHealth();
					DeactivatePowerUps();

					CreateDebris(m_player.GetPosition(), m_player.GetColor(), m_player.GetForwardVector(), 50);
					CreateDebris(m_beetles[beetleIndex].GetPosition(), m_beetles[beetleIndex].GetColor(), m_beetles[beetleIndex].GetForwardVector(), 10);
				}
			}
			for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
			{
				if( m_bullets[bulletIndex].IsAlive()  && m_beetles[beetleIndex].IsAlive() )
				{
					distBetweenEntities = GetDistance2D( m_bullets[bulletIndex].GetPosition(), m_beetles[beetleIndex].GetPosition() );
					if( distBetweenEntities < BULLET_PHYSICS_RADIUS + BEETLE_PHYSICS_RADIUS )
					{
						m_maxCameraShake+= .2f;

						m_beetles[beetleIndex].Lose1Health();
						m_bullets[bulletIndex].Lose1Health();
						if( m_bullets[bulletIndex].IsAlive() )
						{
							m_bullets[bulletIndex].Bounce();
							ShootBulletFromPosition( m_bullets[bulletIndex].GetPosition() );
							ShootBulletFromPosition( m_bullets[bulletIndex].GetPosition() );

						}


						CreateDebris( m_beetles[beetleIndex].GetPosition(), m_beetles[beetleIndex].GetColor(), m_beetles[beetleIndex].GetForwardVector(), 10 );
						CreateDebris( m_bullets[bulletIndex].GetPosition(), m_bullets[bulletIndex].GetColor(), m_bullets[bulletIndex].GetForwardVector(), 3 );
					}
				}

			}
		}

	}
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		if( m_wasps[waspIndex].IsAlive() )
		{
			if( m_player.IsAlive() )
			{
				distBetweenEntities = GetDistance2D( m_player.GetPosition(), m_wasps[waspIndex].GetPosition() );

				if( distBetweenEntities < PLAYER_SHIP_PHYSICS_RADIUS + WASP_PHYSICS_RADIUS )
				{
					m_maxCameraShake+= 5.f;
					m_player.LoseAllHealth();
					DeactivatePowerUps();
					m_wasps[waspIndex].Lose1Health();

					CreateDebris( m_player.GetPosition(), m_player.GetColor(), m_player.GetForwardVector(), 50 );
					CreateDebris( m_wasps[waspIndex].GetPosition(), m_wasps[waspIndex].GetColor(), m_wasps[waspIndex].GetForwardVector(), 10 );
				}


			}
			for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
			{
				if( m_bullets[bulletIndex].IsAlive() && m_wasps[waspIndex].IsAlive())
				{
					distBetweenEntities = GetDistance2D( m_bullets[bulletIndex].GetPosition(), m_wasps[waspIndex].GetPosition() );
					if( distBetweenEntities < BULLET_PHYSICS_RADIUS + WASP_PHYSICS_RADIUS )
					{
						m_maxCameraShake+= .2f;
						m_wasps[waspIndex].Lose1Health();
						m_bullets[bulletIndex].Lose1Health();
						if( m_bullets[bulletIndex].IsAlive() )
						{
							m_bullets[bulletIndex].Bounce();
							ShootBulletFromPosition( m_bullets[bulletIndex].GetPosition() );
							ShootBulletFromPosition( m_bullets[bulletIndex].GetPosition() );

						}


						CreateDebris( m_wasps[waspIndex].GetPosition(), m_wasps[waspIndex].GetColor(), m_wasps[waspIndex].GetForwardVector(), 10 );
						CreateDebris( m_bullets[bulletIndex].GetPosition(), m_bullets[bulletIndex].GetColor(), m_bullets[bulletIndex].GetForwardVector(), 3 );
					}
				}

			}
		}

	}
}

void Game::UpdateEntities( float deltaSeconds )
{
	if( m_player.IsAlive() )
	{
		m_player.Update( deltaSeconds );
	}

	for( int powerUpIndex = 0; powerUpIndex < MAX_POWERUPS; powerUpIndex++ )
	{
		if( m_powerUps[powerUpIndex].IsAlive() )
		{
			m_powerUps[powerUpIndex].Update( deltaSeconds );
		}
	}

	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		if( m_debris[debrisIndex].IsAlive() )
		{
			m_debris[debrisIndex].Update( deltaSeconds );
		}
	}

	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		if( m_bullets[bulletIndex].IsAlive() )
		{
			m_bullets[bulletIndex].Update( deltaSeconds );
		}
	}
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		if( m_asteroids[asteroidIndex].IsAlive() )
		{
			m_asteroids[asteroidIndex].Update( deltaSeconds );
		}
	}
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		if( m_beetles[beetleIndex].IsAlive() )
		{
			m_beetles[beetleIndex].Update( deltaSeconds );
		}
	}
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		if( m_wasps[waspIndex].IsAlive() )
		{
			m_wasps[waspIndex].Update( deltaSeconds );

		}
	}
}

void Game::UpdateCamera( float deltaSeconds )
{
	m_maxCameraShake -= CAMERAABLATIONPERSECOND * deltaSeconds;
	m_maxCameraShake = Clampf(m_maxCameraShake, 0.f, 1.f);
	Vec2 currentShake = Vec2(m_rand.RollRandomFloatInRange(-m_maxCameraShake,m_maxCameraShake),m_rand.RollRandomFloatInRange(-m_maxCameraShake,m_maxCameraShake));

	m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_camera.Translate2D(currentShake);
}

void Game::CreateDebris( const Vec2 initialPosition, const Rgba8& debrisColor, Vec2 initialVelocity, int debrisCount )
{
	for( int debrisToAdd = 0; debrisToAdd < debrisCount; debrisToAdd++ )
	{
		int debrisIndex = 0;
		while( m_debris[debrisIndex].IsAlive()  && debrisIndex < MAX_DEBRIS - 1)
		{
			debrisIndex++;
		}
		if( debrisIndex <= MAX_DEBRIS )
		{
			m_debris[debrisIndex].ActivateDebris(debrisColor, initialVelocity,initialPosition);
		}


	}
}

void Game::RenderGame()
{
	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		if( m_debris[debrisIndex].IsAlive() )
		{
			m_debris[debrisIndex].Render();
		}
	}

	if( m_player.IsAlive() )
	{
		m_player.Render();
	}

	for( int powerUpIndex = 0; powerUpIndex < MAX_POWERUPS; powerUpIndex++ )
	{
		if( m_powerUps[powerUpIndex].IsAlive() )
		{
			m_powerUps[powerUpIndex].Render();
		}
	}

	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		if( m_bullets[bulletIndex].IsAlive() )
		{
			m_bullets[bulletIndex].Render();
		}
	}
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		if( m_asteroids[asteroidIndex].IsAlive() )
		{
			m_asteroids[asteroidIndex].Render();
		}
	}
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		if( m_beetles[beetleIndex].IsAlive() )
		{
			m_beetles[beetleIndex].Render();
		}
	}
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		if( m_wasps[waspIndex].IsAlive() )
		{
			m_wasps[waspIndex].Render();

		}
	}

	if( m_debugMode )
	{
		if( m_player.IsAlive() )
		{
			DrawLine( m_player.GetPosition(), m_player.GetPosition()+m_player.GetForwardVector(), Rgba8( 255, 255, 0 ), 0.1f );
			DrawRing( m_player.GetPosition(), PLAYER_SHIP_PHYSICS_RADIUS, Rgba8( 0, 255, 255 ), 0.1f );
			DrawRing( m_player.GetPosition(), PLAYER_SHIP_COSMETIC_RADIUS, Rgba8( 255, 0, 255 ), 0.1f );
		}

		for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
		{
			if( m_bullets[bulletIndex].IsAlive() )
			{
				if( m_player.IsAlive() )
				{
					DrawLine( m_player.GetPosition(), m_bullets[bulletIndex].GetPosition(), Rgba8( 50, 50, 50 ), 0.1f );

				}
				DrawLine( m_bullets[bulletIndex].GetPosition(), m_bullets[bulletIndex].GetPosition()+m_bullets[bulletIndex].GetForwardVector(), Rgba8( 255, 255, 0 ), 0.1f );
				DrawRing( m_bullets[bulletIndex].GetPosition(), BULLET_PHYSICS_RADIUS, Rgba8( 0, 255, 255 ), 0.1f );
				DrawRing( m_bullets[bulletIndex].GetPosition(), BULLET_COSMETIC_RADIUS, Rgba8( 255, 0, 255 ), 0.1f );
			}
		}
		for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
		{
			if( m_asteroids[asteroidIndex].IsAlive() )
			{
				if( m_player.IsAlive() )
				{
					DrawLine( m_player.GetPosition(), m_asteroids[asteroidIndex].GetPosition(), Rgba8( 50, 50, 50 ), 0.1f );

				}
				DrawLine( m_asteroids[asteroidIndex].GetPosition(), m_asteroids[asteroidIndex].GetPosition()+m_asteroids[asteroidIndex].GetForwardVector(), Rgba8( 255, 255, 0 ), 0.1f );
				DrawRing( m_asteroids[asteroidIndex].GetPosition(), ASTEROID_PHYSICS_RADIUS, Rgba8( 0, 255, 255 ), 0.1f );
				DrawRing( m_asteroids[asteroidIndex].GetPosition(), ASTEROID_COSMETIC_RADIUS, Rgba8( 255, 0, 255 ), 0.1f );

			}
		}

	}
}

void Game::RenderUI()
{
	for( int livesIndex = 0; livesIndex < m_player.GetLivesCount() - 1; livesIndex++ )
	{
		Vertex_PCU playerShipVertexes[] =
		{
			Vertex_PCU( Vec3( 0.0f,-1.0f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 1.f,0.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

			Vertex_PCU( Vec3( -2.0f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 0.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

			Vertex_PCU( Vec3( -2.0f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( -2.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

			Vertex_PCU( Vec3( -2.0f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 0.f,2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

			Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 2.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 0.f,2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

			Vertex_PCU( Vec3( -2.0f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 0.f,-2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 0.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

			Vertex_PCU( Vec3( 0.f,-2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 2.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
			Vertex_PCU( Vec3( 0.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) )
		};
		Vertex_PCU::TransformVertexArray( 21, playerShipVertexes, 1.f, 0, Vec2((float)livesIndex * 5.f + 5.f, 95.f ));

		g_theRenderer->DrawVertexArray( 21, playerShipVertexes );
	}
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	const XboxController& controller = g_theInput->GetXboxController(0);
	static float timeSinceLastShot = 0.f;
	if( m_currentLevel == 0 )
	{
		if( controller.GetButtonState( XBOX_BUTTON_ID_START ).WasJustPressed() )
		{
			m_currentLevel = 1;

		}
	}
	else 
	{
		if( controller.GetButtonState( XBOX_BUTTON_ID_START ).WasJustPressed() )
		{
			ResurrectPlayer();

		}
		if( m_activePowerUps[RAPID_FIRE] )
		{
			if( controller.GetButtonState( XBOX_BUTTON_ID_A ).IsPressed() )
			{
				if( timeSinceLastShot > RAPID_FIRE_PER_SECOND )
				{
					timeSinceLastShot = 0.f;
					ShootBullet();
				}

			}
		}
		else
		{
			if( controller.GetButtonState( XBOX_BUTTON_ID_A ).WasJustPressed() )
			{

				ShootBullet();
			}
		}

	}
	timeSinceLastShot += deltaSeconds;
}

void Game::RenderLogo()
{
	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	g_theRenderer->BeginCamera( m_camera );


	Vertex_PCU playerShipVertexes[] =
	{
		Vertex_PCU( Vec3( 0.0f,-1.0f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 1.f,0.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( -2.0f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( -2.0f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( -2.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( -2.0f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( 0.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 2.f,1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( -2.0f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( 0.f,-2.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 2.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( 0.f,-1.f,0.f ),Rgba8( 132,156,169,255 ),Vec2( 0.f,0.f ) )
	};
	Vertex_PCU::TransformVertexArray( 21, playerShipVertexes, 10.f, 0, Vec2(WORLD_CENTER_X,WORLD_CENTER_Y ) );

	g_theRenderer->DrawVertexArray( 21, playerShipVertexes );


	g_theRenderer->EndCamera( m_camera );
}

bool Game::IsLevelOver()
{
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		if( m_beetles[beetleIndex].IsAlive() )
		{
			return false;
		}
	}
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		if( m_wasps[waspIndex].IsAlive() )
		{
			return false;
		}
	}
	return true;
}

void Game::SpawnEnemies()
{
	if( m_currentLevel == 0 )
	{

	}
	else if( m_currentLevel == 1 )
	{
		for( int beetleIndex = 0; beetleIndex < 5; beetleIndex++ )
		{
			m_beetles[beetleIndex].ActivateBeetle( GetRandomPositionOutsideWorld(), 0.f );
		}
		for( int waspIndex = 0; waspIndex < 3; waspIndex++ )
		{
			m_wasps[waspIndex].ActivateWasp( GetRandomPositionOutsideWorld(), 0.f );
		}
	}
	else if( m_currentLevel == 2 )
	{
		for( int beetleIndex = 0; beetleIndex < 10; beetleIndex++ )
		{
			m_beetles[beetleIndex].ActivateBeetle( GetRandomPositionOutsideWorld(), 0.f );
		}
		for( int waspIndex = 0; waspIndex < 5; waspIndex++ )
		{
			m_wasps[waspIndex].ActivateWasp( GetRandomPositionOutsideWorld(), 0.f );
		}
	}
	
	else if( m_currentLevel == 3 )
	{
		for( int beetleIndex = 0; beetleIndex < 15; beetleIndex++ )
		{
			m_beetles[beetleIndex].ActivateBeetle( GetRandomPositionOutsideWorld(), 0.f );
		}
		for( int waspIndex = 0; waspIndex < 10; waspIndex++ )
		{
			m_wasps[waspIndex].ActivateWasp( GetRandomPositionOutsideWorld(), 0.f );
		}
	}
	else if( m_currentLevel == 4 )
	{
		for( int beetleIndex = 0; beetleIndex < 20; beetleIndex++ )
		{
			m_beetles[beetleIndex].ActivateBeetle( GetRandomPositionOutsideWorld(), 0.f );
		}
		for( int waspIndex = 0; waspIndex < 15; waspIndex++ )
		{
			m_wasps[waspIndex].ActivateWasp( GetRandomPositionOutsideWorld(), 0.f );
		}
	}
	else if( m_currentLevel == 5 )
	{
		for( int beetleIndex = 0; beetleIndex < 25; beetleIndex++ )
		{
			m_beetles[beetleIndex].ActivateBeetle( GetRandomPositionOutsideWorld(), 0.f );
		}
		for( int waspIndex = 0; waspIndex < 20; waspIndex++ )
		{
			m_wasps[waspIndex].ActivateWasp( GetRandomPositionOutsideWorld(), 0.f );
		}
	}
	
}

Vec2 Game::GetRandomPositionOutsideWorld()
{
	float randX = m_rand.RollRandomFloatInRange(-50.f, 250.f);
	float randY = m_rand.RollRandomFloatInRange(-50.f, 150.f);
	while( randX > 0.f && randX < WORLD_SIZE_X )
	{
		randX = m_rand.RollRandomFloatInRange(-50.f, 250.f);
	}
	while( randY > 0.f && randY < WORLD_SIZE_Y )
	{
		randY = m_rand.RollRandomFloatInRange(-50.f, 150.f);
	}
	return Vec2(randX,randY);
}

void Game::AddAPowerUp()
{
	int powerUpIndex = 0;
	while( m_powerUps[powerUpIndex].IsAlive() && powerUpIndex < MAX_POWERUPS - 1 )
	{
		powerUpIndex++;
	}
	
	int activePowerUpIndex = 0;
	while( m_activePowerUps[activePowerUpIndex] )
	{
		activePowerUpIndex++;
	}
	m_powerUps[powerUpIndex].ActivatePowerUp(Vec2(m_rand.RollRandomFloatInRange(30.f,150.f),m_rand.RollRandomFloatInRange( 20.f, 80.f)),activePowerUpIndex);

}

void Game::DeactivatePowerUps()
{
	for( int activePowerUpIndex = 0; activePowerUpIndex < MAX_POWERUPS; activePowerUpIndex++ )
	{
		m_activePowerUps[activePowerUpIndex] = false;
	}
}

void Game::FlipDebugMode()
{
	m_debugMode = !m_debugMode;
}

const Vec2 Game::GetPlayerPosition() 
{
	return m_player.GetPosition();
}

int Game::GetCurrentLevel()
{
	return m_currentLevel;
}

void Game::IncrementLevel()
{
	m_currentLevel += 1;
}
