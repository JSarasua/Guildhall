#pragma once
#include <string>

class RenderContext;
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D10Blob;
struct ID3D11RasterizerState;
struct ID3D11InputLayout;
struct BufferAttribute;

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

	void const* GetByteCode();
	size_t GetByteCodeLength() const;

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
	~Shader();

	bool CreateFromFile( std::string const& filename );
	void CreateRasterState();

	ID3D11InputLayout* GetOrCreateInputLayout( const BufferAttribute*  layout );

public:
	RenderContext* m_owner;
	ShaderStage m_vertexStage;
	ShaderStage m_fragmentStage; //PixelStage
	std::string m_filename;

	ID3D11RasterizerState* m_rasterState = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
	BufferAttribute const* m_previousLayout = nullptr;
};