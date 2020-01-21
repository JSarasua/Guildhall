#pragma once
#include "GameCommon.hpp"
#include "App.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Game.hpp"

App* g_theApp = nullptr;
Game* g_theGame = nullptr;
RenderContext* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
SpriteSheet* g_tileSpriteSheet = nullptr;
SpriteSheet* g_actorSpriteSheet = nullptr;
const char* IMAGETESTPATH = "Data/Images/Test_StbiAndDirectX.png";






