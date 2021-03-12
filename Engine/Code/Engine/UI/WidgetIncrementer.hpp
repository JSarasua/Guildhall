#pragma once
#include "Engine/UI/Widget.hpp"
#include "Engine/UI/WidgetGrid.hpp"

class WidgetIncrementer : public WidgetGrid
{
public:
	WidgetIncrementer( int startValue, Transform const& widgetTransform );
	WidgetIncrementer( float startValue, Transform const& widgetTransform );

	virtual void AddChild(  Widget* childWidget ) override;

	bool DecrementValue( EventArgs const& args );
	bool IncrementValue( EventArgs const& args );

public:
	bool m_useInt = true;
	Widget* m_downIncrementWidget = nullptr;
	Widget* m_valueWidget = nullptr;
	Widget* m_upIncrementWidget = nullptr;

	int m_intValue = 0;
	float m_floatValue = 0;
};
