#include "Engine/Console/DevConsole.hpp"
//#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"


ColoredLine::ColoredLine( const Rgba8& textColor, const std::string& devConsolePrintString ):
	m_textColor( textColor ),
	m_devConsolePrintString( devConsolePrintString )
{
}


DevConsole::DevConsole():
	m_currentColoredLine(Rgba8::WHITE,std::string(""))
{

}

void DevConsole::Startup()
{

}

void DevConsole::BeginFrame()
{

}

void DevConsole::Update( InputSystem* input )
{
	if( m_isOpen )
	{
		const KeyButtonState* keyStates = input->GetAllKeyStates();

		for( int keyIndex = 0; keyIndex < NUM_KEYCODES; keyIndex++ )
		{
			const KeyButtonState& currentKeyState = keyStates[keyIndex];
			if( currentKeyState.WasJustPressed() )
			{
				if( keyIndex == 0x0D )		// Enter Key
				{
					PringString(m_currentColoredLine.m_textColor, m_currentColoredLine.m_devConsolePrintString);
					g_theEventSystem->FireEvent(m_currentColoredLine.m_devConsolePrintString, nullptr);
					m_currentColoredLine.m_devConsolePrintString = std::string("");
				}
				else if( keyIndex == 0x08 ) //Backspace
				{
					m_currentColoredLine.m_devConsolePrintString.pop_back();
				}
				else if( (keyIndex >= 0x30 && keyIndex <= 0x39) || (keyIndex >= 0x41 && keyIndex <= 0x5A) || (keyIndex == 0x20) ) //Check for Numbers, characters, and Space Bar
				{
					m_currentColoredLine.m_devConsolePrintString += (const char)keyIndex;

				}
			}
		}
	}
}

void DevConsole::EndFrame()
{

}

void DevConsole::Shutdown()
{

}

void DevConsole::PringString( const Rgba8& textColor, const std::string& devConsolePrintString )
{
	m_coloredLines.push_back(ColoredLine(textColor,devConsolePrintString));
}

void DevConsole::Render( RenderContext& renderer, const Camera& camera, float lineHeight ) const
{
	if( !m_isOpen )
	{
		return;
	}
	AABB2 cameraAABB(camera.GetOrthoBottomLeft(),camera.GetOrthoTopRight());
	renderer.BindTexture(nullptr);
	renderer.DrawAABB2Filled(cameraAABB,Rgba8(0,0,0,128));

	Vec2 currentDrawPosition = camera.GetOrthoBottomLeft();
	renderer.DrawTextAtPosition(m_currentColoredLine.m_devConsolePrintString.c_str(), currentDrawPosition, lineHeight, m_currentColoredLine.m_textColor);
	currentDrawPosition.y += lineHeight;

	int textIndex = (int)m_coloredLines.size() - 1;
	while( (currentDrawPosition.y < camera.GetOrthoTopRight().y) && (textIndex >= 0) )
	{
		renderer.DrawTextAtPosition(m_coloredLines[textIndex].m_devConsolePrintString.c_str(), currentDrawPosition, lineHeight, m_coloredLines[textIndex].m_textColor);

		currentDrawPosition.y += lineHeight;
		textIndex--;
	}
}

void DevConsole::SetIsOpen( bool isOpen )
{
	m_isOpen = isOpen;
}




