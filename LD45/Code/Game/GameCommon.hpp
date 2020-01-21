#pragma once

struct Vec2;
struct Rgba8;


constexpr float CLIENT_ASPECT = 2.0f;

//Given Constants
constexpr int NUM_STARTING_ASTEROIDS = 6;
constexpr int MAX_ASTEROIDS = 20;
constexpr int MAX_BULLETS = 1000;
constexpr int MAX_DEBRIS = 1000;
constexpr int MAX_BEETLES = 100;
constexpr int MAX_WASPS = 100;
constexpr int MAX_POWERUPS = 3;
constexpr int BEETLE_HEALTH = 2;
constexpr int WASP_HEALTH = 2;
constexpr int BULLET_HEALTH = 1;
constexpr int BOUNCING_BULLET_HEALTH = 2;
constexpr int PLAYER_SHIP_HEALTH = 1;
constexpr int ASTEROID_HEALTH = 1;
constexpr int INTWORLD_SIZE_X = 40;
constexpr int INTWORLD_SIZE_Y = 20;
constexpr int INTWORLD_SIZE_PIECES = 5;
constexpr int SAFEZONE = 30;
constexpr int MAXPLAYERCOUNT = 4;
constexpr unsigned char TILECOLORFLUTTER = 50;
constexpr float PLAYER_SPEED = 10.f;
constexpr float PLAYER_SIZE = 1.f;
constexpr float WORLD_SIZE_X = 200.f;
constexpr float WORLD_SIZE_Y = 100.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;
constexpr float ASTEROID_SPEED = 10.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 1.6f;
constexpr float ASTEROID_COSMETIC_RADIUS = 2.0f;
constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 50.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f;
constexpr float PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 1.75f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;
constexpr float BEETLE_SPEED = 10.f;
constexpr float BEETLE_PHYSICS_RADIUS = 0.5f;
constexpr float BEETLE_COSMETIC_RADIUS = 2.0f;
constexpr float WASP_PHYSICS_RADIUS = 1.5f;
constexpr float WASP_COSMETIC_RADIUS = 2.0f;
constexpr float WASP_ACCELERATION = 100.f;
constexpr float WASP_MAXSPEED = 25.f;
constexpr float CAMERAABLATIONPERSECOND = 1.f;
constexpr float POWERUP_PHYSICALRADIUS = 2.f;
constexpr float POWERUP_SPEED = 10.f;
constexpr float RAPID_FIRE_PER_SECOND = 0.1f;
constexpr float DEBRIS_COSMETIC_RADIUS = 0.5f;




void DrawLine( Vec2 startPoint, Vec2 endPoint, Rgba8 color, float thickness );
void DrawRing( Vec2 center, float radius, Rgba8 color, float thickness );