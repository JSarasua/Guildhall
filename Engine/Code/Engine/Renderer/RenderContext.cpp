#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/Camera.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb_image.h"


#if !defined(WIN32_LEAN_AND_MEAN) 
#define WIN32_LEAN_AND_MEAN
#endif

#include "Engine/Renderer/D3D11Common.hpp"
#pragma comment( lib, "d3d11.lib" )         // needed a01
#pragma comment( lib, "dxgi.lib" )          // needed a01
#pragma comment( lib, "d3dcompiler.lib" )   // needed when we get to shaders




void RenderContext::StartUp(Window* window)
{
	//ID3D11Device


	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

	#if defined(RENDER_DEBUG)
		flags |= D3D11_CREATE_DEVICE_DEBUG;
		CreateDebugModule(); 
	#endif

	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	memset( &swapchainDesc, 0, sizeof(swapchainDesc) );
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//on swap, the old buffer is discarded
	swapchainDesc.Flags = 0;
	
	HWND hwnd = (HWND) window->m_hwnd;
	swapchainDesc.OutputWindow = hwnd;	//HWND for the window to be used
	swapchainDesc.SampleDesc.Count = 1;	// how many samples per pixel (1 so no MSAA)

	swapchainDesc.Windowed = TRUE;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferDesc.Width = window->GetClientWidth();		//Save off as a member variable when creating the window
	swapchainDesc.BufferDesc.Height = window->GetClientHeight();

	// create
	IDXGISwapChain* swapchain;

	HRESULT result = D3D11CreateDeviceAndSwapChain( nullptr, 
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags, // constrols the type of device we make
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapchainDesc,
		&swapchain,
		&m_device,
		nullptr,
		&m_context
		);

	GUARANTEE_OR_DIE( SUCCEEDED(result), "failed to create rendering device" );

	m_swapchain = new SwapChain( this, swapchain );
	//m_defaultShader = new Shader( this );
	m_defaultShader = GetOrCreateShader( "Data/Shaders/Default.hlsl" );
	CreateDefaultRasterState();

	m_immediateVBO = new VertexBuffer( this, MEMORY_HINT_DYNAMIC );
	m_immediateIBO = new IndexBuffer( this, MEMORY_HINT_DYNAMIC );

	m_frameUBO = new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );

	m_sampPoint = new Sampler( this, SAMPLER_POINT );
	m_texWhite = CreateTextureFromColor( Rgba8::WHITE );

	CreateBlendModes();
	
}

void RenderContext::BeginFrame()
{

}

void RenderContext::EndFrame()
{
	m_swapchain->Present();
}



void RenderContext::Shutdown()
{
	for( size_t shaderIndex = 0; shaderIndex < m_shaders.size(); shaderIndex++ )
	{
		if( nullptr == m_shaders[shaderIndex] )
		{
			continue;
		}

		delete m_shaders[shaderIndex];
		m_shaders[shaderIndex] = nullptr;
	}
	m_defaultShader = nullptr;
	DX_SAFE_RELEASE( m_rasterState );

	delete m_swapchain;
	m_swapchain = nullptr;


	delete m_immediateVBO;
	m_immediateVBO = nullptr;
	m_swapchain = nullptr;

	delete m_immediateIBO;
	m_immediateIBO = nullptr;

	delete m_frameUBO;
	m_frameUBO = nullptr;

	delete m_modelUBO;
	m_frameUBO = nullptr;

	delete m_sampPoint;
	m_sampPoint = nullptr;

	DX_SAFE_RELEASE( m_alphaBlendStateHandle );
	DX_SAFE_RELEASE( m_additiveBlendStateHandle );
	DX_SAFE_RELEASE( m_solidBlendStateHandle );

	DX_SAFE_RELEASE( m_depthStencilState );

	for( int textureIndex = 0; textureIndex < m_Textures.size(); textureIndex++ )
	{
		delete m_Textures[textureIndex];
		m_Textures[textureIndex] = nullptr;
	}
	m_Textures.clear();

	m_texWhite = nullptr;

	DX_SAFE_RELEASE( m_device );
	DX_SAFE_RELEASE( m_context );

	ReportLiveObjects();    // do our leak reporting just before shutdown to give us a better detailed list; 
	DestroyDebugModule();
}




void RenderContext::Setup( Clock* gameClock )
{
	m_gameClock = gameClock;
	if( nullptr == m_gameClock )
	{
		m_gameClock = Clock::GetMaster();
	}
}

void RenderContext::UpdateFrameTime()
{
	float currentTime = (float)m_gameClock->GetTotalElapsedSeconds();
	float dt = (float)m_gameClock->GetLastDeltaSeconds();

	FrameData framedata;
	framedata.systemTime = currentTime;
	framedata.systemDeltaTime = dt;

	m_frameUBO->Update( &framedata, sizeof(framedata), sizeof(framedata) );
}

