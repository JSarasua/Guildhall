#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include"Game/App.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"



//Removed includes since they didn't seem necessary
//#include <math.h>
//#include <cassert>
//#include <crtdbg.h>


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move each of these items to its proper place, once that place is established
// 

const char* APP_NAME = "SD1-A4: Protogame2D";	// ...becomes ??? (Change this per project!)

extern App* g_theApp;
extern InputSystem* g_theInput;

extern Window* g_theWindow;




//-----------------------------------------------------------------------------------------------
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
// void CreateOSWindow( void* applicationInstanceHandle, float clientAspect )
// {
// 
// 
// /*	g_displayDeviceContext = GetDC( g_hWnd );*/
// 
// }



//------------------------------------------------------------------------------------------------
// Given an existing OS Window, create a Rendering Context (RC) for, say, OpenGL to draw to it.
//
// void CreateRenderContext()
// {
// 	// Creates an OpenGL rendering context (RC) and binds it to the current window's device context (DC)
// 	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
// 	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
// 	pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
// 	pixelFormatDescriptor.nVersion = 1;
// 	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
// 	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
// 	pixelFormatDescriptor.cColorBits = 24;
// 	pixelFormatDescriptor.cDepthBits = 24;
// 	pixelFormatDescriptor.cAccumBits = 0;
// 	pixelFormatDescriptor.cStencilBits = 8;
// 
// 	int pixelFormatCode = ChoosePixelFormat( g_displayDeviceContext, &pixelFormatDescriptor );
// 	SetPixelFormat( g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
// 	g_openGLRenderingContext = wglCreateContext( g_displayDeviceContext );
// 	wglMakeCurrent( g_displayDeviceContext, g_openGLRenderingContext );
// }








//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( _In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE, _In_ LPSTR commandLineString, _In_ int )
{
	UNUSED( commandLineString );
	UNUSED( applicationInstanceHandle );


	g_theApp = new App();
	g_theWindow = new Window();
	g_theWindow->Open(APP_NAME, CLIENT_ASPECT, 0.90f);

	g_theApp->Startup();
	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting() )
	{
		//Sleep( 16 );
		g_theWindow->BeginFrame();
		g_theApp->RunFrame();


	}

	//Need to allow g_app to call its deletes
	g_theApp->Shutdown();
	delete g_theApp;

	g_theWindow->Close();
	delete g_theWindow;
	g_theWindow = nullptr;
	return 0;
}


