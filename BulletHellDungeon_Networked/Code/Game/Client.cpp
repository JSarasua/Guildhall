#include "Game/Client.hpp"
#include "Game/Game.hpp"


Client::Client()
{

}

Client::~Client()
{

}

void Client::Startup()
{

}

void Client::Shutdown()
{

}

void Client::BeginFrame()
{

}

void Client::EndFrame()
{

}

void Client::Update( float deltaSeconds )
{

}

void Client::Render()
{
	g_theGame->Render();
}

