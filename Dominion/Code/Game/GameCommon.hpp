#pragma once
#include "Engine/Platform/Window.hpp"
#include "Game/CardDefinition.hpp"

class RenderContext;

struct Vec2;
struct Rgba8;

class Game;

extern Window* g_theWindow;
extern RenderContext* g_theRenderer;
extern Game* g_theGame;

constexpr float SQRT_2 = 1.41421356237f;

constexpr float CLIENT_ASPECT = 16.f/9.f;


int constexpr GAMENOTOVER	= -1;
int constexpr PLAYER_1		= 0;
int constexpr PLAYER_2		= 1;
int constexpr TIE			= 3;

int constexpr MONEYPILESIZE = 50;
int constexpr VPPileSize = 8;
int constexpr CURSEPILESIZE = 10;
int constexpr ACTIONPILESIZE = 10;

int constexpr NUMBEROFPILES = (int)NUM_CARDS;

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


void DrawLine( Vec2 startPoint, Vec2 endPoint, Rgba8 color, float thickness );
void DrawRing( Vec2 center, float radius, Rgba8 color, float thickness );