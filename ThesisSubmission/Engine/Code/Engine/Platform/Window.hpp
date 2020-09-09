#pragma once
#include <string>

class EventSystem;
class InputSystem;

class Window
{
public:
	Window();
	~Window();

	bool Open( std::string const& title, float aspectRatio, float ratioOfHeight /*= 0.9f*/ );
	void Close();
	void BeginFrame();
	void EndFrame();

	void ToggleBorder();

	void SetInputSystem( InputSystem* input );
	void SetEventSystem( EventSystem* eventSystem );
	InputSystem* GetInputSystem() const { return m_input; }


	void* m_hwnd = nullptr; //Used to not include a windows file

	InputSystem* m_input = nullptr;
	EventSystem* m_eventSystem = nullptr;
	unsigned int GetClientWidth();
	unsigned int GetClientHeight();

	unsigned int m_clientWidth = 0;
	unsigned int m_clientHeight = 0;

	bool m_isbordered = true;
};