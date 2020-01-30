#pragma once
#include <string>

class RenderContext;
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D10Blob;

enum SHADERTYPE
{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_FRAGMENT
};


class ShaderStage
{
public:
	~ShaderStage();

	bool Compile( RenderContext* context,
		std::string const& filename,
		void const* source,
		size_t const sourceByteLen,
		SHADERTYPE stage );

	bool IsValid() const { return (nullptr != m_handle); };

public:
	SHADERTYPE m_type;
	ID3D10Blob* m_byteCode = nullptr;
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
	Shader( RenderContext* context );
	bool CreateFromFile( std::string const& filename );

public:
	RenderContext* m_owner;
	ShaderStage m_vertexStage;
	ShaderStage m_fragmentStage; //PixelStage
};