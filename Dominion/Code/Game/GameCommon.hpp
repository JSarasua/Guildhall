#pragma once
#include "Engine/Platform/Window.hpp"

struct Vec2;
struct Rgba8;

class Game;

extern Window* g_theWindow;
extern Game* g_theGame;

constexpr float SQRT_2 = 1.41421356237f;

constexpr float CLIENT_ASPECT = 16.f/9.f;

//Given Constants

constexpr int INT_MAP_SIZE_X = 70;
constexpr int INT_MAP_SIZE_Y = 30;
constexpr int SAFEZONE = 3;
constexpr float GAME_CAMERA_X = 13.5f;
constexpr float GAME_CAMERA_Y = GAME_CAMERA_X/2.f;


constexpr float PLAYER_SPEED = 1.f;
constexpr float PLAYER_TURN_SPEED = 180.f;
constexpr float PLAYER_SIZE = 0.5f;
constexpr float PLAYER_PHYSICS_RADIUS = 0.3f;
constexpr float PLAYER_MAX_SPEED = 2.f;

//constexpr float CAMERAABLATIONPERSECOND = 1.f;





void DrawLine( Vec2 startPoint, Vec2 endPoint, Rgba8 color, float thickness );
void DrawRing( Vec2 center, float radius, Rgba8 color, float thickness );