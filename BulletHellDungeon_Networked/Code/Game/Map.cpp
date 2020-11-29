#include "Game/Map.hpp"
#include "Game/Bullet.hpp"
#include "Game/TileMetaData.hpp"
#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/MapGenStep.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Game/EnemySpawner.hpp"
#include "Game/Loot.hpp"
#include "Game/EntityFactory.hpp"



extern App* g_theApp;
extern RenderContext* g_theRenderer;


Map::Map( const char* mapDefName )
{
	m_mapDef = MapDefinition::s_definitions[mapDefName];
	GUARANTEE_OR_DIE(m_mapDef, "Map definition doesn't exist");
	
	m_name = m_mapDef->m_name;
	m_mapSize = m_mapDef->m_mapDimensions;
	m_startPosition = m_mapDef->m_startPosition;
	SpawnTiles();
	SpawnEntities();
}

Map::~Map()
{
	for( size_t entityIndex = 1; entityIndex < m_entities.size(); entityIndex++ )
	{
		if( m_entities[entityIndex] )
		{
			EntityFactory::DeleteEntity( m_entities[entityIndex] );
			m_entities[entityIndex] = nullptr;
		}
	}

	for( size_t spawnerIndex = 0; spawnerIndex < m_enemySpawners.size(); spawnerIndex++ )
	{
		if( m_enemySpawners[spawnerIndex] )
		{
			delete m_enemySpawners[spawnerIndex];
			m_enemySpawners[spawnerIndex] = nullptr;
		}
	}
}


void Map::Startup()
{
	//m_entities[0]->SetPosition( m_startPosition );

	for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		m_tiles[tileIndex].AppendVerts(m_vertsToRender);
	}
}


void Map::Shutdown()
{

}


void Map::Update( float deltaSeconds )
{
	UpdateTiles(deltaSeconds);
	UpdateEntities(deltaSeconds);
	UpdateDebugMode();
	GarbageCollectEntities();
}

void Map::Render()
{
	RenderTiles();
	RenderEntities();

	if( g_theApp->GetDebugGameMode() )
	{
		//RenderDebugMode();
	}
}


Actor* Map::GetPlayer()
{
	return static_cast<Actor*>(m_entities[0]);
}


void Map::GetPlayers(std::vector<Actor*>& players)
{
	players.push_back(static_cast<Actor*>(m_entities[0]));
	players.push_back(static_cast<Actor*>(m_entities[1]));
	players.push_back(static_cast<Actor*>(m_entities[2]));
	players.push_back(static_cast<Actor*>(m_entities[3]));
}

IntVec2 Map::GetMapBounds() const
{
	return m_mapSize;
}


std::vector<Tile*> Map::GetNeighboringTiles( Tile* currentTile )
{
	std::vector<Tile*> neighboringTiles;

	Tile* northTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_NORTH);
	Tile* westTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_WEST);
	Tile* southTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_SOUTH);
	Tile* eastTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_EAST);
	Tile* northWestTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_NORTHWEST);
	Tile* southWestTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_SOUTHWEST);
	Tile* southEastTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_SOUTHEAST);
	Tile* northEastTile = GetTileAtPosition(currentTile->m_tileCoords + STEP_NORTHEAST);

	if(northTile )		neighboringTiles.push_back(northTile); 
	if(westTile )		neighboringTiles.push_back(westTile); 
	if(southTile )		neighboringTiles.push_back(southTile); 
	if(eastTile )		neighboringTiles.push_back(eastTile); 
	if(northWestTile )	neighboringTiles.push_back(northWestTile); 
	if(southWestTile )	neighboringTiles.push_back(southWestTile); 
	if(southEastTile )	neighboringTiles.push_back(southEastTile); 
	if(northEastTile )	neighboringTiles.push_back(northEastTile); 

	return neighboringTiles;
}


void Map::RenderTiles()
{
	Texture const& tex = g_tileSpriteSheet->GetTexture();
	g_theRenderer->BindTexture(&tex);
	g_theRenderer->DrawVertexArray(m_vertsToRender);
}


void Map::RenderEntities()
{
	for( int entityIndex = 1; entityIndex < m_entities.size(); entityIndex++ )
	{
		if( !m_entities[entityIndex]->IsGarbage() )
		{
			m_entities[entityIndex]->Render();
		}
	}

	if( !m_entities[0]->IsGarbage() )
	{
		m_entities[0]->Render();
	}
}


void Map::UpdateTiles( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}


void Map::UpdateEntities( float deltaSeconds )
{
	static int frameCounter = 0;
	UpdateSpawners();

	SpawnBullets();

	for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		if( !m_entities[entityIndex]->IsGarbage() )
		{
			m_entities[entityIndex]->Update(deltaSeconds);
			Entity const& entity = *m_entities[entityIndex];

			if( frameCounter == 0 )
			{
// 				if( entity.m_entityID > 4 )
// 				{
// 					std::string updateStr = Stringf( "Update Entity: %i, pos: %f, %f", entity.m_entityID, entity.m_position.x, entity.m_position.y );
// 					g_theConsole->PrintString( Rgba8::GREEN, updateStr );
// 				}
				EventArgs args;
				args.SetValue( "position", entity.m_position );
				args.SetValue( "velocity", entity.m_velocity );
				args.SetValue( "orientationDegrees", entity.m_orientationDegrees );
				args.SetValue( "weaponOrientationDegrees", entity.m_weaponOrientationDegrees );
				args.SetValue( "angularVelocity", entity.m_angularVelocity );
				args.SetValue( "health", entity.m_health );
				args.SetValue( "isDead", entity.m_isDead );
				args.SetValue( "entityID", entity.m_entityID );

				g_theEventSystem->FireEvent( "UpdateEntity", NOCONSOLECOMMAND, &args );
			}

		}
	}

	frameCounter++;
	frameCounter %= 3;

	if( !g_theApp->IsNoClipping() )
	{
		PushEntitiesOutOfWalls();
		ResolveEntitiesCollisions();
	}
}

