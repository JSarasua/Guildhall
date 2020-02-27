#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include"Game/App.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"



extern App* g_theApp;
extern InputSystem* g_theInput;
extern Window* g_theWindow;


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( _In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE, _In_ LPSTR commandLineString, _In_ int )
{
	UNUSED( commandLineString );
	UNUSED( applicationInstanceHandle );

	//Will eventually move
// 	CreateOSWindow( applicationInstanceHandle, CLIENT_ASPECT );	// #SD1ToDo: replace these two lines...
// 	CreateRenderContext();										// #SD1ToDo: ...with the two lines below


	g_theApp = new App();
	g_theApp->Startup();
	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting() )
	{
		//Sleep( 16 );
/*		RunMessagePump();*/
		g_theApp->RunFrame();

		//Moved here to stop using TheApp_EndFrame();
		//SwapBuffers( g_displayDeviceContext ); // Note: call this once at the end of each frame
	}

	//Need to allow g_app to call its deletes
	g_theApp->Shutdown();
	delete g_theApp;

	g_theWindow->Close();
	delete g_theWindow;
	g_theWindow = nullptr;
	return 0;
}