void RenderContext::ClearScreen( const Rgba8& clearColor )
{
	UNUSED(clearColor);
}

void RenderContext::ClearDepth( Texture* depthStencilTarget, float depth /*= 1.f*/, float stencil /*= 0.f */ )
{
	if( depthStencilTarget != nullptr )
	{
		TextureView* depthStencilView = depthStencilTarget->GetOrCreateDepthStencilView();
		ID3D11DepthStencilView* dsv = depthStencilView->GetAsDSV();

		m_context->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH, depth, (UINT8)stencil );
	}
}

void RenderContext::DrawVertexArray(int numVertexes, const Vertex_PCU* vertexes )
{
	//RenderBuffer is the gpu equivalent of a block of cpu memory
	// RenderBuffer* m_immediateVBO; // VBO - Vertex Buffer Object
	//Update a vertex buffer
	size_t bufferTotalByteSize = numVertexes * sizeof( Vertex_PCU );
	size_t elementSize = sizeof( Vertex_PCU );
	m_immediateVBO->Update( vertexes, bufferTotalByteSize, elementSize );

	// Bind
	BindVertexBuffer( m_immediateVBO );

	// Draw
	Draw( numVertexes, 0 );
}

void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU>& vertexes )
{
	if( !vertexes.empty() )
	{
	DrawVertexArray( static_cast<int>(vertexes.size()), &vertexes[0] );
	}
}

void RenderContext::DrawIndexedVertexArray( std::vector<Vertex_PCU> const& vertexes, std::vector<uint> const& indices )
{
	if( vertexes.size() > 0 )
	{
		size_t bufferTotalByteSize = vertexes.size() * sizeof( Vertex_PCU );
		size_t elementSize = sizeof( Vertex_PCU );
		m_immediateVBO->Update( &vertexes[0], bufferTotalByteSize, elementSize );
		m_immediateIBO->Update(indices);

		BindVertexBuffer( m_immediateVBO );
		BindIndexBuffer( m_immediateIBO );

		DrawIndexed( (int)indices.size() );
	}
}

void RenderContext::DrawMesh( GPUMesh* mesh )
{
	//m_immediateVBO = mesh->m_vertices;

	BindVertexBuffer( mesh->GetVertexBuffer() );

	if( nullptr != mesh->GetIndexBuffer() )
	{
		BindIndexBuffer( mesh->GetIndexBuffer() );
		DrawIndexed( mesh->GetIndexCount() );
	}
	else
	{
		Draw( mesh->m_vertexCount, 0 );
	}

}

void RenderContext::AppendVerts( std::vector<Vertex_PCU>& masterVertexList, std::vector<Vertex_PCU>& vertsToAppend )
{
	for( int vertIndex = 0; vertIndex < vertsToAppend.size(); vertIndex++ )
	{
		masterVertexList.push_back(vertsToAppend[vertIndex]);
	}
}

void RenderContext::AppendVertsFromAABB2( std::vector<Vertex_PCU>& masterVertexList, const AABB2& aabb, const Rgba8& tint, const Vec2& uvMins, const Vec2& uvMaxs )
{

	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.mins ), tint, uvMins ));
	masterVertexList.push_back( Vertex_PCU( Vec3( Vec2( aabb.maxs.x, aabb.mins.y ) ), tint, Vec2( uvMaxs.x, uvMins.y ) ));
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.maxs ),tint, uvMaxs ));

	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.mins ),tint, uvMins ));
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.maxs ),tint, uvMaxs ));
	masterVertexList.push_back( Vertex_PCU( Vec3( Vec2( aabb.mins.x, aabb.maxs.y ) ),tint, Vec2( uvMins.x,uvMaxs.y ) ));
	
}

void RenderContext::BindShader( Shader* shader )
{
	GUARANTEE_OR_DIE( IsDrawing(), "You haven't started begin camera");

	m_currentShader = shader;
	if( nullptr == m_currentShader )
	{
		m_currentShader = m_defaultShader;
	}

	m_context->VSSetShader( m_currentShader->m_vertexStage.m_vs, nullptr, 0 );
	m_context->PSSetShader( m_currentShader->m_fragmentStage.m_fs, nullptr, 0 );
}

void RenderContext::BindShader( char const* filename )
{
	for( size_t shaderIndex = 0; shaderIndex < m_shaders.size(); shaderIndex++ )
	{
		if( nullptr == m_shaders[shaderIndex] )
		{
			continue;
		}

		if( m_shaders[shaderIndex]->m_filename.compare( filename ) == 0 )
		{
			m_currentShader = m_shaders[shaderIndex];
		}
	}
}

