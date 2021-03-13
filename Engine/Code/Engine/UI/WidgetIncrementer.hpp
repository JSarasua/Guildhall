#pragma once
#include "Engine/UI/Widget.hpp"
#include "Engine/UI/WidgetGrid.hpp"


class WidgetIncrementer : public WidgetGrid
{
public:
	WidgetIncrementer( std::vector<std::string> const& choices, Transform const& widgetTransform );

	//virtual void AddChild(  Widget* childWidget ) override;
	void SetTextures( Texture const* backgroundTexture );
	bool DecrementValue( EventArgs const& args );
	bool IncrementValue( EventArgs const& args );

public:
	Widget* m_downIncrementWidget = nullptr;
	Widget* m_valueWidget = nullptr;
	Widget* m_upIncrementWidget = nullptr;

	std::vector<std::string> m_stringValues;
	size_t m_stringIndex = 0;
};
