#include "Engine/Renderer/ShaderState.hpp"

ShaderState::ShaderState( XmlElement const& element )
{
	SetupFromXML( element );

}

ShaderState::~ShaderState()
{

}

void ShaderState::SetupFromXML( XmlElement const& element )
{

	//Get Data out of XML
	std::string shaderFilePath = ParseXMLAttribute( element, "ShaderFilePath", "" );
	std::string blendModeStr = ParseXMLAttribute( element, "BlendMode", "SOLID" );
	std::string depthCompareOpStr = ParseXMLAttribute( element, "DepthCompareMode", "COMPARE_LESS_THAN_OR_EQUAL" );
	std::string windingOrderStr = ParseXMLAttribute( element, "WindingOrder", "WIND_COUNTERCLOCKWISE" );
	std::string cullModeStr = ParseXMLAttribute( element, "CullMode", "CULL_BACK" );
	std::string fillModeStr = ParseXMLAttribute( element, "FillMode", "FILL_SOLID" );
	bool writeDepth = ParseXMLAttribute( element, "WriteDepth", true );


	//Shader
	m_shaderFilePath = shaderFilePath;

	//Blend Mode
	if( blendModeStr == "ALPHA" )
	{
		m_blendMode = eBlendMode::ALPHA;
	}
	else if( blendModeStr == "ADDITIVE" )
	{
		m_blendMode = eBlendMode::ADDITIVE;
	}
	else if( blendModeStr == "SOLID" )
	{
		m_blendMode = eBlendMode::SOLID;
	}
	else
	{
		ERROR_AND_DIE(" BlendMode was not ALPHA, ADDITIVE, or SOLID" );
	}

	//DepthCompare
	if( depthCompareOpStr == "COMPARE_LESS_THAN_OR_EQUAL" )
	{
		m_compareMode = eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL;
	}
	else if( depthCompareOpStr == "COMPARE_GREATER_THAN" )
	{
		m_compareMode = eDepthCompareMode::COMPARE_GREATER_THAN;
	}
	else if( depthCompareOpStr == "COMPARE_ALWAYS" )
	{
		m_compareMode = eDepthCompareMode::COMPARE_ALWAYS;
	}
	else if( depthCompareOpStr == "COMPARE_EQUAL" )
	{
		m_compareMode = eDepthCompareMode::COMPARE_EQUAL;
	}
	else {
		ERROR_AND_DIE( "Invalid Depth Compare Op: Must be COMPARE_LESS_THAN_OR_EQUAL, COMPARE_GREATER_THAN, COMPARE_ALWAYS, or COMPARE_EQUAL" );
	}

	//Write Depth
	if( writeDepth )
	{
		m_writeDepth = eDepthWriteMode::WRITE_ALL;
	}
	else
	{
		m_writeDepth = eDepthWriteMode::WRITE_NONE;
	}

	//Winding order
	if( windingOrderStr == "CLOCKWISE" )
	{
		m_windingOrder = eFrontFaceWindOrder::CLOCKWISE;
	}
	else
	{
		m_windingOrder = eFrontFaceWindOrder::COUNTERCLOCKWISE;
	}

	//Cull Mode
	if( cullModeStr == "CULL_FRONT" )
	{
		m_cullMode = eCullMode::CULL_FRONT;
	}
	else if( cullModeStr == "CULL_NONE" )
	{
		m_cullMode = eCullMode::CULL_NONE;
	}
	else
	{
		m_cullMode = eCullMode::CULL_BACK;
	}

	//Fill Mode
	if( fillModeStr == "FILL_SOLID" )
	{
		m_fillMode = eFillMode::FILL_SOLID;
	}
	else if( fillModeStr == "FILL_WIREFRAME" )
	{
		m_fillMode = eFillMode::FILL_WIREFRAME;
	}


}