void RenderContext::BindVertexBuffer( VertexBuffer* vbo )
{
	ID3D11Buffer* vboHandle = vbo->m_handle;
	if( m_lastVBOHandle == vboHandle )
	{
		return;
	}
	m_lastVBOHandle = vboHandle;

	uint stride = sizeof(Vertex_PCU); // how far from one vertex to next
	uint offset = 0;	// how far into buffer we start


	m_context->IASetVertexBuffers( 0, 1, &vboHandle, &stride, &offset );
	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

void RenderContext::BindIndexBuffer( IndexBuffer* ibo )
{
	ID3D11Buffer* iboHandle = ibo->m_handle;
// 	if( m_lastVBOHandle == iboHandle )
// 	{
// 		return;
// 	}
// 	m_lastIBOHandle = iboHandle;

	m_context->IASetIndexBuffer( iboHandle, DXGI_FORMAT_R32_UINT, 0);

}

void RenderContext::SetDepth( eDepthCompareMode compareMode, eDepthWriteMode writeMode )
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	memset( &dsDesc, 0, sizeof( dsDesc ) );
	dsDesc.DepthEnable = true;
	dsDesc.StencilEnable = false;
	
	switch( compareMode )
	{
		case eDepthCompareMode::COMPARE_ALWAYS:
		{

			dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			break;
		}
		case eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL:
		{

			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			break;
		}
		case eDepthCompareMode::COMPARE_GREATER_THAN:
		{
			dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
			break;
		}
		default:
			break;
	}

	switch( writeMode )
	{
	case eDepthWriteMode::WRITE_ALL:
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		break;
	case eDepthWriteMode::WRITE_NONE:
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		break;
	default:
		break;
	}


	if( nullptr != m_depthStencilState )
	{
		DX_SAFE_RELEASE( m_depthStencilState );
	}

	m_device->CreateDepthStencilState( &dsDesc, &m_depthStencilState );
	m_context->OMSetDepthStencilState( m_depthStencilState, 0 );
}

void RenderContext::BindUniformBuffer( unsigned int slot, RenderBuffer* ubo )
{
	ID3D11Buffer* uboHandle = ubo->GetHandle();

	m_context->VSSetConstantBuffers( slot, 1, &uboHandle );
	m_context->PSSetConstantBuffers( slot, 1, &uboHandle );
}

void RenderContext::SetModelMatrix( Mat44 const& model )
{
	ModelData modelData;
	modelData.model = model;
	m_modelUBO->Update( &modelData, sizeof( modelData ), sizeof( modelData ) );
}

Texture* RenderContext::GetBackBuffer()
{
	return m_swapchain->GetBackBuffer();
}

Texture* RenderContext::CreateDepthStencilTarget()
{
	Texture* backBuffer = m_swapchain->GetBackBuffer();
	IntVec2 backBufferSize = backBuffer->GetTexelSize();


	D3D11_TEXTURE2D_DESC desc;
	desc.Width = backBufferSize.x;
	desc.Height = backBufferSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1; // MSAA
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT; // if you do mip-chians, we need this to be GPU/DEFAULT
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	//DirectX Creation
	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc, nullptr, &texHandle );

	Texture* newTexture = new Texture( this, texHandle, nullptr );
	//m_Textures.push_back( newTexture );




	D3D11_DEPTH_STENCIL_DESC dsDesc;
	memset( &dsDesc, 0, sizeof(dsDesc));
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.StencilEnable = false;


	m_device->CreateDepthStencilState( &dsDesc, &m_depthStencilState );
	m_context->OMSetDepthStencilState( m_depthStencilState, 1 );

	return newTexture;
}

Texture* RenderContext::CreateTextureFromColor( Rgba8 color, IntVec2 texelSize )
{
	unsigned char* imageDataRawArray = new unsigned char[texelSize.x * texelSize.y * 4];
	
	for( int dataIndex = 0; dataIndex < texelSize.x * texelSize.y; dataIndex++ )
	{
		int currentIndex = dataIndex * 4;
		imageDataRawArray[currentIndex]		 = color.r;
		imageDataRawArray[currentIndex + 1]	 = color.g;
		imageDataRawArray[currentIndex + 2]	 = color.b;
		imageDataRawArray[currentIndex + 3]	 = color.a;
		
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = texelSize.x;
	desc.Height = texelSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1; // MSAA
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = imageDataRawArray;
	initialData.SysMemPitch = texelSize.x * 4;
	initialData.SysMemSlicePitch = 0;

	//DirectX Creation
	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc, &initialData, &texHandle );

	// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	//stbi_image_free( imageData );
	////const IntVec2 texelSize( imageTexelSizeX, imageTexelSizeY );
	//Texture* newTexture = new Texture(textureID, texelSize, imageFilePath);
	Texture* newTexture = new Texture( this, texHandle, nullptr );
	m_Textures.push_back( newTexture );
	return newTexture;
}

