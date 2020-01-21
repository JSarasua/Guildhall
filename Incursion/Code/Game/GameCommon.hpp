#pragma once

struct Vec2;
struct Rgba8;


constexpr float CLIENT_ASPECT = 16.f/9.f;

//Given Constants

constexpr int INT_MAP_SIZE_X_1 = 20;
constexpr int INT_MAP_SIZE_Y_1 = 20;
constexpr int INT_MAP_SIZE_X_2 = 15;
constexpr int INT_MAP_SIZE_Y_2 = 35;
constexpr int INT_MAP_SIZE_X_3 = 35;
constexpr int INT_MAP_SIZE_Y_3 = 15;

constexpr int LEVEL_ONE_TURRET_COUNT	= 3;
constexpr int LEVEL_ONE_TANK_COUNT		= 1;
constexpr int LEVEL_ONE_BOULDER_COUNT	= 5;
constexpr int LEVEL_TWO_TURRET_COUNT	= 5;
constexpr int LEVEL_TWO_TANK_COUNT		= 2;
constexpr int LEVEL_TWO_BOULDER_COUNT	= 10;
constexpr int LEVEL_THREE_TURRET_COUNT	= 10;
constexpr int LEVEL_THREE_TANK_COUNT	= 5;
constexpr int LEVEL_THREE_BOULDER_COUNT	= 15;




constexpr int SAFEZONE = 3;
constexpr float GAME_CAMERA_X = 13.5f;
constexpr float GAME_CAMERA_Y = GAME_CAMERA_X/2.f;

constexpr int PLAYERLIVES = 3;
constexpr int NPCTANK_HEALTH = 3;
constexpr int PLAYER_HEALTH = 5;
constexpr int NPCTURRET_HEALTH = 3;
constexpr int BOULDER_HEALTH = 99;
constexpr int WORMCOUNT = 10;
constexpr int MAXWORMLENGTH = 20;


constexpr float PLAYER_SPEED = 1.f;
constexpr float PLAYER_TURN_SPEED = 90.f;
constexpr float PLAYER_TURRET_TURN_SPEED = 180.f;
constexpr float PLAYER_COSMETIC_RADIUS = 0.5f;
constexpr float PLAYER_PHYSICS_RADIUS = 0.3f;
constexpr float PLAYER_MAX_SPEED = 2.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.03f;
constexpr float BULLET_COSMETIC_RADIUS = 0.05f;
constexpr float BULLET_SPEED = 10.f;
constexpr float BOULDER_PHYSICS_RADIUS = 0.2f;
constexpr float BOULDER_COSMETIC_RADIUS = 0.3f;
constexpr float NPCTURRET_PHYSICS_RADIUS = 0.4f;
constexpr float NPCTURRET_COSMETIC_RADIUS = 0.5f;
constexpr float NPCTURRET_ANGULARVELOCITY = 15.f;
constexpr float NPCTURRET_APERTURE_DEGREES = 90.f;
constexpr float NPCTURRET_MAXRANGE = 15.f;
constexpr float NPCTANK_PHYSICS_RADIUS = 0.4f;
constexpr float NPCTANK_COSMETIC_RADIUS = 0.5f;
constexpr float NPCTANK_SPEED = 0.3f;
constexpr float NPCTANK_ANGULARVELOCITY = 30.f;
constexpr float NPCTANK_MAXRANGE = 10.f;
constexpr float NPCTANK_WHISKERRANGE = 0.5f;
constexpr float NPC_TANK_MIDDLEWHISKERRANGE = 0.7f;
constexpr float RAYCAST_STEP = 0.01f;
constexpr float EXPLOSION_SIZE = 0.5f;
constexpr float EXPLOSION_DURATION = 0.5f;



//constexpr float CAMERAABLATIONPERSECOND = 1.f;





void DrawLine( Vec2 startPoint, Vec2 endPoint, Rgba8 color, float thickness );
void DrawRing( Vec2 center, float radius, Rgba8 color, float thickness );