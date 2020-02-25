#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
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

	m_immediateVBO = new VertexBuffer( this, MEMORY_HINT_DYNAMIC );

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

	delete m_swapchain;
	m_swapchain = nullptr;


	delete m_immediateVBO;
	m_swapchain = nullptr;

	delete m_frameUBO;
	m_frameUBO = nullptr;

	delete m_sampPoint;
	m_sampPoint = nullptr;

	//delete m_texWhite;


	DX_SAFE_RELEASE(m_device);
	DX_SAFE_RELEASE(m_context);

	DX_SAFE_RELEASE( m_alphaBlendStateHandle );
	DX_SAFE_RELEASE( m_additiveBlendStateHandle );
	DX_SAFE_RELEASE( m_solidBlendStateHandle );

	for( int textureIndex = 0; textureIndex < m_Textures.size(); textureIndex++ )
	{
		delete m_Textures[textureIndex];
		m_Textures[textureIndex] = nullptr;
	}
	m_Textures.clear();

	m_texWhite = nullptr;

	ReportLiveObjects();    // do our leak reporting just before shutdown to give us a better detailed list; 
	DestroyDebugModule();
}


void RenderContext::UpdateFrameTime( float deltaSeconds )
{
	FrameData framedata;
	framedata.systemTime = (float)GetCurrentTimeSeconds();
	framedata.systemDeltaTime = deltaSeconds;

	m_frameUBO->Update( &framedata, sizeof(framedata), sizeof(framedata) );
}

void RenderContext::ClearScreen( const Rgba8& clearColor )
{
	UNUSED(clearColor);

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

void RenderContext::DrawMesh( GPUMesh* mesh )
{
	m_immediateVBO = mesh->m_vertices;

	BindVertexBuffer( m_immediateVBO );

	Draw( mesh->m_vertexCount, 0 );
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
	m_context->RSSetState( m_currentShader->m_rasterState ); //use defaults
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

void RenderContext::BindUniformBuffer( unsigned int slot, RenderBuffer* ubo )
{
	ID3D11Buffer* uboHandle = ubo->GetHandle();

	m_context->VSSetConstantBuffers( slot, 1, &uboHandle );
	m_context->PSSetConstantBuffers( slot, 1, &uboHandle );
}

Texture* RenderContext::CreateTextureFromColor( Rgba8 color )
{
	// make a 1x1 texture of that color, and return it;
	unsigned char imageDataRaw[4];
	imageDataRaw[0] = color.r;
	imageDataRaw[1] = color.g;
	imageDataRaw[2] = color.b;
	imageDataRaw[3] = color.a;

	int imageTexelSizeX = 1; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 1; // This will be filled in for us to indicate image height
	//int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	//int numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
	//unsigned char* imageData = stbi_load_from_memory(imageDataRaw, 4,&imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested);
	

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
	initialData.pSysMem = imageDataRaw;
	initialData.SysMemPitch = imageTexelSizeX * 4;
	initialData.SysMemSlicePitch = 0;

	//DirectX Creation
	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc, &initialData, &texHandle );

	// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	//stbi_image_free( imageData );
	const IntVec2 texelSize( imageTexelSizeX, imageTexelSizeY );
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


	if( camera.m_clearMode & CLEAR_COLOR_BIT )
	{
		//ClearScreen( camera.m_clearColor );
		Rgba8 clearColor = camera.m_clearColor;
		float clearFloats[4];
		clearFloats[0] = clearColor.r / 255.f;
		clearFloats[1] = clearColor.g / 255.f;
		clearFloats[2] = clearColor.b / 255.f;
		clearFloats[3] = clearColor.a / 255.f;

		Texture* backbuffer = nullptr;

		if( nullptr == camera.GetColorTarget() )
		{
			backbuffer = m_swapchain->GetBackBuffer();
		}
		else
		{
			backbuffer = camera.GetColorTarget();
		}
		TextureView* backbuffer_rtv = backbuffer->GetRenderTargetView();

		ID3D11RenderTargetView* rtv = backbuffer_rtv->GetAsRTV();
		m_context->ClearRenderTargetView( rtv, clearFloats );

	}

	Texture* texture = m_swapchain->GetBackBuffer();
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


	m_context->RSSetViewports( 1, &viewport );
	m_context->OMSetRenderTargets( 1, &rtv, nullptr );

	m_isDrawing = true;

	BindShader( m_currentShader );

	BindUniformBuffer( 0, m_frameUBO );

	BindTexture( nullptr );
	BindSampler( nullptr );




	if( nullptr == camera.m_cameraUBO )
	{
		camera.m_cameraUBO = new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );

	}

	//CameraData
	CameraData camData;
	camData.projection = camera.GetProjection();
	camData.view = Mat44::CreateTranslation3D( -camera.GetPosition() );
// 	camData.mins = camera.GetOrthoBottomLeft();
// 	camData.maxs = camera.GetOrthoTopRight();

	camera.m_cameraUBO->Update( & camData, sizeof( camData ), sizeof( camData ) );


	BindUniformBuffer( 1, camera.m_cameraUBO );
}


//-----DONT USE THIS BEGINCAMERA, ITS AN OLD METHOD FOR USING SPLIT SCREEN
void RenderContext::BeginCamera( const Camera& camera, Viewport viewPort )
{

	UNUSED(camera);
	UNUSED(viewPort);
	UNIMPLEMENTED();
// 	RECT clientRect;
// 	GetClientRect( g_hWnd, &clientRect );
// 	AABB2 clientBounds( (float)clientRect.left, (float)clientRect.top, (float)clientRect.right, (float)clientRect.bottom );
// 	AABB2 singleBox = clientBounds.GetTranslated(-clientBounds.mins);
// 	singleBox.maxs /= 2.f;
// 	
// 	glLoadIdentity();
// 	switch( viewPort )
// 	{
// 	case Viewport::TopLeft:
// 		glViewport(0,(int)singleBox.maxs.y,(int)singleBox.maxs.x, (int)singleBox.maxs.y);
// 		break;
// 	case Viewport::TopRight:
// 		glViewport( (int)singleBox.maxs.x, (int)singleBox.maxs.y, (int)singleBox.maxs.x, (int)singleBox.maxs.y );
// 		break;
// 	case Viewport::BottomLeft:
// 		glViewport( 0, 0, (int)singleBox.maxs.x, (int)singleBox.maxs.y );
// 		break;
// 	case Viewport::BottomRight:
// 		glViewport( (int)singleBox.maxs.x, 0, (int)singleBox.maxs.x, (int)singleBox.maxs.y );
// 		break;
// 	case Viewport::FullScreen:
// 		glViewport( 0, 0, (int)clientBounds.maxs.x, (int)clientBounds.maxs.y );
// 		break;
// 	default:
// 		break;
// 	}
// 	glOrtho(camera.GetOrthoBottomLeft().x, camera.GetOrthoTopRight().x, camera.GetOrthoBottomLeft().y, camera.GetOrthoTopRight().y, 0.f, 1.f);
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