void Map::UpdateSpawners()
{
	for( size_t spawnerIndex = 0; spawnerIndex < m_enemySpawners.size(); spawnerIndex++ )
	{
		m_enemySpawners[spawnerIndex]->Update();
	}
}

Tile& Map::GetRandomMapTile()
{
	IntRange xMapRange(1, m_mapSize.x - 2);
	IntRange yMapRange(1, m_mapSize.y - 2);

	IntVec2 randomMapCoordinates;
	randomMapCoordinates.x = xMapRange.GetRandomInRange(g_theGame->m_rand);
	randomMapCoordinates.y = yMapRange.GetRandomInRange(g_theGame->m_rand);

	int tileIndex = GetTileIndexForTileCoordinates(randomMapCoordinates);

	Tile& randomTile = m_tiles[tileIndex];
	return randomTile;
}


Tile& Map::GetRandomAdjacentMapTile(Tile& currentTile)
{
	int randomDirection = g_theGame->m_rand.RollRandomIntInRange(0,3);
	IntVec2 currentTilePosition = currentTile.m_tileCoords;
	IntVec2 directionToChangeTilePosition(0,0);

	if( randomDirection == 0 )
	{
		directionToChangeTilePosition = IntVec2( 1, 0 );
	}
	else if( randomDirection == 1 )
	{
		directionToChangeTilePosition = IntVec2( 0, 1 );
	}
	else if( randomDirection == 2 )
	{
		directionToChangeTilePosition = IntVec2( -1, 0 );
	}
	else if( randomDirection == 3 )
	{
		directionToChangeTilePosition = IntVec2( 0, -1 );
	}

	while( !IsTilePositionValid( currentTilePosition+directionToChangeTilePosition ) )
	{
		randomDirection = g_theGame->m_rand.RollRandomIntInRange(0,3);
		if( randomDirection == 0 )
		{
			directionToChangeTilePosition = IntVec2(1,0);
		}
		else if( randomDirection == 1 )
		{
			directionToChangeTilePosition = IntVec2(0,1);
		}
		else if( randomDirection == 2 )
		{
			directionToChangeTilePosition = IntVec2(-1,0);
		}
		else if( randomDirection == 3 )
		{
			directionToChangeTilePosition = IntVec2(0,-1);
		}
	}
	IntVec2 newTilePosition = currentTilePosition + directionToChangeTilePosition;
	int newTileIndex = GetTileIndexForTileCoordinates(newTilePosition);
	Tile& newTile = m_tiles[newTileIndex];

	return newTile;
}


void Map::SpawnTiles()
{
	int x = 0;
	int y = 0;
	std::string fillTile = m_mapDef->m_fillTile;
	std::string edgeTile = m_mapDef->m_edgeTile;
	TileDefinition* fillTileDef = TileDefinition::s_definitions[fillTile.c_str()];
	//TileDefinition* edgeTileDef = TileDefinition::s_definitions[edgeTile.c_str()];
	int numOfTiles = m_mapSize.x * m_mapSize.y;

	for( int mapIndex = 0; mapIndex < numOfTiles; mapIndex++ )
	{
		y = mapIndex / (m_mapSize.x);
		x = mapIndex - y * m_mapSize.x;

		m_tiles.push_back(Tile(IntVec2(x,y), fillTileDef));
		m_tileMetaData.push_back(TileMetaData());
	}

	//MapGenSteps
	std::vector<MapGenStep*>& mapGenSteps = m_mapDef->m_mapGenSteps;
	for( int mapGenStepIndex = 0; mapGenStepIndex < mapGenSteps.size(); mapGenStepIndex++ )
	{
		mapGenSteps[mapGenStepIndex]->RunStep(*this);
	}
}

void Map::SpawnEntities()
{
	m_entities.push_back( nullptr );
	m_entities.push_back( nullptr );
	m_entities.push_back( nullptr );
	m_entities.push_back( nullptr );

}


void Map::SpawnBullets()
{
	size_t entityListSize = m_entities.size();
	for( size_t entityIndex = 0; entityIndex < entityListSize; entityIndex++ )
	{
		Entity* entity = m_entities[entityIndex];
		if( !entity->IsGarbage() )
		{
			if( entity->IsFiring() )
			{
				SpawnBullet( entity );
			}
		}
	}
}

void Map::SpawnBullet( Entity* shooter )
{
	Actor* actorShooter = (Actor*)shooter;
	BulletDefinition const* bulletDef = actorShooter->GetBulletDefinition();
	int bulletCount = actorShooter->GetBulletsPerShot();
	float bulletSpread = actorShooter->GetBulletSpreadDegrees();
	float bulletSpeedMultiplier = 1.f;
	Vec2 actorVelocity = shooter->GetForwardVector();
	actorVelocity *= 0.5f;

	//Vec2 bulletPosition = shooter->GetBulletStartPosition();
	Vec2 bulletPosition = actorShooter->GetMuzzlePosition();
	float bulletOrientation = shooter->GetWeaponOrientationDegrees();
	EntityType bulletType;
	if( shooter->m_entityType == ENTITY_TYPE_PLAYER )
	{
		bulletType = ENTITY_TYPE_GOOD_BULLET;
	}
	else
	{
		bulletType = ENTITY_TYPE_EVIL_BULLET;
	}

	if( shooter->m_entityType == ENTITY_TYPE_NPC_ENEMY )
	{
		bulletSpeedMultiplier = 0.5f;
	}

	for( size_t entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		if( !m_entities[entityIndex] )
		{
/*			if( m_entities[entityIndex]->m_entityType != ENTITY_TYPE_BOSS )*/
			{
				float spread = g_theGame->m_rand.RollRandomFloatInRange( -bulletSpread, bulletSpread );
				float bulletOrientationWithSpread = bulletOrientation + spread;
				m_entities[entityIndex] = EntityFactory::CreateBullet( bulletDef, bulletPosition, bulletOrientationWithSpread, bulletType, FACTION_GOOD, bulletSpeedMultiplier, actorVelocity );
				//m_entities[entityIndex] = new Bullet( bulletPosition, bulletOrientationWithSpread, bulletType, FACTION_GOOD, bulletDef, bulletSpeedMultiplier, actorVelocity );

				bulletCount--;
				if( bulletCount <= 0 )
				{
					break;
				}
			}
// 			else
// 			{
// 				SpawnBossBullets( actorShooter );
// 			}

		}
	}
	for( bulletCount; bulletCount > 0; bulletCount-- )
	{
		float spread = g_theGame->m_rand.RollRandomFloatInRange( -bulletSpread, bulletSpread );
		float bulletOrientationWithSpread = bulletOrientation + spread;
		m_entities.push_back( EntityFactory::CreateBullet( bulletDef, bulletPosition, bulletOrientationWithSpread, bulletType, FACTION_GOOD, bulletSpeedMultiplier, actorVelocity ) );
		//m_entities.push_back( new Bullet( bulletPosition, bulletOrientationWithSpread, bulletType, FACTION_GOOD, bulletDef, bulletSpeedMultiplier, actorVelocity ) );
	}

	shooter->SetIsFiring( false );
}

