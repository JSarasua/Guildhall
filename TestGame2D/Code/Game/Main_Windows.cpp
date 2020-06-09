#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include"Game/App.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Platform/Window.hpp"

extern App* g_theApp;
extern Window* g_theWindow;


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( _In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE, _In_ LPSTR commandLineString, _In_ int )
{
	UNUSED( commandLineString );

	//Load Blackboard
	g_gameConfigBlackboard = new NamedStrings();
	XmlDocument* gameConfigDoc = new XmlDocument;
	const XmlElement& gameConfigRootElement = GetRootElement(*gameConfigDoc,"Data/GameConfig.xml" );
	g_gameConfigBlackboard->PopulateFromXmlElementAttributes(gameConfigRootElement);


	UNUSED( commandLineString );
	UNUSED( applicationInstanceHandle );


	g_theApp = new App();
	g_theApp->Startup();
	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting() )
	{
		//Sleep( 16 );

		g_theApp->RunFrame();


	}

	//Need to allow g_app to call its deletes
	g_theApp->Shutdown();
	delete g_theApp;

	g_theWindow->Close();
	delete g_theWindow;
	g_theWindow = nullptr;
	delete g_gameConfigBlackboard;
	delete gameConfigDoc;
	return 0;
}


