#pragma once

struct Vec2;
struct Rgba8;

class App;
class Game;
class RenderContext;
class InputSystem;
class SpriteSheet;


constexpr float CLIENT_ASPECT = 16.f/9.f;

//Given Constants

constexpr int INT_MAP_SIZE_X = 70;
constexpr int INT_MAP_SIZE_Y = 30;
constexpr int SAFEZONE = 3;
constexpr float GAME_CAMERA_X = 13.5f;
constexpr float GAME_CAMERA_Y = GAME_CAMERA_X/2.f;


constexpr float UI_CAMERA_Y = 90.f;
constexpr float UI_CAMERA_X = UI_CAMERA_Y * CLIENT_ASPECT;
constexpr float TESTTEXTLINEHEIGHT = 1.f;
extern const char* IMAGETESTPATH;// = "/Data/Images/Test_StbiAndDirectX.png";


//constexpr float CAMERAABLATIONPERSECOND = 1.f;

extern App* g_theApp;
extern Game* g_theGame;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;
extern SpriteSheet* g_tileSpriteSheet;
extern SpriteSheet* g_actorSpriteSheet;