void Map::SpawnBossBullets( Actor* boss )
{
	UNUSED( boss );
}

void Map::PushEntityOutOfWalls(Entity* currentEntity)
{
	IntVec2 entityPostionInt(currentEntity->GetPosition());
	Vec2 entityPosition(currentEntity->GetPosition());

	//Grab all of the tile coordinates for tiles around the player
	IntVec2 leftPlayerPositionInt(entityPostionInt.x - 1,entityPostionInt.y);
	IntVec2 rightPlayerPositionInt(entityPostionInt.x + 1, entityPostionInt.y);
	IntVec2 upPlayerPositionInt(entityPostionInt.x, entityPostionInt.y + 1);
	IntVec2 downPlayerPositionInt(entityPostionInt.x, entityPostionInt.y - 1);
	IntVec2 topLeftPlayerPositionInt( entityPostionInt.x - 1, entityPostionInt.y + 1 );
	IntVec2 topRightPlayerPositionInt( entityPostionInt.x + 1, entityPostionInt.y + 1 );
	IntVec2 botttomLeftPlayerPositionInt( entityPostionInt.x - 1, entityPostionInt.y - 1 );
	IntVec2 bottomRightPlayerPositionInt( entityPostionInt.x + 1, entityPostionInt.y - 1 );

	const Tile& leftTile		= m_tiles[GetTileIndexForTileCoordinates( leftPlayerPositionInt )];
	const Tile& rightTile		= m_tiles[GetTileIndexForTileCoordinates( rightPlayerPositionInt)];
	const Tile& upTile			= m_tiles[GetTileIndexForTileCoordinates( upPlayerPositionInt )];
	const Tile& downTile		= m_tiles[GetTileIndexForTileCoordinates( downPlayerPositionInt )];
	const Tile& topLeftTile		= m_tiles[GetTileIndexForTileCoordinates( topLeftPlayerPositionInt )];
	const Tile& topRightTile	= m_tiles[GetTileIndexForTileCoordinates( topRightPlayerPositionInt )];
	const Tile& bottomLeftTile	= m_tiles[GetTileIndexForTileCoordinates( botttomLeftPlayerPositionInt )];
	const Tile& bottomRightTile = m_tiles[GetTileIndexForTileCoordinates( bottomRightPlayerPositionInt )];

	PushEntityOutOfWall( currentEntity, leftTile);
	PushEntityOutOfWall( currentEntity, rightTile);
	PushEntityOutOfWall( currentEntity, upTile);
	PushEntityOutOfWall( currentEntity, downTile);
	PushEntityOutOfWall( currentEntity, topLeftTile);
	PushEntityOutOfWall( currentEntity, topRightTile);
	PushEntityOutOfWall( currentEntity, bottomLeftTile);
	PushEntityOutOfWall( currentEntity, bottomRightTile);

	if( currentEntity->m_entityType == ENTITY_TYPE_GOOD_BULLET || currentEntity->m_entityType == ENTITY_TYPE_EVIL_BULLET )
	{
		Tile const& currentTile = m_tiles[GetTileIndexForTileCoordinates( entityPostionInt )];
		if( !currentTile.m_tileDef->m_allowsWalking )
		{
			currentEntity->m_isGarbage = true;
		}
	}
}


void Map::PushEntityOutOfWall( Entity* entityToPush, const Tile& tile )
{
	Vec2 entityPosition( entityToPush->GetPosition() );
	float physicsRadius = entityToPush->m_physicsRadius;
	AABB2 tileAABB2 = tile.GetAABB2();

	bool entityCanWalk = entityToPush->m_canWalk;
	bool entityCanFly = entityToPush->m_canFly;
	bool entityCanSwim = entityToPush->m_canSwim;

	bool tileAllowsWalking = tile.m_tileDef->m_allowsWalking;
	bool tileAllowsFlying = tile.m_tileDef->m_allowsFlying;
	bool TileAllowsSwimming = tile.m_tileDef->m_allowsSwimming;


	if( tileAllowsWalking && entityCanWalk )
	{
		return;
	}
	if( tileAllowsFlying && entityCanFly )
	{
		return;
	}
	if( TileAllowsSwimming && entityCanSwim )
	{
		return;
	}

	PushDiscOutOfAABB2D( entityPosition, physicsRadius, tileAABB2 );

	if( entityPosition != entityToPush->m_position )
	{
		entityToPush->m_hasCollided = true;
		if( entityToPush->m_entityType == ENTITY_TYPE_GOOD_BULLET || entityToPush->m_entityType == ENTITY_TYPE_EVIL_BULLET )
		{
			entityToPush->m_isGarbage = true;
		}
	}
	entityToPush->SetPosition(entityPosition);
}


void Map::PushEntitiesOutOfWalls()
{
	for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		if( !m_entities[entityIndex]->IsGarbage() )
		{
			//EntityType entityType = m_entities[entityIndex]->m_entityType;
			//if( entityType != ENTITY_TYPE_GOOD_BULLET &&  entityType != ENTITY_TYPE_EVIL_BULLET )
			{
				PushEntityOutOfWalls(m_entities[entityIndex]);
			}
		}
	}
}