Texture* RenderContext::CreateTextureFromFile(const char* filePath)
{
	//const char* imageFilePath = "Data/Images/Test_StbiFlippedAndOpenGL.png";
	const char* imageFilePath = filePath;
	//unsigned int textureID = 0;
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)


	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );

	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ));
	GUARANTEE_OR_DIE( numComponents == 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = imageTexelSizeX;
	desc.Height = imageTexelSizeY;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1; // MSAA
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE; // if you do mip-chians, we need this to be GPU/DEFAULT
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = imageData;
	initialData.SysMemPitch = imageTexelSizeX * 4;
	initialData.SysMemSlicePitch = 0;

	//DirectX Creation
	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc, &initialData, &texHandle );

							// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	stbi_image_free( imageData );
	const IntVec2 texelSize(imageTexelSizeX,imageTexelSizeY);
	//Texture* newTexture = new Texture(textureID, texelSize, imageFilePath);
	Texture* newTexture = new Texture(this, texHandle, imageFilePath);
	m_Textures.push_back(newTexture);
	return newTexture;

}


Texture* RenderContext::CreateOrGetTextureFromFile(const char* filePath)
{
	for( int textureIndex = 0; textureIndex < m_Textures.size(); textureIndex++ )
	{
		if( m_Textures[textureIndex]->IsFileInTexture( filePath ) )
		{
			return m_Textures[textureIndex];
		}
	}
	Texture* newTexture = CreateTextureFromFile(filePath);
	//m_Textures.push_back(newTexture);

	return newTexture;
}


Shader* RenderContext::GetOrCreateShader( char const* filename )
{
	for( size_t shaderIndex = 0; shaderIndex < m_shaders.size(); shaderIndex++ )
	{
		if( nullptr == m_shaders[shaderIndex] )
		{
			continue;
		}

		if( m_shaders[shaderIndex]->m_filename.compare( filename ) == 0 )
		{
			return m_shaders[shaderIndex];
		}
	}

	Shader* newShader = new Shader( this );
	newShader->CreateFromFile( filename );

	m_shaders.push_back( newShader );

	return newShader;
}

//-----------------------------------------------------------------------------------------------

void RenderContext::BindTexture( const Texture* constTex ) 
{
	Texture* texture = const_cast<Texture*>(constTex);
	if( nullptr == constTex )
	{
		texture = m_texWhite;
	}

	TextureView* shaderResourceView = texture->GetOrCreateShaderResourceView();
	ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
	m_context->PSSetShaderResources( 0, 1, &srvHandle ); //srv
}



void RenderContext::BindSampler( Sampler const* constSampler )
{
	Sampler* sampler = const_cast<Sampler*>(constSampler);
	if( nullptr == sampler )
	{
		sampler = m_sampPoint;
	}
	ID3D11SamplerState* samplerHandle = sampler->GetHandle();
	m_context->PSSetSamplers( 0, 1, &samplerHandle );
}

void RenderContext::SetBlendMode( BlendMode blendMode )
{
	float const zeroes[] ={ 0, 0, 0, 0 };

	switch( blendMode )
	{
	case BlendMode::ALPHA: 	m_context->OMSetBlendState( m_alphaBlendStateHandle, zeroes, ~(uint)0 );
		break;
	case BlendMode::ADDITIVE: 	m_context->OMSetBlendState( m_additiveBlendStateHandle, zeroes, ~(uint)0 );
		break;
	case BlendMode::SOLID: 	m_context->OMSetBlendState( m_solidBlendStateHandle, zeroes, ~(uint)0 );
		break;
	default:
		break;
	}

}

void RenderContext::CreateBlendModes()
{
	D3D11_BLEND_DESC alphaDesc;
	alphaDesc.AlphaToCoverageEnable = false;
	alphaDesc.IndependentBlendEnable = false;
	alphaDesc.RenderTarget[0].BlendEnable = true;
	alphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	alphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	alphaDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_device->CreateBlendState( &alphaDesc, &m_alphaBlendStateHandle );


	D3D11_BLEND_DESC additiveDesc;
	additiveDesc.AlphaToCoverageEnable = false;
	additiveDesc.IndependentBlendEnable = false;
	additiveDesc.RenderTarget[0].BlendEnable = true;
	additiveDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	additiveDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	additiveDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_device->CreateBlendState( &additiveDesc, &m_additiveBlendStateHandle );


	D3D11_BLEND_DESC solidDesc;
	solidDesc.AlphaToCoverageEnable = false;
	solidDesc.IndependentBlendEnable = false;
	solidDesc.RenderTarget[0].BlendEnable = false;
	solidDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	solidDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	solidDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;

	solidDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	solidDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	solidDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	solidDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_device->CreateBlendState( &solidDesc, &m_solidBlendStateHandle );
}

bool RenderContext::IsDrawing() const
{
	return m_isDrawing;
}

