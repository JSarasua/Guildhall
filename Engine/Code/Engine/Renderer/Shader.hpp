#pragma once
#include <string>

class RenderContext;
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

enum SHADERTYPE
{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_FRAGMENT
};


class ShaderStage
{
public:
	bool Compile( RenderContext* context,
		std::string const& filename,
		void const* source,
		size_t const sourceByteLen,
		SHADERTYPE stage );

public:
	SHADERTYPE m_type;
	union
	{
		ID3D11Resource *m_handle;
		ID3D11VertexShader *m_vs;
		ID3D11PixelShader *m_fs;
	};
};


class Shader
{
public:
	bool CreateFromFile( std::string const& filename );

public:
	ShaderStage m_vertexStage;
	ShaderStage m_fragmentStage; //PixelStage
};