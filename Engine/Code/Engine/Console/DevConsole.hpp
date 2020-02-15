#pragma once
#include <vector>
#include <string>
#include "Engine/Core/Rgba8.hpp"

class RenderContext;
class Camera;
class InputSystem;

struct ColoredLine
{
public:
	ColoredLine( const Rgba8& textColor, const std::string& devConsolePrintString );

public:
	Rgba8 m_textColor;
	std::string m_devConsolePrintString;
};


class DevConsole
{
public:
	DevConsole();
	~DevConsole() {}

	void Startup();
	void BeginFrame();
	void Update( float deltaSeconds );
	void EndFrame();
	void Shutdown();

	void HandleKeyStroke( unsigned char keyStroke );

	void PringString( const Rgba8& textColor, const std::string& devConsolePrintString );
	void Render( RenderContext& renderer, const Camera& camera, float lineHeight ) const;

	void SetIsOpen( bool isOpen );
	bool IsOpen() const {return m_isOpen;}

private:
	std::vector<ColoredLine> m_coloredLines;
	ColoredLine m_currentColoredLine;
	ColoredLine m_caret;
	bool m_isCaretRendering = true;
	float m_caretTimer = 0.f;
	bool m_isOpen = false;
	int m_currentCharIndex = 0;
	int m_currentPreviousLineIndex = 0;
};