void RenderContext::BeginCamera( Camera& camera )
{
	#if defined(RENDER_DEBUG)
		m_context->ClearState();		//Can be slow but helps to find bugs
	#endif

	Texture* backbuffer = nullptr;
	if( nullptr == camera.GetColorTarget() )
	{
		backbuffer = m_swapchain->GetBackBuffer();
	}
	else
	{
		backbuffer = camera.GetColorTarget();
	}

	if( camera.m_clearMode & CLEAR_COLOR_BIT )
	{
		//ClearScreen( camera.m_clearColor );
		Rgba8 clearColor = camera.m_clearColor;
		float clearFloats[4];
		clearFloats[0] = clearColor.r / 255.f;
		clearFloats[1] = clearColor.g / 255.f;
		clearFloats[2] = clearColor.b / 255.f;
		clearFloats[3] = clearColor.a / 255.f;


		TextureView* backbuffer_rtv = backbuffer->GetRenderTargetView();

		ID3D11RenderTargetView* rtv = backbuffer_rtv->GetAsRTV();
		m_context->ClearRenderTargetView( rtv, clearFloats );

	}
	if( camera.m_clearMode & CLEAR_DEPTH_BIT )
	{
		Texture* depthStencilTarget = camera.m_depthStencilTarget;
		ClearDepth( depthStencilTarget, camera.m_clearDepth, camera.m_clearStencil );
	}

	//Texture* texture = m_swapchain->GetBackBuffer();
	Texture* texture = backbuffer;
	TextureView* view = texture->GetRenderTargetView();
	ID3D11RenderTargetView* rtv = view->GetAsRTV();

	IntVec2 outputSize = texture->GetTexelSize();
	D3D11_VIEWPORT viewport;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)outputSize.x;
	viewport.Height = (float)outputSize.y;

	// TEMPORARY - this will be moved
	//m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	Texture* depthStencilTarget = camera.m_depthStencilTarget;
	if( depthStencilTarget != nullptr )
	{
		TextureView* depthStencilView = depthStencilTarget->GetOrCreateDepthStencilView();
		ID3D11DepthStencilView* dsv = depthStencilView->GetAsDSV();

		m_context->RSSetViewports( 1, &viewport );
		m_context->OMSetRenderTargets( 1, &rtv, dsv );
	}
	else
	{
		m_context->RSSetViewports( 1, &viewport );
		m_context->OMSetRenderTargets( 1, &rtv, nullptr );
	}

	m_isDrawing = true;

	BindShader( m_currentShader );
	m_context->RSSetState( m_rasterState );
	BindUniformBuffer( 0, m_frameUBO );
	BindTexture( nullptr );
	BindSampler( nullptr );

	if( nullptr == camera.m_cameraUBO )
	{
		camera.m_cameraUBO = new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );

	}

	camera.UpdateCameraUBO();
	BindUniformBuffer( 1, camera.m_cameraUBO );

	if( nullptr == m_modelUBO )
	{
		m_modelUBO = new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );

	}

	Mat44 identity;
	SetModelMatrix(identity);
	BindUniformBuffer( 2, m_modelUBO );
}

void RenderContext::EndCamera( const Camera& camera )
{
	UNUSED(camera);
	
	m_lastVBOHandle = nullptr;
	m_isDrawing = false;
}

void RenderContext::Draw( int numVertexes, int vertexOffset /*= 0 */ )
{
	// So at this point, I need to describe the Vertex Format to the shader
	ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout( Vertex_PCU::LAYOUT );
	m_context->IASetInputLayout( inputLayout );

	m_context->Draw( numVertexes, vertexOffset );
}

void RenderContext::DrawIndexed( int numIndices, int indexOffset, int vertexOffset /*= 0 */ )
{
	ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout( Vertex_PCU::LAYOUT );
	m_context->IASetInputLayout( inputLayout );

	m_context->DrawIndexed( numIndices, indexOffset, vertexOffset );
}

void RenderContext::DrawLine( const Vec2& startPoint, const Vec2& endPoint, const Rgba8& color, float thickness )
{
	Vec2 normalizedDisplacement = (endPoint - startPoint).GetNormalized();
	float radius = thickness * 0.5f;
	Vec2 fwd = normalizedDisplacement * radius;
	Vec2 leftVec = fwd.GetRotated90Degrees();
	Vec2 endLeft = endPoint + fwd + leftVec;
	Vec2 endRight= endPoint + fwd - leftVec;
	Vec2 startLeft = startPoint - fwd + leftVec;
	Vec2 startRight = startPoint - fwd - leftVec;

	Vertex_PCU vertexes[6] =
	{
		Vertex_PCU( Vec3( startRight.x,startRight.y,0.f ),color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( endRight.x,endRight.y,0.f ),color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( endLeft.x,endLeft.y,0.f ),color,Vec2( 0.f,0.f ) ),

		Vertex_PCU( Vec3( startLeft.x,startLeft.y,0.f ),color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( startRight.x,startRight.y,0.f ),color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( endLeft.x,endLeft.y,0.f ),color,Vec2( 0.f,0.f ) )
	};
	
	BindTexture(nullptr);
	DrawVertexArray( 6, vertexes );
}

