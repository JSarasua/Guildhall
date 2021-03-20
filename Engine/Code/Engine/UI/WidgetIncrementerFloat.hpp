#pragma once
#include "Engine/UI/Widget.hpp"
#include <vector>
#include <string>

struct AABB2;

enum eIncrementChangeType
{
	OnPress,
	OnRelease
};

// HeaderString CurrentValue
// -10 -5 +5 +10
class WidgetIncrementerFloat : public Widget
{
public:
	WidgetIncrementerFloat( std::vector<float> const& increments, std::string const& headerString, float initialValue, float minValue, float maxValue, 
		AABB2 const& localAABB2, float textSize = 0.5f, eIncrementChangeType incrementType = OnRelease, Widget* parentWidget = nullptr );

	//virtual void AddChild(  Widget* childWidget ) override;
	void SetIncrementerTextures( Texture const* backgroundTex, Texture const* hoverTex, Texture const* selectTex );
	bool ChangeValues( EventArgs const& args );
	bool DecrementValue( EventArgs const& args );
	bool IncrementValue( EventArgs const& args );

public:
	eIncrementChangeType m_incrementChangeType = OnRelease;
	Widget* m_valueWidget = nullptr;

	float m_currentFloatValue = 0.f;
	float m_minValue = 0.f;
	float m_maxValue = 1.f;

	std::string m_headerString;
	

	Delegate<EventArgs const&> m_valueChangeDelegate;
};