#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/XmlUtils.hpp"

class ShaderState
{
public:
	ShaderState( RenderContext* context );
	ShaderState( RenderContext* context, XmlElement const& element );
	~ShaderState();

	void SetupFromXML( XmlElement const& element );
	void SetupFromFile( char const* filename );

public:
	RenderContext* m_context = nullptr;

	Shader* m_shader = nullptr;
	std::string m_shaderFilePath = "";
	eBlendMode m_blendMode = eBlendMode::SOLID;
	eDepthCompareMode m_compareMode = eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL;
	eDepthWriteMode m_writeDepth = eDepthWriteMode::WRITE_ALL;
	eFrontFaceWindOrder m_windingOrder = eFrontFaceWindOrder::COUNTERCLOCKWISE;
	eCullMode m_cullMode = eCullMode::CULL_BACK;
	eFillMode m_fillMode = eFillMode::FILL_SOLID;
};