void RenderContext::DrawRing( const Vec2& center, float radius, const Rgba8& color, float thickness )
{
	Vec2 ringArray[64];
	for( int thetaIndex = 0; thetaIndex < 64; thetaIndex++ )
	{
		ringArray[thetaIndex] = Vec2::MakeFromPolarDegrees( 360.f/64.f * thetaIndex, radius );
		ringArray[thetaIndex] += center;
	}
	for( int ringIndex = 0; ringIndex < 64; ringIndex++ )
	{
		if( ringIndex != 63 )
		{
			BindTexture(nullptr);
			DrawLine( ringArray[ringIndex], ringArray[ringIndex+1], color, thickness );

		}
		else {
			BindTexture(nullptr);
			DrawLine( ringArray[ringIndex], ringArray[0], color, thickness );

		}
	}

}

void RenderContext::DrawDisc( Vec2 const& center, float radius, Rgba8 const& fillColor, Rgba8 const& borderColor, float thickness )
{
	std::vector<Vertex_PCU> vertexArray;

	float increment = 180.f/32.f;
	for( float currentOrientation = 0.f; currentOrientation < 360.f; currentOrientation += increment )
	{
		Vec2 firstVertex = Vec2::MakeFromPolarDegrees( currentOrientation, radius ) + center;
		Vec2 secondVertex = Vec2::MakeFromPolarDegrees( currentOrientation+increment, radius ) + center;

		vertexArray.push_back( Vertex_PCU( Vec3( center ), fillColor, Vec2( 0.f, 0.f ) ) );
		vertexArray.push_back( Vertex_PCU( Vec3( firstVertex ), fillColor, Vec2( 0.f, 0.f ) ) );
		vertexArray.push_back( Vertex_PCU( Vec3( secondVertex ), fillColor, Vec2( 0.f, 0.f ) ) );
	}
	
	BindTexture( nullptr );
	DrawVertexArray(vertexArray);
	DrawRing( center, radius, borderColor, thickness );
}

void RenderContext::DrawAABB2( const AABB2& aabb, const Rgba8& color, float thickness )
{
	DrawLine(aabb.mins, Vec2(aabb.mins.x, aabb.maxs.y), color, thickness);
	DrawLine(aabb.mins, Vec2(aabb.maxs.x, aabb.mins.y), color, thickness);
	DrawLine(Vec2(aabb.maxs.x, aabb.mins.y), aabb.maxs, color, thickness);
	DrawLine(Vec2(aabb.mins.x, aabb.maxs.y), aabb.maxs, color, thickness);

}

// void RenderContext::DrawAABB2Filled( const AABB2& aabb, const Rgba8& color )
// {
// 	Vertex_PCU vertexes[6] =
// 	{
// 		Vertex_PCU( Vec3( aabb.mins.x, aabb.mins.y,0.f ),color,Vec2( 0.f,0.f ) ),
// 		Vertex_PCU( Vec3( aabb.maxs.x, aabb.mins.y,0.f ),color,Vec2( 1.f,0.f ) ),
// 		Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y,0.f ),color,Vec2( 1.f,1.f ) ),
// 
// 		Vertex_PCU( Vec3( aabb.mins.x,aabb.mins.y,0.f ),color,Vec2( 0.f,0.f ) ),
// 		Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y,0.f ),color,Vec2( 1.f,1.f ) ),
// 		Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,0.f ),color,Vec2( 0.f,1.f ) )
// 	};
// 	//BindTexture(nullptr);
// 	DrawVertexArray( 6, vertexes );
// }

void RenderContext::DrawAABB2Filled( const AABB2& aabb, const Rgba8& color, float z )
{
	Vertex_PCU vertexes[6] =
	{
		Vertex_PCU( Vec3( aabb.mins.x, aabb.mins.y, z ),color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.maxs.x, aabb.mins.y, z ),color,Vec2( 1.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y, z ),color,Vec2( 1.f,1.f ) ),

		Vertex_PCU( Vec3( aabb.mins.x,aabb.mins.y, z ),color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y, z ),color,Vec2( 1.f,1.f ) ),
		Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y, z ),color,Vec2( 0.f,1.f ) )
	};
	//BindTexture(nullptr);
	DrawVertexArray( 6, vertexes );
}

