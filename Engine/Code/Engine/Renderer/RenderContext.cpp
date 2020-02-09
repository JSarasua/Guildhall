
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include <gl/gl.h>	
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb_image.h"
#pragma comment( lib, "opengl32" )

extern HWND g_hWnd;


void RenderContext::StartUp()
{
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void RenderContext::BeginFrame()
{

}

void RenderContext::EndFrame()
{

}

void RenderContext::Shutdown()
{
	
}

void RenderContext::ClearScreen( const Rgba8& clearColor )
{
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear( GL_COLOR_BUFFER_BIT );

}

void RenderContext::DrawVertexArray(int numVertexes, const Vertex_PCU* vertexes ) const
{
	glBegin( GL_TRIANGLES );
	for( int beginVertexes = 0; beginVertexes < numVertexes; beginVertexes++ )
	{
		glTexCoord2f(vertexes[beginVertexes].uvTexCoords.x, vertexes[beginVertexes].uvTexCoords.y);
		glColor4ub( vertexes[beginVertexes].tint.r, vertexes[beginVertexes].tint.g, vertexes[beginVertexes].tint.b, vertexes[beginVertexes].tint.a );
		glVertex3f( vertexes[beginVertexes].position.x, vertexes[beginVertexes].position.y, vertexes[beginVertexes].position.z );
	}
	glEnd();
}

void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU>& vertexes ) const
{
	if( !vertexes.empty() )
	{
	DrawVertexArray( static_cast<int>(vertexes.size()), &vertexes[0] );
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

Texture* RenderContext::CreateTextureFromFile(const char* filePath)
{
	//const char* imageFilePath = "Data/Images/Test_StbiFlippedAndOpenGL.png";
	const char* imageFilePath = filePath;
	unsigned int textureID = 0;
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)


	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );

	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ));
	GUARANTEE_OR_DIE( numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );


	// Enable OpenGL texturing
	glEnable( GL_TEXTURE_2D );

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, (GLuint*)&textureID );

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_2D, textureID );

	// Set texture clamp vs. wrap (repeat) default settings
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // GL_CLAMP or GL_REPEAT
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); // GL_CLAMP or GL_REPEAT

																	// Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	// Pick the appropriate OpenGL format (RGB or RGBA) for this texel data
	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if( numComponents == 3 )
	{
		bufferFormat = GL_RGB;
	}
	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; technically allows us to translate into a different texture format as we upload to OpenGL


	// Upload the image texel data (raw pixels bytes) to OpenGL under this textureID
	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		imageTexelSizeX,	// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
		imageTexelSizeY,	// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1, recommend 0)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
		imageData );		// Address of the actual pixel data bytes/buffer in system memory

							// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	stbi_image_free( imageData );
	const IntVec2 texelSize(imageTexelSizeX,imageTexelSizeY);
	Texture* newTexture = new Texture(textureID, texelSize, imageFilePath);
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
	m_Textures.push_back(newTexture);

	return newTexture;
}


//-----------------------------------------------------------------------------------------------

void RenderContext::BindTexture( const Texture* texture ) const
{
	if( texture )
	{
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, texture->GetTextureID() );
	}
	else
	{
		glDisable( GL_TEXTURE_2D );
	}
}



