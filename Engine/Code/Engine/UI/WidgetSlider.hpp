#pragma once
#include "Engine/UI/Widget.hpp"
#include "Engine/Core/Rgba8.hpp"

class WidgetSlider : public Widget
{
public:
	WidgetSlider( Transform const& transform, Widget* parentWidget = nullptr );
	virtual void Render() override;

	void SetSliderValue( float sliderValueZeroToOne );
	void SetBackgroundAndFillTextures( Texture const* backgroundTexture, Texture const* fillTexture );
	Mat44 GetFillModelMatrix();
	Mat44 GetFillRelativeModelMatrixScaleOnlySelf();

private:
	float m_sliderValue = 0.f;
	Rgba8 m_backgroundColor = Rgba8::RED;
	Rgba8 m_fillColor = Rgba8::GREEN;

	Texture const* m_backgroundTexture = nullptr;
	Texture const* m_fillTexture = nullptr;

};