void RenderContext::DrawAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax )
{
	Vertex_PCU vertexes[6] =
	{
		Vertex_PCU( Vec3( aabb.mins.x, aabb.mins.y,0.f ),color,uvMin ),
		Vertex_PCU( Vec3( aabb.maxs.x, aabb.mins.y,0.f ),color,Vec2( uvMax.x,uvMin.y ) ),
		Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y,0.f ),color,uvMax ),

		Vertex_PCU( Vec3( aabb.mins.x,aabb.mins.y,0.f ),color,uvMin ),
		Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y,0.f ),color,uvMax ),
		Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,0.f ),color,Vec2( uvMin.x,uvMax.y ) )
	};
	//BindTexture(nullptr);
	DrawVertexArray( 6, vertexes );
}

void RenderContext::DrawRotatedAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax, float orientationDegrees )
{

	Vec2 center = aabb.GetCenter();
	AABB2 localSpaceAABB2 = aabb;
	localSpaceAABB2.Translate(-center);
	Vertex_PCU vertexes[6] =
	{
		Vertex_PCU( Vec3( localSpaceAABB2.mins.x, localSpaceAABB2.mins.y,0.f ),color,uvMin ),
		Vertex_PCU( Vec3( localSpaceAABB2.maxs.x, localSpaceAABB2.mins.y,0.f ),color,Vec2( uvMax.x,uvMin.y ) ),
		Vertex_PCU( Vec3( localSpaceAABB2.maxs.x,localSpaceAABB2.maxs.y,0.f ),color,uvMax ),

		Vertex_PCU( Vec3( localSpaceAABB2.mins.x,localSpaceAABB2.mins.y,0.f ),color,uvMin ),
		Vertex_PCU( Vec3( localSpaceAABB2.maxs.x,localSpaceAABB2.maxs.y,0.f ),color,uvMax ),
		Vertex_PCU( Vec3( localSpaceAABB2.mins.x,localSpaceAABB2.maxs.y,0.f ),color,Vec2( uvMin.x,uvMax.y ) )
	};

	Vertex_PCU::TransformVertexArray(6,vertexes, 1.f, orientationDegrees, center);
	
	//BindTexture(nullptr);
	DrawVertexArray( 6, vertexes );
}

void RenderContext::DrawPolygon2D( Polygon2D const& polygon, Rgba8 const& fillColor, Rgba8 const& borderColor, float thickness )
{
	std::vector<Vertex_PCU> vertexes;

	for( size_t triangleIndex = 0; triangleIndex < polygon.GetTriangleCount(); triangleIndex++ )
	{
		Vec2 pointA;
		Vec2 pointB;
		Vec2 pointC;
		polygon.GetTriangle( &pointA, &pointB, &pointC, triangleIndex );


		Vertex_PCU vertexA(Vec3(pointA), fillColor, Vec2(0.f, 0.f));
		Vertex_PCU vertexB(Vec3(pointB), fillColor, Vec2(0.f, 0.f));
		Vertex_PCU vertexC(Vec3(pointC), fillColor, Vec2(0.f, 0.f));

		vertexes.push_back(vertexA);
		vertexes.push_back(vertexB);
		vertexes.push_back(vertexC);
	}

	BindTexture( nullptr );
	DrawVertexArray(vertexes);

	for( size_t edgeIndex = 0; edgeIndex < polygon.GetEdgeCount(); edgeIndex++ )
	{
		Vec2 pointA;
		Vec2 pointB;
		polygon.GetEdge( &pointA, &pointB, edgeIndex );

		DrawLine( pointA, pointB, borderColor, thickness );
	}
}

BitmapFont* RenderContext::CreateOrGetBitmapFont( const char* bitmapFontFilePathNoExtension )
{
	for( int fontIndex = 0; fontIndex < m_fonts.size(); fontIndex++ )
	{
		if( m_fonts[fontIndex]->GetFontName() == bitmapFontFilePathNoExtension )
		{
			return m_fonts[fontIndex];
		}
	}
	BitmapFont* newBitmapFont = CreateBitMapFontFromFile(bitmapFontFilePathNoExtension);
	m_fonts.push_back(newBitmapFont);

	return newBitmapFont;

}

BitmapFont* RenderContext::CreateBitMapFontFromFile( const char* filePath )
{
	Texture* texture = CreateOrGetTextureFromFile(filePath);
	BitmapFont* font = new BitmapFont(filePath, texture);
	return font;
}


void RenderContext::CreateBlendStates()
{

}

//DrawText is a macro don't call it that!


void RenderContext::DrawTextAtPosition( const char* textstring, const Vec2& textMins, float fontHeight, const Rgba8& tint /*= Rgba8::WHITE*/ )
{
	std::vector<Vertex_PCU> vertexArray;

	m_fonts[0]->AddVertsForText2D(vertexArray, textMins, fontHeight, std::string(textstring), tint);
	const Texture* texture = m_fonts[0]->GetTexture();
	BindTexture(texture);
	DrawVertexArray(vertexArray);
}

