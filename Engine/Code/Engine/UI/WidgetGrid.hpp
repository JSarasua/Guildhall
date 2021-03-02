#pragma once
#include "Engine/UI/Widget.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/vec2.hpp"

class WidgetGrid : public Widget
{
public:
	WidgetGrid( Transform const& transform, IntVec2 const& gridDimensions, Widget* parentWidget = nullptr );
	
	virtual void AddChild( Widget* childWidget ) override;

	Vec2 GetSlotPosition( IntVec2 const& slotIndex ) const;
	Vec2 GetNextSlotPosition() const;
	IntVec2 GetNextSlotIndex() const;

private:
	IntVec2 m_gridDimensions;
};