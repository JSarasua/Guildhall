#include "Engine/UI/WidgetIncrementer.hpp"
#include "Engine/Math/AABB2.hpp"

WidgetIncrementer::WidgetIncrementer( std::vector<std::string> const& choices, AABB2 const& localAABB2, float incrementerXPercentSplit, float textSize, Widget* parentWidget  ) :
	Widget( localAABB2, parentWidget ),
	m_stringValues( choices )
{
	//Place child widgets
	AABB2 myAABB2 = GetLocalAABB2();
	AABB2 incrementerBounds =  myAABB2.CarveBoxOffLeft( incrementerXPercentSplit );
	AABB2 downIncrementerBounds = incrementerBounds.CarveBoxOffLeft( 0.5f );
	AABB2 upIncrementerBounds = incrementerBounds;
	AABB2 stringTextBounds = myAABB2;

	Transform downIncrementerTransform;
	downIncrementerTransform.m_position = downIncrementerBounds.GetCenter();
	downIncrementerTransform.m_scale = Vec3( downIncrementerBounds.GetDimensions(), 1.f );
	m_downIncrementWidget = new Widget( downIncrementerTransform );

	Transform upIncrementerTransform;
	upIncrementerTransform.m_position = upIncrementerBounds.GetCenter();
	upIncrementerTransform.m_scale = Vec3( upIncrementerBounds.GetDimensions(), 1.f );
	m_upIncrementWidget = new Widget( upIncrementerTransform );

	Transform stringTextTransform;
	stringTextTransform.m_position = stringTextBounds.GetCenter();
	stringTextTransform.m_scale = Vec3( stringTextBounds.GetDimensions(), 1.f );
	m_valueWidget = new Widget( stringTextTransform );

	m_downIncrementWidget->SetTextSize( textSize );
	m_downIncrementWidget->SetText( "<<" );
	m_upIncrementWidget->SetTextSize( textSize );
	m_upIncrementWidget->SetText( ">>" );
	m_valueWidget->SetTextSize( textSize );
	m_valueWidget->SetTextAlignment( Vec2( 0.1f, 0.5f ) );
	AddChild( m_downIncrementWidget );
	AddChild( m_upIncrementWidget );
	AddChild( m_valueWidget );

	//Set up initial values
	if( m_stringValues.size() > 0 )
	{
		m_valueWidget->SetText( m_stringValues[0] );
	}

	//Activate buttons
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