void RenderContext::DrawAlignedTextAtPosition( const char* textstring, const AABB2& box, float fontHeight, const Vec2& alignment, const Rgba8& tint /*= Rgba8::WHITE */ )
{
	std::vector<Vertex_PCU> vertexArray;

	m_fonts[0]->AddVertsForTextInBox2D( vertexArray, box, fontHeight, std::string( textstring ), tint, 1.f, alignment );
	const Texture* texture = m_fonts[0]->GetTexture();
	BindTexture( texture );
	DrawVertexArray( vertexArray );
}

void RenderContext::CreateDefaultRasterState()
{
	if( nullptr != m_rasterState )
	{
		DX_SAFE_RELEASE( m_rasterState );
	}

	D3D11_RASTERIZER_DESC desc; //description

	desc.FillMode = D3D11_FILL_SOLID; //full triangle
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = TRUE; //the only reason we're doing this;
	desc.DepthBias = 0U;
	desc.DepthBiasClamp = 0.f;
	desc.SlopeScaledDepthBias = 0.f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	ID3D11Device* device = m_device;
	device->CreateRasterizerState( &desc, &m_rasterState );
	m_context->RSSetState( m_rasterState );
}

void RenderContext::SetCullMode( eCullMode cullMode )
{
	D3D11_RASTERIZER_DESC desc; //description
	m_rasterState->GetDesc( &desc );
	
	switch( cullMode )
	{
	case eCullMode::CULL_BACK:
		desc.CullMode = D3D11_CULL_BACK;
		break;
	case eCullMode::CULL_FRONT:
		desc.CullMode = D3D11_CULL_FRONT;
		break;
	case eCullMode::CULL_NONE:
		desc.CullMode = D3D11_CULL_NONE;
		break;
	default: ERROR_AND_DIE("Invalid Cull Mode");
		break;
	}

	DX_SAFE_RELEASE( m_rasterState );

	ID3D11Device* device = m_device;
	device->CreateRasterizerState( &desc, &m_rasterState );
	m_context->RSSetState( m_rasterState );
	
}

void RenderContext::SetFillMode( eFillMode fillMode )
{
	D3D11_RASTERIZER_DESC desc; //description
	m_rasterState->GetDesc( &desc );

	switch( fillMode )
	{
	case eFillMode::FILL_WIREFRAME: desc.FillMode = D3D11_FILL_WIREFRAME;
		break;
	case eFillMode::FILL_SOLID: desc.FillMode = D3D11_FILL_SOLID;
		break;
	default: ERROR_AND_DIE("Invalid Cull Mode");
		break;
	}

	DX_SAFE_RELEASE( m_rasterState );

	ID3D11Device* device = m_device;
	device->CreateRasterizerState( &desc, &m_rasterState );
	m_context->RSSetState( m_rasterState );
}

void RenderContext::SetFrontFaceWindOrder( eFrontFaceWindOrder windOrder )
{
	D3D11_RASTERIZER_DESC desc; //description
	m_rasterState->GetDesc( &desc );

	switch( windOrder )
	{
	case eFrontFaceWindOrder::COUNTERCLOCKWISE: desc.FrontCounterClockwise = TRUE;
		break;
	case eFrontFaceWindOrder::CLOCKWISE: desc.FrontCounterClockwise = FALSE;
		break;
	default: ERROR_AND_DIE("Invalid Cull Mode");
		break;
	}

	DX_SAFE_RELEASE( m_rasterState );

	ID3D11Device* device = m_device;
	device->CreateRasterizerState( &desc, &m_rasterState );
	m_context->RSSetState( m_rasterState );
}

// create the debug module for us to use (for now, only for reporting)
void RenderContext::CreateDebugModule()
{
	// load the dll
	m_debugModule = ::LoadLibraryA( "Dxgidebug.dll" );
	if( m_debugModule == nullptr ) {
		DebuggerPrintf( "gfx", "Failed to find dxgidebug.dll.  No debug features enabled." );
	}
	else {
		// find a function in the loaded dll
		typedef HRESULT( WINAPI* GetDebugModuleCB )(REFIID, void**);
		GetDebugModuleCB cb = (GetDebugModuleCB) ::GetProcAddress( (HMODULE)m_debugModule, "DXGIGetDebugInterface" );

		// create our debug object
		HRESULT hr = cb( __uuidof(IDXGIDebug), (void**)&m_debug );
		GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create debug module" );
	}
}

// cleanup after ourselves
void RenderContext::DestroyDebugModule()
{
	if( nullptr != m_debug ) {
		DX_SAFE_RELEASE( m_debug );   // release our debug object
		FreeLibrary( (HMODULE)m_debugModule ); // unload the dll

		m_debug = nullptr;
		m_debugModule = nullptr;
	}
}

// This method will list all current live D3D objects, types, and reference counts
void RenderContext::ReportLiveObjects()
{
	if( nullptr != m_debug ) {
		m_debug->ReportLiveObjects( DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL );
	}
}