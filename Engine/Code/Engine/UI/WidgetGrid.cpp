#include "Engine/UI/WidgetGrid.hpp"

WidgetGrid::WidgetGrid( Transform const& transform, IntVec2 const& gridDimensions, Widget* parentWidget /*= nullptr */ ) :
	Widget( transform, parentWidget ),
	m_gridDimensions( gridDimensions )
{}

void WidgetGrid::AddChild( Widget* childWidget )
{
	if( !childWidget )
	{
		m_childWidgets.push_back( childWidget );
		return;
	}

	int currentNumberOfChildren = (int)m_childWidgets.size();

	int numberOfSlots = m_gridDimensions.x * m_gridDimensions.y;

	if( currentNumberOfChildren + 1 > numberOfSlots )
	{
		return;
	}

	Vec2 slotPosition = GetNextSlotPosition();
	childWidget->SetPosition( slotPosition );
	m_childWidgets.push_back( childWidget );
	childWidget->SetParent( this );
}

Vec2 WidgetGrid::GetSlotPosition( IntVec2 const& slotIndex ) const
{
	AABB2 gridAABB2 = GetLocalAABB2();
	AABB2 gridSlot = gridAABB2.GetGridBoxFromGridDimensions( m_gridDimensions, slotIndex );
	Vec2 slotCenter = gridSlot.GetCenter();

	return slotCenter;
}

Vec2 WidgetGrid::GetNextSlotPosition() const
{
	IntVec2 nextSlotIndex = GetNextSlotIndex();
	Vec2 nextSlotPosition = GetSlotPosition( nextSlotIndex );

	return nextSlotPosition;
}

IntVec2 WidgetGrid::GetNextSlotIndex() const
{
	IntVec2 nextSlotIndex;

	int nextSlotVectorIndex = (int)m_childWidgets.size();

	int nextSlotIndexX = 0;
	int nextSlotIndexY = 0;

	while( nextSlotVectorIndex >= m_gridDimensions.x )
	{
		nextSlotVectorIndex -= m_gridDimensions.x;
		nextSlotIndexY++;
	}
	nextSlotIndexX = nextSlotVectorIndex;

	nextSlotIndex.x = nextSlotIndexX;
	nextSlotIndex.y = nextSlotIndexY;

	return nextSlotIndex;
}

Widget* WidgetGrid::GetChildWidgetAtIndex( int gridIndex ) const
{
	int childWidgetSize = (int)m_childWidgets.size();

	if( childWidgetSize > gridIndex )
	{
		return m_childWidgets[gridIndex];
	}
	else
	{
		ERROR_AND_DIE( "Tried to get a child widget past array bounds" );
	}
}