void Map::ResolveEntitiesCollisions()
{
	for( size_t entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		Entity* currentEntity = m_entities[entityIndex];
		if( !currentEntity->IsGarbage() )
		{
			if( currentEntity->m_entityType == ENTITY_TYPE_PLAYER || currentEntity->m_entityType == ENTITY_TYPE_NPC_ENEMY || currentEntity->m_entityType == ENTITY_TYPE_BOSS )
			{
				ResolveEntityCollisions( currentEntity );
			}
		}
	}
}

void Map::ResolveEntityCollisions( Entity* currentEntity )
{
	if( !currentEntity->IsAlive() )
	{
		return;
	}
	EntityType typeToCheck;
	if( currentEntity->m_entityType == ENTITY_TYPE_PLAYER )
	{
		typeToCheck = ENTITY_TYPE_EVIL_BULLET;
		if( currentEntity->IsDodging() )
		{
			return;
		}
	}
	else
	{
		typeToCheck = ENTITY_TYPE_GOOD_BULLET;
	}
	for( size_t entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		if( !m_entities[entityIndex]->IsGarbage() )
		{
			if( m_entities[entityIndex]->m_entityType == typeToCheck )
			{
				if( AreEntitiesColliding( currentEntity, m_entities[entityIndex] ) )
				{
					Bullet* bullet = (Bullet*)m_entities[entityIndex];
					int bulletDamage = bullet->GetBulletDamage();
					currentEntity->LoseHealth( bulletDamage );
					if( !currentEntity->IsAlive() )
					{
						if( currentEntity->m_entityType == ENTITY_TYPE_NPC_ENEMY )
						{
							Actor* enemy = (Actor*)currentEntity;
							bool doesLootDrop = g_theGame->m_rand.RollPercentChance( 0.25f );
							if( doesLootDrop )
							{
								Loot* loot = EntityFactory::CreateLoot( enemy->GetPosition(), WeaponDefinition::GetRandomWeapon( g_theGame->m_rand ) );
								//Loot* loot = new Loot( enemy->GetPosition(), WeaponDefinition::GetRandomWeapon( g_theGame->m_rand ) );
								m_entities.push_back( loot );
							}
						}
					}
					//currentEntity->Lose1Health();
					m_entities[entityIndex]->m_isGarbage = true;
				}
			}
			else if( currentEntity->m_entityType == ENTITY_TYPE_PLAYER && m_entities[entityIndex]->m_entityType == ENTITY_TYPE_LOOT )
			{
				if( AreEntitiesColliding( currentEntity, m_entities[entityIndex] ) )
				{
					Actor* player = (Actor*)currentEntity;
					Loot* loot = (Loot*)m_entities[entityIndex];
					player->AddWeapon( loot->GetLootWeapon() );
					loot->m_isGarbage = true;
				}
			}
		}
	}
}

bool Map::AreEntitiesColliding( Entity* entityA, Entity* entityB )
{
	float radiusA = entityA->m_physicsRadius;
	Vec2 positionA = entityA->GetPosition();

	float radiusB = entityB->m_physicsRadius;
	Vec2 positionB = entityB->GetPosition();

	float distanceAToB = GetDistance2D( positionA, positionB );

	if( distanceAToB < radiusA + radiusB )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Map::UpdateEntityMouseIsTouching()
{
	for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		if( m_entities[entityIndex]->IsGarbage() )
		{
			continue;
		}
		SetMouseIsTouchingEntity( m_entities[entityIndex] );

		if( m_entityMouseIsTouching )
		{
			break;
		}
	}
}


void Map::UpdateTileMouseIsTouching()
{
	m_tileMouseIsTouching = nullptr;

	IntVec2 mouseTilePosition = IntVec2(g_theGame->GetMousePositionOnMainCamera());

	if( IsTilePositionValid( mouseTilePosition ) )
	{
		int tileIndex = GetTileIndexForTileCoordinates( mouseTilePosition );

		if( tileIndex )
		{
			m_tileMouseIsTouching = &(m_tiles[tileIndex]);
		}
	}
}


void Map::SetMouseIsTouchingEntity( Entity* entity )
{
	m_entityMouseIsTouching = nullptr;

	Vec2 mousePosition = g_theGame->GetMousePositionOnMainCamera();

	if( IsPointInsideDisc2D( mousePosition, entity->m_position, entity->m_physicsRadius ) )
	{
		m_entityMouseIsTouching = entity;
	}
}


void Map::GarbageCollectEntities()
{
	for( size_t entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		Entity* entity = m_entities[entityIndex];
		if( entity )
		{
			if( entity->IsGarbage() )
			{
				if( entity->m_entityType == ENTITY_TYPE_NPC_ENEMY )
				{
					Actor* enemy = (Actor*)entity;
					Loot* loot = EntityFactory::CreateLoot( enemy->GetPosition(), enemy->GetCurrentWeapon() );
					//Loot* loot = new Loot( enemy->GetPosition(), enemy->GetCurrentWeapon() );
					m_entities.push_back( loot );
				}

				EntityFactory::DeleteEntity( m_entities[entityIndex] );
				m_entities[entityIndex] = nullptr;
			}
			else
			{
				IntVec2 entityTileCoords = (IntVec2)entity->GetPosition();
				bool isValidPosition = IsValidTileCoordinates( entityTileCoords );
				if( !isValidPosition )
				{
					EntityFactory::DeleteEntity( m_entities[entityIndex] );
					m_entities[entityIndex] = nullptr;
				}
			}

		}
	}
}

