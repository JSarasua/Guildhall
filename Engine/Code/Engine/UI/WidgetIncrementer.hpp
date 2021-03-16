#pragma once
#include "Engine/UI/Widget.hpp"
#include <vector>
#include <string>

struct AABB2;

class WidgetIncrementer : public Widget
{
public:
	WidgetIncrementer( std::vector<std::string> const& choices, AABB2 const& localAABB2, float incrementerXPercentSplit, float textSize = 0.5f, Widget* parentWidget = nullptr );

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
