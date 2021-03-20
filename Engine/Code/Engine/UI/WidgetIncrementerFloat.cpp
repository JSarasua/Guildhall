#include "Engine/UI/WidgetIncrementerFloat.hpp"
#include "Engine/Math/MathUtils.hpp"

WidgetIncrementerFloat::WidgetIncrementerFloat( std::vector<float> const& increments, std::string const& headerString, float initialValue, float minValue, float maxValue, 
	AABB2 const& localAABB2, float textSize /*= 0.5f*/, eIncrementChangeType incrementType /*= OnRelease*/, Widget* parentWidget /*= nullptr */ ) :
	Widget( localAABB2, parentWidget ),
	m_currentFloatValue( initialValue ),
	m_minValue( minValue ),
	m_maxValue( maxValue ),
	m_headerString( headerString ),
	m_incrementChangeType( incrementType )
{
	//Covered up by child Widgets
	SetIsVisible( false );

	//Initialize header widget
	AABB2 topAABB = GetLocalAABB2().GetBoxAtTop( 0.5f );
	m_valueWidget = new Widget( topAABB, this );
	m_valueWidget->SetText( Stringf( "%s %.2f", m_headerString.c_str(), m_currentFloatValue ) );
	m_valueWidget->SetTextSize( textSize );
	m_valueWidget->SetCanHover( false );

	//Initialize incrementer widgets
	AABB2 botAABB = GetLocalAABB2().GetBoxAtBottom( 0.5f );
	int numberOfIncrements = (int)increments.size();
	std::vector<AABB2> incrementBoxes = botAABB.GetBoxAsColumns( numberOfIncrements );

	for( int incrementIndex = 0; incrementIndex < numberOfIncrements; incrementIndex++ )
	{
		AABB2 const& incrementBounds = incrementBoxes[incrementIndex];
		float currentIncrement = increments[incrementIndex];
		
		Widget* incrementWidget = new Widget( incrementBounds, this );
		if( currentIncrement > 0.f )
		{
			incrementWidget->SetText( Stringf("+%.1f", currentIncrement ) );
		}
		else
		{
			incrementWidget->SetText( Stringf("%.1f", currentIncrement ) );
		}

		incrementWidget->SetTextSize( textSize );
		incrementWidget->SetCanSelect( true );

		//Add firing events
		if( m_incrementChangeType == OnPress )
		{
			incrementWidget->m_selectArgs.SetValue( "increment", currentIncrement );
			incrementWidget->m_selectDelegate.SubscribeMethod( this, &WidgetIncrementerFloat::ChangeValues );
		}
		else
		{
			incrementWidget->m_releaseArgs.SetValue( "increment", currentIncrement );
			incrementWidget->m_releaseDelegate.SubscribeMethod( this, &WidgetIncrementerFloat::ChangeValues );
		}
	}
}


void WidgetIncrementerFloat::SetIncrementerTextures( Texture const* backgroundTex, Texture const* hoverTex, Texture const* selectTex )
{
	SetTexture( backgroundTex, backgroundTex, backgroundTex );

	m_valueWidget->SetTexture( backgroundTex, backgroundTex, backgroundTex );

	for( Widget* childWidget : m_childWidgets )
	{
		childWidget->SetTexture( backgroundTex, hoverTex, selectTex );
	}
}

bool WidgetIncrementerFloat::ChangeValues( EventArgs const& args )
{
	float deltaSeconds = args.GetValue( "deltaSeconds", 1.f );
	m_currentFloatValue += args.GetValue( "increment", 0.f ) * deltaSeconds;
	m_currentFloatValue = Clampf( m_currentFloatValue, m_minValue, m_maxValue );
	m_valueWidget->SetText( Stringf( "%s %.2f", m_headerString.c_str(), m_currentFloatValue ) );

	EventArgs valueArgs;
	valueArgs.SetValue( "value", m_currentFloatValue );
	m_valueChangeDelegate.Invoke( valueArgs );
	return true;
}

