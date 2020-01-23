#pragma once
#include <string>

class Window
{
public:
	Window();
	~Window();

	bool Open( std::string const& title, float aspectRatio, float ratioOfHeight /*= 0.9f*/ );
	void Close();
	void BeginFrame();

	void* m_hwnd; //Used to not include a windows file


};