#pragma once
struct Vec2;
struct Rgba8;
struct IntVec2;

class App;
class Game;
class Window;
class RenderContext;
class AudioSystem;
/*class InputSystem;*/
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

extern const IntVec2 STEP_NORTH;
extern const IntVec2 STEP_WEST;
extern const IntVec2 STEP_SOUTH;
extern const IntVec2 STEP_EAST;

extern const IntVec2 STEP_NORTHWEST;
extern const IntVec2 STEP_SOUTHWEST;
extern const IntVec2 STEP_SOUTHEAST;
extern const IntVec2 STEP_NORTHEAST;

extern App* g_theApp;
extern Game* g_theGame;
extern Window* g_theWindow;
extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudio;
/*extern InputSystem* g_theInput;*/
extern SpriteSheet* g_tileSpriteSheet;
extern SpriteSheet* g_actorSpriteSheet;
extern SpriteSheet* g_portraitSpriteSheet;
extern SpriteSheet* g_weaponSpriteSheet;
extern SpriteSheet* g_bulletsSpriteSheet;


