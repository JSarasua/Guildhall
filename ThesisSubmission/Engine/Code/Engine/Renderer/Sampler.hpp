#pragma once

struct ID3D11SamplerState;
class RenderContext;

enum eSamplerType
{
	SAMPLER_POINT,		// pixelated look (also called nearest)
	SAMPLER_BILINEAR,	// smoother look - no mips
};

enum eSamplerEdgeMode
{
	SAMPLER_WRAP,
	SAMPLER_MIRROR,
	SAMPLER_CLAMP,
	SAMPLER_BORDER
};

class Sampler
{
public:
	Sampler( RenderContext* ctx, eSamplerType type, eSamplerEdgeMode edgeMode = SAMPLER_CLAMP );
	~Sampler();

	ID3D11SamplerState* GetHandle() const { return m_handle; }

public:
	RenderContext* m_owner;
	ID3D11SamplerState* m_handle;
};