#pragma once


//-----------------------------------------------------------------------------------------------
struct Rgba8
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;

public:
	// Construction/Destruction
	~Rgba8() {}												// destructor (do nothing)
	Rgba8() {}												// default constructor (do nothing)
	Rgba8( const Rgba8& copyFrom );							// copy constructor (from another Rgba8)
	Rgba8( float const* colorArray);						// 4 elements in array
	explicit Rgba8( unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255 );		// 

	bool		operator==( const Rgba8& compare ) const;
	void		operator=( const Rgba8& copyFrom );
	const Rgba8		operator+( const Rgba8& copyRgba8 ) const;

	void SetFromText(const char* text);
	void TintColorBy( Rgba8 const& tint );
	void ToFloatArray( float* colorArray ) const;

	static Rgba8 LerpColorTo( Rgba8 const& startColor, Rgba8 const& endColor, float lerpValue );
	static Rgba8 LerpColorAsHSL( Rgba8 const& startColor, Rgba8 const& endColor, float lerpValue );
	static void RGBToHSL( Rgba8 const& rgba, float* hsl );
	static void HSLToRGB( float const* hsl, Rgba8& rgba );

	static float hue2RGB( float p, float q, float t );

	static const Rgba8 WHITE;
	static const Rgba8 RED;
	static const Rgba8 GREEN;
	static const Rgba8 BLUE;
	static const Rgba8 BLACK;
	static const Rgba8 GREY;

};
