#include "Engine/UI/WidgetIncrementer.hpp"

WidgetIncrementer::WidgetIncrementer( std::vector<std::string> const& choices, Transform const& widgetTransform ):
	WidgetGrid( widgetTransform, IntVec2( 3, 1 ) ),
	m_stringValues( choices )
{
	Transform gridElementTranform;
	gridElementTranform.m_scale = widgetTransform.m_scale;
	gridElementTranform.m_scale.x /= 3;

	m_downIncrementWidget = new Widget( gridElementTranform );
	m_valueWidget = new Widget( gridElementTranform );
	m_upIncrementWidget = new Widget( gridElementTranform );
	WidgetGrid::AddChild( m_downIncrementWidget );
	WidgetGrid::AddChild( m_upIncrementWidget );
	WidgetGrid::AddChild( m_valueWidget );
	m_downIncrementWidget->SetText( "<<" );
	m_upIncrementWidget->SetText( ">>" );
	m_downIncrementWidget->SetTextSize( 0.1f );
	m_upIncrementWidget->SetTextSize( 0.1f );
	m_valueWidget->SetTextSize( 0.1f );

	if( m_stringValues.size() > 0 )
	{
		m_valueWidget->SetText( m_stringValues[0] );
	}



	m_downIncrementWidget->m_selectDelegate.SubscribeMethod( this, &WidgetIncrementer::DecrementValue );
	m_upIncrementWidget->m_selectDelegate.SubscribeMethod( this, &WidgetIncrementer::IncrementValue );
}

void WidgetIncrementer::SetTextures( Texture const* backgroundTexture )
{
	m_downIncrementWidget->SetTexture( backgroundTexture, backgroundTexture, backgroundTexture );
	m_valueWidget->SetTexture( backgroundTexture, backgroundTexture, backgroundTexture );
	m_upIncrementWidget->SetTexture( backgroundTexture, backgroundTexture, backgroundTexture );
}

bool WidgetIncrementer::DecrementValue( EventArgs const& args )
{
	if( m_stringIndex == 0 )
	{
		m_stringIndex = m_stringValues.size() - 1;
	}
	else
	{
		m_stringIndex--;
	}

	m_valueWidget->SetText( m_stringValues[m_stringIndex] );

	return true;
}

bool WidgetIncrementer::IncrementValue( EventArgs const& args )
{
	if( m_stringIndex == m_stringValues.size() - 1 )
	{
		m_stringIndex = 0;
	}
	else
	{
		m_stringIndex++;
	}

	m_valueWidget->SetText( m_stringValues[m_stringIndex] );

	return true;
}