void RenderContext::SetBlendMode( BlendMode blendMode )
{
	if( blendMode == BlendMode::ALPHA )
	{
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
	else if( blendMode == BlendMode::ADDITIVE )
	{
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	}
	else
	{
		ERROR_AND_DIE( Stringf( "Unknown/unsupported blend mode #%i", blendMode) );
	}
}

void RenderContext::BeginCamera( const Camera& camera )
{
	glLoadIdentity();
	//glViewport(0,0,800,400);
	glOrtho(camera.GetOrthoBottomLeft().x, camera.GetOrthoTopRight().x, camera.GetOrthoBottomLeft().y, camera.GetOrthoTopRight().y, 0.f, 1.f);
}

void RenderContext::BeginCamera( const Camera& camera, Viewport viewPort )
{
	RECT clientRect;
	GetClientRect( g_hWnd, &clientRect );
	AABB2 clientBounds( (float)clientRect.left, (float)clientRect.top, (float)clientRect.right, (float)clientRect.bottom );
	AABB2 singleBox = clientBounds.GetTranslated(-clientBounds.mins);
	singleBox.maxs /= 2.f;
	
	glLoadIdentity();
	switch( viewPort )
	{
	case Viewport::TopLeft:
		glViewport(0,(int)singleBox.maxs.y,(int)singleBox.maxs.x, (int)singleBox.maxs.y);
		break;
	case Viewport::TopRight:
		glViewport( (int)singleBox.maxs.x, (int)singleBox.maxs.y, (int)singleBox.maxs.x, (int)singleBox.maxs.y );
		break;
	case Viewport::BottomLeft:
		glViewport( 0, 0, (int)singleBox.maxs.x, (int)singleBox.maxs.y );
		break;
	case Viewport::BottomRight:
		glViewport( (int)singleBox.maxs.x, 0, (int)singleBox.maxs.x, (int)singleBox.maxs.y );
		break;
	case Viewport::FullScreen:
		glViewport( 0, 0, (int)clientBounds.maxs.x, (int)clientBounds.maxs.y );
		break;
	default:
		break;
	}
	glOrtho(camera.GetOrthoBottomLeft().x, camera.GetOrthoTopRight().x, camera.GetOrthoBottomLeft().y, camera.GetOrthoTopRight().y, 0.f, 1.f);
}

void RenderContext::EndCamera( const Camera& camera )
{
	UNUSED(camera);
}



void RenderContext::DrawLine( const Vec2& startPoint, const Vec2& endPoint, const Rgba8& color, float thickness ) const
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

void RenderContext::DrawRing( const Vec2& center, float radius, const Rgba8& color, float thickness ) const
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

void RenderContext::DrawDisc( Vec2 const& center, float radius, Rgba8 const& fillColor, Rgba8 const& borderColor, float thickness ) const
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

void RenderContext::DrawAABB2( const AABB2& aabb, const Rgba8& color, float thickness ) const
{
	DrawLine(aabb.mins, Vec2(aabb.mins.x, aabb.maxs.y), color, thickness);
	DrawLine(aabb.mins, Vec2(aabb.maxs.x, aabb.mins.y), color, thickness);
	DrawLine(Vec2(aabb.maxs.x, aabb.mins.y), aabb.maxs, color, thickness);
	DrawLine(Vec2(aabb.mins.x, aabb.maxs.y), aabb.maxs, color, thickness);

}

void RenderContext::DrawAABB2Filled( const AABB2& aabb, const Rgba8& color ) const
{
	Vertex_PCU vertexes[6] =
	{
		Vertex_PCU( Vec3( aabb.mins.x, aabb.mins.y,0.f ),color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.maxs.x, aabb.mins.y,0.f ),color,Vec2( 1.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y,0.f ),color,Vec2( 1.f,1.f ) ),

		Vertex_PCU( Vec3( aabb.mins.x,aabb.mins.y,0.f ),color,Vec2( 0.f,0.f ) ),
		Vertex_PCU( Vec3( aabb.maxs.x,aabb.maxs.y,0.f ),color,Vec2( 1.f,1.f ) ),
		Vertex_PCU( Vec3( aabb.mins.x,aabb.maxs.y,0.f ),color,Vec2( 0.f,1.f ) )
	};
	//BindTexture(nullptr);
	DrawVertexArray( 6, vertexes );
}

void RenderContext::DrawAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax ) const
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

void RenderContext::DrawRotatedAABB2Filled( const AABB2& aabb, const Rgba8& color, const Vec2& uvMin, const Vec2& uvMax, float orientationDegrees ) const
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

void RenderContext::DrawPolygon2D( Polygon2D const& polygon, Rgba8 const& fillColor, Rgba8 const& borderColor, float thickness ) const
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


//DrawText is a macro don't call it that!


void RenderContext::DrawTextAtPosition( const char* textstring, const Vec2& textMins, float fontHeight, const Rgba8& tint /*= Rgba8::WHITE*/ ) const
{
	std::vector<Vertex_PCU> vertexArray;

	m_fonts[0]->AddVertsForText2D(vertexArray, textMins, fontHeight, std::string(textstring), tint);
	const Texture* texture = m_fonts[0]->GetTexture();
	BindTexture(texture);
	DrawVertexArray(vertexArray);
}

void RenderContext::DrawAlignedTextAtPosition( const char* textstring, const AABB2& box, float fontHeight, const Vec2& alignment, const Rgba8& tint /*= Rgba8::WHITE */ ) const
{
	std::vector<Vertex_PCU> vertexArray;

	m_fonts[0]->AddVertsForTextInBox2D( vertexArray, box, fontHeight, std::string( textstring ), tint, 1.f, alignment );
	const Texture* texture = m_fonts[0]->GetTexture();
	BindTexture( texture );
	DrawVertexArray( vertexArray );
}

