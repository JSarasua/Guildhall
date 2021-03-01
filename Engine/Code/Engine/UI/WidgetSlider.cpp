#include "Engine/UI/WidgetSlider.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"

WidgetSlider::WidgetSlider( Transform const& transform, Widget* parentWidget /*= nullptr */ ) :
	Widget( transform, parentWidget )
{}

void WidgetSlider::Render()
{
	//Render Self
	if( nullptr != m_mesh && m_isVisible )
	{
		RenderContext* context = m_mesh->m_renderContext;
		if( nullptr != context )
		{
			//Mat44 modelMatrix = GetRelativeModelMatrix();
			Mat44 modelMatrix = GetRelativeModelMatrixScaleOnlySelf();

			context->SetModelMatrix( modelMatrix );
			context->BindShader( (Shader*)nullptr );
			context->SetModelTint( m_backgroundColor );
			context->BindTexture( m_backgroundTexture );
			context->DrawMesh( m_mesh );

			if( m_sliderValue > 0.f )
			{
				Mat44 fillModelMatrix = GetFillRelativeModelMatrixScaleOnlySelf();
				context->SetModelMatrix( fillModelMatrix );
				context->BindTexture( m_fillTexture );
				context->SetModelTint( m_fillColor );
				context->DrawMesh( m_mesh );
			}

			context->SetModelMatrix( modelMatrix );
			if( m_text.size() > 0 )
			{
				AABB2 textBox = AABB2( -0.5f, -0.5f, 0.5f, 0.5f );
				context->DrawAlignedTextAtPosition( m_text.c_str(), textBox, m_textSize, Vec2( 0.5f, 0.5f ) );
			}

			context->SetModelTint( Rgba8::WHITE );
		}
	}

	//Render Children
	for( Widget* childWidget : m_childWidgets )
	{
		if( nullptr != childWidget )
		{
			childWidget->Render();
		}
	}
}

void WidgetSlider::SetSliderValue( float sliderValueZeroToOne )
{
	m_sliderValue = sliderValueZeroToOne;
	m_sliderValue = Clampf( m_sliderValue, 0.f, 1.f );
}

void WidgetSlider::SetBackgroundAndFillTextures( Texture const* backgroundTexture, Texture const* fillTexture )
{
	m_backgroundTexture = backgroundTexture;
	m_fillTexture = fillTexture;
}

Mat44 WidgetSlider::GetFillModelMatrix()
{
	Transform fillTransform = m_widgetTransform;
	fillTransform.m_scale.x *= m_sliderValue;

	Vec2 localBottomLeft = Vec2( -0.5f, -0.5f );
	Mat44 backgroundMatrix = m_widgetTransform.ToMatrix();
	Mat44 fillMatrix = fillTransform.ToMatrix();

	Vec2 widgetBottomLeft = backgroundMatrix.TransformPosition2D( localBottomLeft );
	Vec2 fillWidgetBottomLeft = fillMatrix.TransformPosition2D( localBottomLeft );

	Vec2 translator = widgetBottomLeft - fillWidgetBottomLeft;
	fillTransform.m_position += translator;

	return fillTransform.ToMatrix();
}

Mat44 WidgetSlider::GetFillRelativeModelMatrixScaleOnlySelf()
{
	Mat44 parentRelativeMatrixNoScale;
	if( m_parentWidget )
	{
		parentRelativeMatrixNoScale = m_parentWidget->GetParentRelativeModelMatrixNoScale();
	}

	Mat44 myLocalMatrix = GetFillModelMatrix();

	parentRelativeMatrixNoScale.TransformBy( myLocalMatrix );
	return parentRelativeMatrixNoScale;
}

