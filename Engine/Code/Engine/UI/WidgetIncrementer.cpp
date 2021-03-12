#include "Engine/UI/WidgetIncrementer.hpp"

WidgetIncrementer::WidgetIncrementer( int startValue, Transform const& widgetTransform ) :
	WidgetGrid( widgetTransform, IntVec2( 3, 1 ) )
{
	m_intValue = startValue;
	m_useInt = true;
	Transform gridElementTranform;
	gridElementTranform.m_scale = widgetTransform.m_scale;
	gridElementTranform.m_scale.x /= 3;

	m_downIncrementWidget = new Widget( gridElementTranform );
	m_valueWidget = new Widget( gridElementTranform );
	m_upIncrementWidget = new Widget( gridElementTranform );
	WidgetGrid::AddChild( m_downIncrementWidget );
	WidgetGrid::AddChild( m_valueWidget );
	WidgetGrid::AddChild( m_upIncrementWidget );
	m_downIncrementWidget->SetText( "<<" );
	m_upIncrementWidget->SetText( ">>" );

	if( m_useInt )
	{
		m_valueWidget->SetText( Stringf( "%i", m_intValue ) );
	}
	else
	{
		m_valueWidget->SetText( Stringf( "%f", m_floatValue ) );
	}

	m_downIncrementWidget->m_selectDelegate.SubscribeMethod( this, &WidgetIncrementer::DecrementValue );
	m_upIncrementWidget->m_selectDelegate.SubscribeMethod( this, &WidgetIncrementer::IncrementValue );
}

bool WidgetIncrementer::DecrementValue( EventArgs const& args )
{
	if( m_useInt )
	{
		float dt = args.GetValue( "deltaSeconds", 0.016f );
		m_intValue -= 1;
	}
	else
	{
		float dt = args.GetValue( "deltaSeconds", 0.016f );
		m_floatValue -= dt;
	}

	return true;
}

bool WidgetIncrementer::IncrementValue( EventArgs const& args )
{
	if( m_useInt )
	{
		float dt = args.GetValue( "deltaSeconds", 0.016f );
		m_intValue += 1;
	}
	else
	{
		float dt = args.GetValue( "deltaSeconds", 0.016f );
		m_floatValue += dt;
	}

	return true;
}