void Map::AddPlayer( Actor* player, int playerSlot )
{
	Actor* playerToAdd = player;
	
	if( playerSlot < MaxNumPlayers )
	{
		if( m_entities[playerSlot] )
		{
			EntityFactory::DeleteEntity( m_entities[playerSlot] );
			//delete m_entities[playerSlot];
			m_entities[playerSlot] = nullptr;
		}

		if( nullptr != playerToAdd )
		{
			m_entities[playerSlot] = playerToAdd;
		}
		else
		{
			ActorDefinition* playerActorDef = ActorDefinition::s_definitions["Player"];
			//playerToAdd = new Actor( Vec2( 2.5, 2.5f ), Vec2( 0.f, 0.f ), 0.f, 0.f, playerActorDef, Player_1 );
			playerToAdd = EntityFactory::CreatePlayer( playerActorDef, Vec2( 2.5f, 2.5f ), Vec2(), 0.f, 0.f, (PlayerController)playerSlot );
			WeaponDefinition* pistolWeapon = WeaponDefinition::s_definitions["Pistol"];
			//WeaponDefinition* shotgunWeapon = WeaponDefinition::s_definitions["Shotgun"];
			playerToAdd->AddWeapon( pistolWeapon );
			//playerToAdd->AddWeapon( shotgunWeapon );

			m_entities[playerSlot] = playerToAdd;
		}
	}
	else
	{
		return;
	}



	for( size_t spawnerIndex = 0; spawnerIndex < m_enemySpawners.size(); spawnerIndex++ )
	{
		m_enemySpawners[spawnerIndex]->SetPlayer( playerToAdd );
	}

	playerToAdd->SetPosition( m_startPosition );
}

void Map::DeletePlayer()
{
	if( m_entities[0] )
	{
		EntityFactory::DeleteEntity( m_entities[0] );
		//delete m_entities[0];
		m_entities[0] = nullptr;
	}

}

bool Map::IsBossDead() const
{
	for( size_t entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		Entity* entity = m_entities[entityIndex];
		if( entity )
		{
			if( entity->m_entityType == ENTITY_TYPE_BOSS )
			{
				if( !entity->IsAlive() )
				{
					return true;
				}
			}
		}
	}

	return false;
}

int Map::GetBossHealth() const
{
	for( size_t entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		Entity* entity = m_entities[entityIndex];
		if( entity )
		{
			if( entity->m_entityType == ENTITY_TYPE_BOSS )
			{
				if( !entity->IsAlive() )
				{
					return -1;
				}
				else
				{
					return entity->GetHealth();
				}
			}
		}
	}

	return -1;
}

void Map::SpawnSpawnersLevel1()
{
	ActorDefinition* maryActorDef = ActorDefinition::s_definitions["Mary"];
	ActorDefinition* josenActorDef = ActorDefinition::s_definitions["Josen"];
	ActorDefinition* bossActorDef = ActorDefinition::s_definitions["Boss"];

	WeaponDefinition* pistolWeapon = WeaponDefinition::s_definitions["Pistol"];
	WeaponDefinition* smgWeapon = WeaponDefinition::s_definitions["SMG"];
	WeaponDefinition* rocketLauncherWeapon = WeaponDefinition::s_definitions["RocketLauncher"];
	WeaponDefinition* shotgunWeapon = WeaponDefinition::s_definitions["Shotgun"];
	WeaponDefinition* flamethrowerWeapon = WeaponDefinition::s_definitions["Flamethrower"];


	EnemySpawner* spawner = new EnemySpawner( Vec2( 14.f, 3.f ), FloatRange( 0.f, 1.f ), IntRange( 1, 2 ), 5.f, nullptr, this );
	EnemySpawner* spawner2 = new EnemySpawner( Vec2( 3.f, 9.f ), FloatRange( 0.f, 2.f ), IntRange( 1, 2 ), 5.f, nullptr, this );
	EnemySpawner* spawner3 = new EnemySpawner( Vec2( 26.f, 2.f ), FloatRange( 0.f, 3.f ), IntRange( 2, 4 ), 6.f, nullptr, this );
	EnemySpawner* spawner4 = new EnemySpawner( Vec2( 26.f, 10.f ), FloatRange( 0.f, 1.f ), IntRange( 3, 5 ), 5.f, nullptr, this );
	EnemySpawner* spawner5 = new EnemySpawner( Vec2( 20.f, 15.f ), FloatRange( 0.f, 1.f ), IntRange( 3, 5 ), 4.f, nullptr, this );
	EnemySpawner* spawner6 = new EnemySpawner( Vec2( 11.f, 14.f ), FloatRange( 0.f, 1.5f ), IntRange( 3, 5 ), 4.f, nullptr, this );
	EnemySpawner* spawner7 = new EnemySpawner( Vec2( 26.f, 25.f ), FloatRange( 0.f, 2.f ), IntRange( 3, 5 ), 5.f, nullptr, this );
	EnemySpawner* spawner8 = new EnemySpawner( Vec2( 17.f, 27.f ), FloatRange( 0.f, 2.f ), IntRange( 3, 5 ), 5.f, nullptr, this );


	spawner->AddEnemyType( josenActorDef );
	spawner->AddWeaponType( smgWeapon );
	spawner->AddWeaponType( shotgunWeapon );
	spawner2->AddEnemyType( josenActorDef );
	spawner2->AddWeaponType( smgWeapon );
	spawner2->AddWeaponType( shotgunWeapon );
	spawner3->AddEnemyType( josenActorDef );
	spawner3->AddWeaponType( smgWeapon );
	spawner3->AddWeaponType( shotgunWeapon );
	spawner5->AddEnemyType( maryActorDef );
	spawner6->AddEnemyType( maryActorDef );
	spawner7->AddEnemyType( maryActorDef );
	spawner8->AddEnemyType( maryActorDef );
	spawner4->AddEnemyType( josenActorDef );
	spawner5->AddEnemyType( josenActorDef );
	spawner4->AddWeaponType( shotgunWeapon );
	spawner5->AddWeaponType( pistolWeapon );
	spawner6->AddWeaponType( flamethrowerWeapon );
	spawner7->AddWeaponType( pistolWeapon );
	spawner8->AddWeaponType( smgWeapon );
	m_enemySpawners.push_back( spawner );
	m_enemySpawners.push_back( spawner2 );
	m_enemySpawners.push_back( spawner3 );
	m_enemySpawners.push_back( spawner4 );
	m_enemySpawners.push_back( spawner5 );
	m_enemySpawners.push_back( spawner6 );
	m_enemySpawners.push_back( spawner7 );
	m_enemySpawners.push_back( spawner8 );


	EnemySpawner* bossSpawner = new EnemySpawner( Vec2( 5.f, 25.f ), FloatRange( 0.f, 0.f ), IntRange( 1, 1 ), 5.f, nullptr, this );
	bossSpawner->AddEnemyType( bossActorDef );
	bossSpawner->AddWeaponType( rocketLauncherWeapon );
	m_enemySpawners.push_back( bossSpawner );
}

