#include "Engine/Console/DevConsole.hpp"
//#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
//#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"


ColoredLine::ColoredLine( const Rgba8& textColor, const std::string& devConsolePrintString ):
	m_textColor( textColor ),
	m_devConsolePrintString( devConsolePrintString )
{
}


DevConsole::DevConsole():
	m_currentColoredLine(Rgba8::WHITE,std::string("")),
	m_caret(Rgba8::WHITE, std::string("^"))
{

}

void DevConsole::Startup()
{
	g_theEventSystem->SubscribeToEvent("NoCall", InvalidCommand);
	g_theEventSystem->SubscribeToEvent("help",ListCommands);
}

void DevConsole::BeginFrame()
{

}

void DevConsole::Update(  float deltaSeconds  )
{
	if( m_isOpen )
	{
		m_caretTimer += deltaSeconds;
		if( m_caretTimer > 0.5f )
		{
			m_caretTimer = 0.f;
			m_isCaretRendering = !m_isCaretRendering;
		}
	}
}

void DevConsole::EndFrame()
{

}

void DevConsole::Shutdown()
{

}

void DevConsole::HandleKeyStroke( unsigned char keyStroke )
{
	if( !m_isOpen )
	{
		return;
	}
	m_isCaretRendering = true;
	m_caretTimer = 0.f;

	if( keyStroke == ENTER_KEY )		// Enter Key
	{
		PringString( m_currentColoredLine.m_textColor, m_currentColoredLine.m_devConsolePrintString );
		g_theEventSystem->FireEvent( m_currentColoredLine.m_devConsolePrintString, nullptr );
		m_commandHistory.push_back(m_currentColoredLine);
		if( !m_isCommandValid )
		{
			std::string command("Invalid Command:" + m_currentColoredLine.m_devConsolePrintString);
			PringString( Rgba8::WHITE, command);
			m_isCommandValid = true;

		}
		m_currentColoredLine.m_devConsolePrintString = std::string( "" );
		m_currentCharIndex = 0;
		m_currentPreviousLineIndex = (int)m_commandHistory.size();
	}
	else if( keyStroke == 0x08 ) //Backspace
	{
		if( !m_currentColoredLine.m_devConsolePrintString.empty() )
		{
			if( m_currentCharIndex > 0 )
			{
				m_currentColoredLine.m_devConsolePrintString.erase(m_currentCharIndex-1,1);
				m_currentCharIndex--;
				m_currentCharIndex = ClampInt( m_currentCharIndex, 0, (int)m_currentColoredLine.m_devConsolePrintString.size() );
			}

		}
	}
	else if( keyStroke == DEL_KEY ) //Delete
	{
		if( !m_currentColoredLine.m_devConsolePrintString.empty() )
		{
			if( m_currentCharIndex < m_currentColoredLine.m_devConsolePrintString.size() )
			{
				m_currentColoredLine.m_devConsolePrintString.erase( m_currentCharIndex, 1 );
			}

		}
	}
	else if( keyStroke == 0x60 ||keyStroke == 0xC0 )	//tilde
	{
		//m_isOpen = false;
	}
	else if( keyStroke == 0x1B )	//ESC key
	{
		SetIsOpen(false);
		m_currentCharIndex = 0;
	}
	else if( keyStroke == LEFT_KEY || keyStroke == RIGHT_KEY || keyStroke == UP_KEY || keyStroke == DOWN_KEY )
	{
		if( keyStroke == LEFT_KEY )
		{
			m_currentCharIndex--;
			m_currentCharIndex = ClampInt(m_currentCharIndex, 0, (int)m_currentColoredLine.m_devConsolePrintString.size());
		}
		else if( keyStroke == RIGHT_KEY )
		{
			m_currentCharIndex++;
			m_currentCharIndex = ClampInt( m_currentCharIndex, 0, (int)m_currentColoredLine.m_devConsolePrintString.size() );
		}
		else if( keyStroke == UP_KEY )
		{
			if( !m_commandHistory.empty() )
			{
				m_currentPreviousLineIndex--;
				m_currentPreviousLineIndex = ClampInt( m_currentPreviousLineIndex, 0, (int)m_commandHistory.size() );

				m_currentColoredLine = m_commandHistory[m_currentPreviousLineIndex];
				m_currentCharIndex = (int)m_currentColoredLine.m_devConsolePrintString.size();
			}
		}
		else if( keyStroke == DOWN_KEY )
		{
			m_currentPreviousLineIndex++;
			m_currentPreviousLineIndex = ClampInt( m_currentPreviousLineIndex, 0, (int)m_commandHistory.size() );
			if( m_currentPreviousLineIndex < (int)m_commandHistory.size() )
			{
				m_currentColoredLine = m_commandHistory[m_currentPreviousLineIndex];
				m_currentCharIndex = (int)m_currentColoredLine.m_devConsolePrintString.size();
			}
		}
	}
	else if( keyStroke == PGUP_KEY )
	{
		m_currentScrollIndex--;
		ClampScrollIndex();
	}
	else if( keyStroke == PGDOWN_KEY )
	{
		m_currentScrollIndex++;
		ClampScrollIndex();
	}
	else
	{
		m_currentColoredLine.m_devConsolePrintString.insert(m_currentCharIndex, 1, keyStroke);
		m_currentCharIndex++;
		m_currentCharIndex = ClampInt(m_currentCharIndex, 0, (int)m_currentColoredLine.m_devConsolePrintString.size());
	}


}

