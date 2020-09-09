#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

Sampler::Sampler( RenderContext* ctx, eSamplerType type, eSamplerEdgeMode edgeMode )
{
	D3D11_TEXTURE_ADDRESS_MODE addressMode;
	
	switch( edgeMode )
	{
	case SAMPLER_WRAP: addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case SAMPLER_MIRROR: addressMode = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;
	case SAMPLER_CLAMP: addressMode = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case SAMPLER_BORDER: addressMode = D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	default:
		ERROR_AND_DIE( "Invalid Sampler EdgeMode");
		break;
	}

	m_handle = nullptr;
	m_owner = ctx;

	ID3D11Device* dev = ctx->m_device;

	D3D11_SAMPLER_DESC desc;
	if( type == SAMPLER_POINT )
	{
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	}
	else if( type == SAMPLER_BILINEAR )
	{
		desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	}
	desc.AddressU = addressMode;
	desc.AddressV = addressMode;
	desc.AddressW = addressMode;

	desc.MipLODBias = 0.f;
	desc.MaxAnisotropy = 0;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.BorderColor[0] = 0.f;
	desc.BorderColor[1] = 0.f;
	desc.BorderColor[2] = 0.f;
	desc.BorderColor[3] = 0.f;

	desc.MinLOD = 0.f;
	desc.MaxLOD = 0.f;

	dev->CreateSamplerState( &desc, &m_handle );
}

Sampler::~Sampler()
{
	DX_SAFE_RELEASE(m_handle);
}