void Map::SpawnSpawnersLevel2()
{
	ActorDefinition* maryActorDef = ActorDefinition::s_definitions["Mary"];
	ActorDefinition* josenActorDef = ActorDefinition::s_definitions["Josen"];
	ActorDefinition* bossActorDef = ActorDefinition::s_definitions["Boss"];

	WeaponDefinition* pistolWeapon = WeaponDefinition::s_definitions["Pistol"];
	WeaponDefinition* smgWeapon = WeaponDefinition::s_definitions["SMG"];
	WeaponDefinition* rocketLauncherWeapon = WeaponDefinition::s_definitions["RocketLauncher"];
	WeaponDefinition* shotgunWeapon = WeaponDefinition::s_definitions["Shotgun"];
	WeaponDefinition* flamethrowerWeapon = WeaponDefinition::s_definitions["Flamethrower"];
	//WeaponDefinition* laserGun = WeaponDefinition::s_definitions["LaserGun"];

	EnemySpawner* spawner = new EnemySpawner( Vec2( 5.f, 31.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 5 ), 6.f, nullptr, this );
	EnemySpawner* spawner2 = new EnemySpawner( Vec2( 12.f, 32.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 6.f, nullptr, this );
	EnemySpawner* spawner3 = new EnemySpawner( Vec2( 25.f, 40.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 6.f, nullptr, this );
	EnemySpawner* spawner4 = new EnemySpawner( Vec2( 25.f, 31.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 5.f, nullptr, this );
	EnemySpawner* spawner5 = new EnemySpawner( Vec2( 38.f, 40.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 6.f, nullptr, this );
	EnemySpawner* spawner6 = new EnemySpawner( Vec2( 38.f, 32.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 6.f, nullptr, this );
	EnemySpawner* spawner7 = new EnemySpawner( Vec2( 36.f, 22.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 7.f, nullptr, this );
	EnemySpawner* spawner8 = new EnemySpawner( Vec2( 30.f, 22.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 7.f, nullptr, this );
	EnemySpawner* spawner9 = new EnemySpawner( Vec2( 20.f, 22.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 5.f, nullptr, this );
	EnemySpawner* spawner10 = new EnemySpawner( Vec2( 10.f, 22.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 6.f, nullptr, this );
	EnemySpawner* spawner11 = new EnemySpawner( Vec2( 9.f, 11.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 6.f, nullptr, this );
	EnemySpawner* spawner12 = new EnemySpawner( Vec2( 5.f, 5.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 7.f, nullptr, this );
	EnemySpawner* spawner13 = new EnemySpawner( Vec2( 13.f, 4.f ), FloatRange( 0.f, 5.f ), IntRange( 3, 6 ), 7.f, nullptr, this );

	spawner->AddEnemyType( josenActorDef );
	spawner->AddWeaponType( smgWeapon );
	spawner->AddWeaponType( shotgunWeapon );
	spawner2->AddEnemyType( josenActorDef );
	spawner2->AddWeaponType( smgWeapon );
	spawner2->AddWeaponType( shotgunWeapon );
	spawner3->AddEnemyType( josenActorDef );
	spawner3->AddWeaponType( smgWeapon );
	spawner3->AddWeaponType( shotgunWeapon );
	spawner5->AddEnemyType( maryActorDef );
	spawner6->AddEnemyType( maryActorDef );
	spawner7->AddEnemyType( maryActorDef );
	spawner8->AddEnemyType( maryActorDef );
	spawner4->AddEnemyType( josenActorDef );
	spawner5->AddEnemyType( josenActorDef );
	spawner9->AddEnemyType( josenActorDef );
	spawner9->AddEnemyType( maryActorDef );
	spawner10->AddEnemyType( josenActorDef );
	spawner10->AddEnemyType( maryActorDef );
	spawner11->AddEnemyType( josenActorDef );
	spawner11->AddEnemyType( maryActorDef );
	spawner12->AddEnemyType( josenActorDef );
	spawner12->AddEnemyType( maryActorDef );
	spawner13->AddEnemyType( josenActorDef );
	spawner13->AddEnemyType( maryActorDef );
	spawner4->AddWeaponType( shotgunWeapon );
	spawner5->AddWeaponType( pistolWeapon );
	spawner6->AddWeaponType( flamethrowerWeapon );
	spawner7->AddWeaponType( pistolWeapon );
	spawner8->AddWeaponType( smgWeapon );
	spawner8->AddWeaponType( flamethrowerWeapon );
	spawner9->AddWeaponType( pistolWeapon );
	spawner9->AddWeaponType( smgWeapon );
	spawner10->AddWeaponType( flamethrowerWeapon );
	spawner10->AddWeaponType( pistolWeapon );
	spawner11->AddWeaponType( smgWeapon );
	spawner11->AddWeaponType( flamethrowerWeapon );
	spawner12->AddWeaponType( pistolWeapon );
	spawner12->AddWeaponType( smgWeapon );
	spawner13->AddWeaponType( flamethrowerWeapon );
	spawner13->AddWeaponType( pistolWeapon );
	spawner13->AddWeaponType( smgWeapon );
	m_enemySpawners.push_back( spawner );
	m_enemySpawners.push_back( spawner2 );
	m_enemySpawners.push_back( spawner3 );
	m_enemySpawners.push_back( spawner4 );
	m_enemySpawners.push_back( spawner5 );
	m_enemySpawners.push_back( spawner6 );
	m_enemySpawners.push_back( spawner7 );
	m_enemySpawners.push_back( spawner8 );
	m_enemySpawners.push_back( spawner9 );
	m_enemySpawners.push_back( spawner10 );
	m_enemySpawners.push_back( spawner11 );
	m_enemySpawners.push_back( spawner12 );
	m_enemySpawners.push_back( spawner13 );


	EnemySpawner* bossSpawner = new EnemySpawner( Vec2( 35.f, 10.f ), FloatRange( 0.f, 0.f ), IntRange( 1, 1 ), 5.f, nullptr, this );
	bossSpawner->AddEnemyType( bossActorDef );
	bossSpawner->AddWeaponType( rocketLauncherWeapon );
	m_enemySpawners.push_back( bossSpawner );
}

void Map::SpawnSpawnersLevel3()
{
	ActorDefinition* maryActorDef = ActorDefinition::s_definitions["Mary"];
	ActorDefinition* josenActorDef = ActorDefinition::s_definitions["Josen"];
	ActorDefinition* bossActorDef = ActorDefinition::s_definitions["Boss"];

	WeaponDefinition* pistolWeapon = WeaponDefinition::s_definitions["Pistol"];
	WeaponDefinition* smgWeapon = WeaponDefinition::s_definitions["SMG"];
	WeaponDefinition* rocketLauncherWeapon = WeaponDefinition::s_definitions["RocketLauncher"];
	WeaponDefinition* shotgunWeapon = WeaponDefinition::s_definitions["Shotgun"];
	WeaponDefinition* flamethrowerWeapon = WeaponDefinition::s_definitions["Flamethrower"];

	EnemySpawner* spawner = new EnemySpawner( Vec2( 40.f, 53.f ), FloatRange( 0.f, 3.f ), IntRange( 4, 7 ), 8.f, nullptr, this );
	EnemySpawner* spawner2 = new EnemySpawner( Vec2( 20.f, 40.f ), FloatRange( 0.f, 1.f ), IntRange( 3, 5 ), 5.f, nullptr, this );
	EnemySpawner* spawner3 = new EnemySpawner( Vec2( 8.f, 55.f ), FloatRange( 0.f, 1.f ), IntRange( 2, 4 ), 5.f, nullptr, this );
	EnemySpawner* spawner4 = new EnemySpawner( Vec2( 8.f, 23.f ), FloatRange( 0.f, 1.f ), IntRange( 1, 2 ), 5.f, nullptr, this );
	EnemySpawner* spawner5 = new EnemySpawner( Vec2( 20.f, 8.f ), FloatRange( 0.f, 1.f ), IntRange( 1, 3 ), 4.f, nullptr, this );
	EnemySpawner* spawner6 = new EnemySpawner( Vec2( 34.f, 30.f ), FloatRange( 0.f, 1.5f ), IntRange( 2, 4 ), 4.f, nullptr, this );
	EnemySpawner* spawner7 = new EnemySpawner( Vec2( 51.f, 40.f ), FloatRange( 0.f, 2.f ), IntRange( 3, 5 ), 5.f, nullptr, this );

	spawner->AddEnemyType( josenActorDef );
	spawner->AddWeaponType( smgWeapon );
	spawner->AddWeaponType( shotgunWeapon );
	spawner2->AddEnemyType( josenActorDef );
	spawner2->AddWeaponType( smgWeapon );
	spawner2->AddWeaponType( shotgunWeapon );
	spawner3->AddEnemyType( josenActorDef );
	spawner3->AddWeaponType( smgWeapon );
	spawner3->AddWeaponType( shotgunWeapon );
	spawner5->AddEnemyType( maryActorDef );
	spawner6->AddEnemyType( maryActorDef );
	spawner7->AddEnemyType( maryActorDef );
	spawner4->AddEnemyType( josenActorDef );
	spawner5->AddEnemyType( josenActorDef );
	spawner4->AddWeaponType( shotgunWeapon );
	spawner5->AddWeaponType( pistolWeapon );
	spawner6->AddWeaponType( flamethrowerWeapon );
	spawner7->AddWeaponType( pistolWeapon );
	m_enemySpawners.push_back( spawner );
	m_enemySpawners.push_back( spawner2 );
	m_enemySpawners.push_back( spawner3 );
	m_enemySpawners.push_back( spawner4 );
	m_enemySpawners.push_back( spawner5 );
	m_enemySpawners.push_back( spawner6 );
	m_enemySpawners.push_back( spawner7 );

	EnemySpawner* bossSpawner = new EnemySpawner( Vec2( 45.f, 15.f ), FloatRange( 0.f, 0.f ), IntRange( 1, 1 ), 8.f, nullptr, this );
	bossSpawner->AddEnemyType( bossActorDef );
	bossSpawner->AddWeaponType( rocketLauncherWeapon );
	m_enemySpawners.push_back( bossSpawner );
}

TileMetaData& Map::GetTileMetaDataAtTilePosition( IntVec2 currentCoords )
{
	int tileIndex = GetTileIndexForTileCoordinates(currentCoords);
	return m_tileMetaData[tileIndex];
}

void Map::AddTagsAtPosition( const IntVec2& tileCoords, const Tags& tags )
{
	int tagIndex = GetTileIndexForTileCoordinates(tileCoords);
	m_tileMetaData[tagIndex].m_tags.AppendTags(tags);
}

Entity* Map::GetEntityWithID( int entityID ) const
{
	Entity* entityToReturn = nullptr;
	for( Entity* entity : m_entities )
	{
		if( entity )
		{
			int currentEntityID = entity->m_entityID;
			if( entityID == currentEntityID )
			{
				entityToReturn = entity;
				break;
			}
		}
	}

	return entityToReturn;
}

void Map::SpawnEntity( Entity* entityToSpawn )
{
	m_entities.push_back( entityToSpawn );
}

void Map::UpdateEntity( UpdateEntityMessage const& updateMessage )
{

	Vec2 position = updateMessage.position;
	Vec2 velocity = updateMessage.velocity;
	float orientation = updateMessage.orientationDegrees;
	float weaponOrientation = updateMessage.weaponOrientationDegrees;
	bool isDead = updateMessage.isDead;
	int entityID = updateMessage.entityID;
	int health = updateMessage.health;

// 	if( entityID > 4 )
// 	{
// 		std::string updateStr = Stringf( "Update Entity: %i, pos: %f, %f", entityID, position.x, position.y );
// 		g_theConsole->PrintString( Rgba8::GREEN, updateStr );
// 	}

	for( Entity* entity : m_entities )
	{
		if( entity )
		{
			if( entity->m_entityID == entityID )
			{
				entity->m_position = position;
				entity->m_velocity = velocity;
				entity->m_orientationDegrees = orientation;
				entity->m_weaponOrientationDegrees = weaponOrientation;
				entity->m_isDead = isDead;
				entity->m_health = health;

				break;
			}
		}
	}
}

void Map::DeleteEntity( DeleteEntityMessage const& deleteMessage )
{
	int entityID = deleteMessage.entityID;
	std::string updateStr = Stringf( "Delete Entity: %i", entityID );
	g_theConsole->PrintString( Rgba8::Tan, updateStr );

	for( size_t entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		if( m_entities[entityIndex] )
		{
			if( m_entities[entityIndex]->m_entityID == deleteMessage.entityID )
			{
				EntityFactory::DeleteEntity( m_entities[entityIndex] );
				m_entities[entityIndex] = nullptr;
			}
		}
	}
}

Tile* Map::GetTileAtPosition( const IntVec2& tileCoords )
{
	if( !IsTilePositionValid( tileCoords ) )
	{
		return nullptr;
	}

	int tileIndex = GetTileIndexForTileCoordinates(tileCoords);
	Tile* tileToGet = &m_tiles[tileIndex];
	return tileToGet;
}


void Map::SetTileAtPosition( const IntVec2& tileCoords, TileDefinition* tileDef )
{
	if( !IsTilePositionValid( tileCoords ) )
	{
		return;
	}
	Tile* tile = GetTileAtPosition(tileCoords);
	tile->SetTileType(tileDef);
}


bool Map::IsTilePositionValid( IntVec2 mouseTilePosition )
{
	if( mouseTilePosition.x >= 0 && mouseTilePosition.x < m_mapSize.x &&
		mouseTilePosition.y >= 0 && mouseTilePosition.y < m_mapSize.y)
	{
		return true;
	}
	return false;
}


void Map::RenderDebugMode()
{ 
	AABB2 cameraBox = g_theGame->GetUICamera();
	AABB2 UIBottom = cameraBox.GetBoxAtBottom(0.2f);
	AABB2 UIBottomTop = UIBottom.CarveBoxOffBottom(0.5f);
	AABB2 UIBottomTopRight = UIBottomTop.GetBoxAtRight(0.3f);
	UIBottomTopRight.MakeSquareFromBottomLeft(true);
	AABB2 UIBOttomRight = UIBottom.GetBoxAtRight(0.3f);
	//AABB2 portraitBox = AABB2(11.f,4.f,12.f,5.f);

	Actor* actor = (Actor*)(m_entityMouseIsTouching);

	if( m_entityMouseIsTouching )
	{
		std::string stringToPrint = Stringf("Entity Name: %s, time until moving: %f", actor->m_actorDef->m_name.c_str(), actor->m_timeUntilNextGoalPosition);
		//std::string stringToPrint = Stringf("Entity Name: ");
		g_theRenderer->DrawAlignedTextAtPosition(stringToPrint.c_str(), UIBOttomRight, 1.f, ALIGN_CENTER_LEFT);

		const Texture& portraitTexture = g_portraitSpriteSheet->GetTexture();
		g_theRenderer->BindTexture(&portraitTexture);
		g_theRenderer->DrawAABB2Filled(UIBottomTopRight,Rgba8::WHITE,actor->m_actorDef->m_spritePortraitUVs.mins, actor->m_actorDef->m_spritePortraitUVs.maxs);
		return;
	}

	if( m_tileMouseIsTouching )
	{
		const char* tileName = m_tileMouseIsTouching->m_tileDef->m_name.c_str();
		std::string stringToPrint = Stringf("Tile Name: %s", tileName);
		g_theRenderer->DrawAlignedTextAtPosition(stringToPrint.c_str(), UIBOttomRight, 1.f, ALIGN_CENTER_LEFT);

		const Texture& tileTexture = g_tileSpriteSheet->GetTexture();
		g_theRenderer->BindTexture( &tileTexture );
		g_theRenderer->DrawAABB2Filled( UIBottomTopRight, Rgba8::WHITE, m_tileMouseIsTouching->m_tileDef->m_spriteUVs.mins, m_tileMouseIsTouching->m_tileDef->m_spriteUVs.maxs );
		return;
	}
}

void Map::UpdateDebugMode()
{
	UpdateEntityMouseIsTouching();
	UpdateTileMouseIsTouching();
}

int Map::GetTileIndexForTileCoordinates( const IntVec2& tileCoords )
{
	if( !IsValidTileCoordinates( tileCoords ) )
	{
		return 0;
	}
	int tileIndex = tileCoords.x + m_mapSize.x * tileCoords.y;
	return tileIndex;
}
IntVec2 Map::GetTileCoordinatesForTileIndex( int tileIndex ) const
{
	return m_tiles[tileIndex].m_tileCoords;
}

bool Map::IsValidTileCoordinates( const IntVec2& tileCoords ) const
{
	//Check if min bounds
	if( tileCoords.x < 0 || tileCoords.y < 0 )
	{
		return false;
	}
	//Check max bounds
	if( tileCoords.x >= m_mapSize.x || tileCoords.y >= m_mapSize.y )
	{
		return false;
	}

	return true;
}

bool Map::IsTileSolid( IntVec2 const& tileCoords )
{
	if( IsValidTileCoordinates( tileCoords ) )
	{
		Tile const* tile = GetTileAtPosition( tileCoords );
		if( tile && tile->m_tileDef->m_allowsWalking )
		{
			return true;
		}
	}

	return false;
}