void DevConsole::PringString( const Rgba8& textColor, const std::string& devConsolePrintString )
{
	m_coloredLines.push_back(ColoredLine(textColor,devConsolePrintString));
	m_currentCharIndex = 0;
	m_currentScrollIndex++;
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

	if( m_isCaretRendering )
	{
		Vec2 caretDrawPosition = currentDrawPosition;
		caretDrawPosition.x += lineHeight * m_currentCharIndex + 0.5f * lineHeight;
		renderer.DrawTextAtPosition( m_caret.m_devConsolePrintString.c_str(), caretDrawPosition, lineHeight, m_caret.m_textColor );
	}
	currentDrawPosition.y += lineHeight;

	std::string currentLine = std::string(">") + m_currentColoredLine.m_devConsolePrintString;
	renderer.DrawTextAtPosition(currentLine.c_str(), currentDrawPosition, lineHeight, m_currentColoredLine.m_textColor);
	currentDrawPosition.y += lineHeight;

	//int textIndex = (int)m_coloredLines.size() - 1;
	int textIndex = m_currentScrollIndex;
	while( (currentDrawPosition.y < camera.GetOrthoTopRight().y) && (textIndex >= 0) )
	{
		currentLine = std::string(">") + m_coloredLines[textIndex].m_devConsolePrintString;
		renderer.DrawTextAtPosition(currentLine.c_str(), currentDrawPosition, lineHeight, m_coloredLines[textIndex].m_textColor);

		currentDrawPosition.y += lineHeight;
		textIndex--;
	}
}

void DevConsole::SetIsOpen( bool isOpen )
{
	m_isOpen = isOpen;

	if( !m_isOpen )
	{
		m_currentColoredLine.m_devConsolePrintString.clear();
		m_currentCharIndex = 0;
	}
	else if( m_isOpen )
	{
		m_currentScrollIndex = (int)m_coloredLines.size() - 1;
	}
}

bool DevConsole::InvalidCommand( const EventArgs* args )
{
	UNUSED(args);

	g_theConsole->m_isCommandValid = false;
	return true;
}

bool DevConsole::ListCommands( const EventArgs* args )
{
	UNUSED(args);

	std::vector<EventSubscription*> events = g_theEventSystem->m_eventSubscriptions;

	for( size_t commandIndex = 0; commandIndex < events.size(); commandIndex++ )
	{
		std::string& commandString = events[commandIndex]->m_eventName;
		g_theConsole->PringString(Rgba8::WHITE, commandString);
	}

	return true;
}

void DevConsole::ClampScrollIndex()
{
	m_currentScrollIndex = ClampInt(m_currentScrollIndex, 0, (int)m_coloredLines.size() - 1);
}

