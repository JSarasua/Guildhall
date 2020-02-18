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
	KeyButtonState const& lctrlKey = g_theInput->GetKeyStates(LCTRL_KEY);
	KeyButtonState const& rctrlKey = g_theInput->GetKeyStates(RCTRL_KEY);
	KeyButtonState const& ctrlKey = g_theInput->GetKeyStates(CTRL_KEY);


	m_isCaretRendering = true;
	m_caretTimer = 0.f;

	if( keyStroke == ENTER_KEY )
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

		ResetSelection();
	}
	else if( keyStroke == BACKSPACE_KEY )
	{
		if( !m_currentColoredLine.m_devConsolePrintString.empty() )
		{
			if( m_beginSelect != m_endSelect )
			{
				EraseSelectedChars();
			}
			else if( m_currentCharIndex > 0 )
			{
				m_currentColoredLine.m_devConsolePrintString.erase(m_currentCharIndex-1,1);
				m_currentCharIndex--;
				ClampCurrentLine();
			}

		}
	}
	else if( keyStroke == DEL_KEY )
	{
		if( !m_currentColoredLine.m_devConsolePrintString.empty() )
		{
			if( m_beginSelect != m_endSelect )
			{
				EraseSelectedChars();
			}
			else if( m_currentCharIndex < m_currentColoredLine.m_devConsolePrintString.size() )
			{
				m_currentColoredLine.m_devConsolePrintString.erase( m_currentCharIndex, 1 );
			}

		}
	}
	else if( keyStroke == 0x60 ||keyStroke == TILDE_KEY )	//tilde
	{
		//m_isOpen = false;
	}
	else if( keyStroke == ESC_KEY )	//ESC key
	{
		SetIsOpen(false);
		m_currentCharIndex = 0;
	}
	else if( keyStroke == LEFT_KEY || keyStroke == RIGHT_KEY || keyStroke == UP_KEY || keyStroke == DOWN_KEY )
	{
		if( keyStroke == LEFT_KEY )
		{
			if( lctrlKey.IsPressed() || rctrlKey.IsPressed() || ctrlKey.IsPressed() )
			{
				if( m_beginSelect == 0 && m_endSelect == 0 )
				{
					m_beginSelect = m_currentCharIndex;
					m_endSelect = m_beginSelect - 1;
				}
				else
				{
					m_endSelect--;
				}
			}
			else
			{
				ResetSelection();
			}

			m_currentCharIndex--;
			ClampCurrentLine();
		}
		else if( keyStroke == RIGHT_KEY )
		{
			if( lctrlKey.IsPressed() || rctrlKey.IsPressed() || ctrlKey.IsPressed() )
			{
				if( m_beginSelect == 0 && m_endSelect == 0 )
				{
					m_beginSelect = m_currentCharIndex;
					m_endSelect = m_beginSelect + 1;
				}
				else
				{
					m_endSelect++;
				}
			}
			else
			{
				ResetSelection();
			}

			m_currentCharIndex++;
			ClampCurrentLine();
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
	else if( keyStroke == HOME_KEY )
	{
		if( lctrlKey.IsPressed() || rctrlKey.IsPressed() || ctrlKey.IsPressed() )
		{
			m_beginSelect = m_currentCharIndex;
			m_endSelect = 0;
		}
		else
		{
			ResetSelection();
		}
		m_currentCharIndex = 0;
	}
	else if( keyStroke == END_KEY )
	{
		if( lctrlKey.IsPressed() || rctrlKey.IsPressed() || ctrlKey.IsPressed() )
		{
			m_beginSelect = m_currentCharIndex;
			m_endSelect = (int)m_currentColoredLine.m_devConsolePrintString.size();
		}
		else
		{
			ResetSelection();
		}
		m_currentCharIndex = (int)m_currentColoredLine.m_devConsolePrintString.size();
	}
	else
	{
		EraseSelectedChars();

		m_currentColoredLine.m_devConsolePrintString.insert(m_currentCharIndex, 1, keyStroke);
		m_currentCharIndex++;
		m_selectedChars = 0;
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

	/************************************************************************/
	/* Render Caret                                                         */
	/************************************************************************/
	if( m_isCaretRendering )
	{
		Vec2 caretDrawPosition = currentDrawPosition;
		caretDrawPosition.x += lineHeight * m_currentCharIndex + 0.5f * lineHeight;
		renderer.DrawTextAtPosition( m_caret.m_devConsolePrintString.c_str(), caretDrawPosition, lineHeight, m_caret.m_textColor );
	}
	currentDrawPosition.y += lineHeight;

	/************************************************************************/
	/* Render Selected text box                                             */
	/************************************************************************/
	AABB2 selectionBox;
	selectionBox.mins = Vec2( m_beginSelect*lineHeight + lineHeight, currentDrawPosition.y );
	selectionBox.maxs = Vec2(m_endSelect*lineHeight + lineHeight, currentDrawPosition.y + lineHeight );
	renderer.BindTexture(nullptr);
	renderer.DrawAABB2Filled( selectionBox, Rgba8(255,255,255,128) );

	/************************************************************************/
	/* Render Command lines                                                 */
	/************************************************************************/
	std::string currentLine = std::string(">") + m_currentColoredLine.m_devConsolePrintString;
	renderer.DrawTextAtPosition(currentLine.c_str(), currentDrawPosition, lineHeight, m_currentColoredLine.m_textColor);
	currentDrawPosition.y += lineHeight;

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

void DevConsole::EraseSelectedChars()
{
	if( m_beginSelect < m_endSelect )
	{
		int range = m_endSelect - m_beginSelect;
		m_currentColoredLine.m_devConsolePrintString.erase(m_beginSelect, range);
		m_currentCharIndex = m_beginSelect;
	}
	else if( m_endSelect < m_beginSelect )
	{
		int range = m_beginSelect - m_endSelect;
		m_currentColoredLine.m_devConsolePrintString.erase( m_endSelect, range );
		m_currentCharIndex = m_endSelect;
	}

	ResetSelection();
	
	return;
}

void DevConsole::ClampCurrentLine()
{
	int currentLineSize = (int)m_currentColoredLine.m_devConsolePrintString.size();
	
	m_currentCharIndex = ClampInt( m_currentCharIndex, 0, currentLineSize );
	m_beginSelect = ClampInt( m_beginSelect, 0, currentLineSize );
	m_endSelect = ClampInt( m_endSelect, 0, currentLineSize );
}

void DevConsole::ResetSelection()
{
	m_beginSelect = 0;
	m_endSelect = 0;
}

