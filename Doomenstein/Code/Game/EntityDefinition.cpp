#include "Game/EntityDefinition.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"

extern RenderContext* g_theRenderer;

std::map< std::string, EntityDefinition* > EntityDefinition::s_definitions;

EntityDefinition::EntityDefinition( XmlElement const& element )
{
	m_type = element.Name();
	g_theConsole->GuaranteeOrError( m_type == "Actor" || m_type == "Entity" || m_type == "Projectile" || m_type == "Portal", "Invalid Entity Type" );

	XmlElement const* physicsElement = element.FirstChildElement( "Physics" );
	if( physicsElement )
	{
		m_physicsRadius = ParseXMLAttribute( *physicsElement, "radius", 0.1f );
		m_height = ParseXMLAttribute( *physicsElement, "height", 0.75f );
		m_eyeHeight = ParseXMLAttribute( *physicsElement, "eyeHeight", 0.5f );
		m_walkSpeed = ParseXMLAttribute( *physicsElement, "walkSpeed", 1.f );
		m_canBePushedByWalls = true;
		m_canBePushedByEntities = true;
		m_canPushEntities = true;
		m_mass = 1.f;
	}


	XmlElement const* appearanceElement = element.FirstChildElement( "Appearance" );
	if( appearanceElement )
	{
		m_drawSize = ParseXMLAttribute( *appearanceElement, "size", Vec2( 0.f, 0.f ) );
		m_billBoardType = ParseXMLAttribute( *appearanceElement, "billboard", "INVALID" );

		if( m_billBoardType == "INVALID" )
		{
			m_billBoardType = "CameraFacingXY";
			g_theConsole->ErrorString( "ERROR: Did not find billboard attribute in Appearance" );
		}

		g_theConsole->GuaranteeOrError( !m_drawSize.IsAlmostEqual( Vec2() ), "ERROR: Need draw size not = to 0" );
		
		m_spriteAnimStates = new EntitySpriteAnimStates( *appearanceElement );
	}


	m_isValid = true;
}

void EntityDefinition::InitializeEntityDefinitions( XmlElement const& rootEntityDefinitionElement )
{
	std::string rootName = rootEntityDefinitionElement.Name();
	g_theConsole->GuaranteeOrError( rootName == "EntityTypes", Stringf("ERROR: Root name must be [EntityTypes], %s is invalid", rootName.c_str() ) );

	for( XmlElement const* entityTypeElement = rootEntityDefinitionElement.FirstChildElement(); entityTypeElement; entityTypeElement = entityTypeElement->NextSiblingElement() )
	{
		XmlAttribute const* entityTypeNameAttribute = entityTypeElement->FindAttribute( "name" );
		if( entityTypeNameAttribute )
		{
			std::string entityTypeName = entityTypeNameAttribute->Value();
			EntityDefinition* entityDef = new EntityDefinition( *entityTypeElement );

			if( entityDef->m_isValid )
			{
				s_definitions[entityTypeName] = entityDef;
			}
			else
			{
				delete entityDef;
				entityDef = nullptr;
			}
		}
		else
		{
			g_theConsole->ErrorString( "Missing name for EntityType" );
		}
	}
}

float EntityDefinition::GetEyeHeight() const
{
	return m_eyeHeight;
}

float EntityDefinition::GetHeight() const
{
	return m_height;
}

float EntityDefinition::GetMass() const
{
	return m_mass;
}

float EntityDefinition::GetPhysicsRadius() const
{
	return m_physicsRadius;
}

float EntityDefinition::GetWalkSpeed() const
{
	return m_walkSpeed;
}

bool EntityDefinition::IsPushedByWalls() const
{
	return m_canBePushedByWalls;
}

bool EntityDefinition::IsPushedByEntities() const
{
	return m_canBePushedByEntities;
}

bool EntityDefinition::CanPushEntities() const
{
	return m_canPushEntities;
}

std::string const& EntityDefinition::GetName() const
{
	return m_name;
}

std::string const& EntityDefinition::GetType() const
{
	return m_type;
}

SpriteAnimMap::SpriteAnimMap( XmlElement const& entityAnimStateElem, SpriteSheet* spriteSheet )
{
	m_spriteSheet = spriteSheet;

	std::string frontIndexes =			ParseXMLAttribute( entityAnimStateElem, "front", "INVALID" );
	std::string frontLeftIndexes =		ParseXMLAttribute( entityAnimStateElem, "frontLeft", "INVALID" );
	std::string frontRightIndexes =		ParseXMLAttribute( entityAnimStateElem, "frontRight", "INVALID" );
	std::string leftIndexes =			ParseXMLAttribute( entityAnimStateElem, "left", "INVALID" );
	std::string rightIndexes =			ParseXMLAttribute( entityAnimStateElem, "right", "INVALID" );
	std::string backIndexes =			ParseXMLAttribute( entityAnimStateElem, "back", "INVALID" );
	std::string backLeftIndexes =		ParseXMLAttribute( entityAnimStateElem, "backLeft", "INVALID" );
	std::string backRightIndexes =		ParseXMLAttribute( entityAnimStateElem, "backRight", "INVALID" );

	AddSpriteIndexesToMapIfValid( "front", frontIndexes );
	AddSpriteIndexesToMapIfValid( "frontLeft", frontLeftIndexes );
	AddSpriteIndexesToMapIfValid( "frontRight", frontRightIndexes );
	AddSpriteIndexesToMapIfValid( "left", leftIndexes );
	AddSpriteIndexesToMapIfValid( "right", rightIndexes );
	AddSpriteIndexesToMapIfValid( "back", backIndexes );
	AddSpriteIndexesToMapIfValid( "backLeft", backLeftIndexes );
	AddSpriteIndexesToMapIfValid( "backRight", backRightIndexes );
}

SpriteAnimMap::~SpriteAnimMap()
{

}

SpriteAnimDefinition const* SpriteAnimMap::GetSpriteAnimDefinition( Vec2 const& localDirection ) const
{
	float currentMaxDot = 0.f;
	
	auto iter = m_spriteanims.find( "front" );
	auto currentMaxDotIter = iter;
	if( iter != m_spriteanims.end() )
	{
		currentMaxDot = DotProduct2D( localDirection, Vec2 ( 1.f, 0.f ) );
		currentMaxDotIter = iter;
	}

	iter = m_spriteanims.find( "frontLeft" );
	if( iter != m_spriteanims.end() )
	{
		float localDot = DotProduct2D( localDirection, Vec2( SQRT_TWO, SQRT_TWO ) );
		if( localDot > currentMaxDot )
		{
			currentMaxDot = localDot;
			currentMaxDotIter = iter;
		}
	}

	iter = m_spriteanims.find( "frontRight" );
	if( iter != m_spriteanims.end() )
	{
		float localDot = DotProduct2D( localDirection, Vec2( SQRT_TWO, -SQRT_TWO ) );
		if( localDot > currentMaxDot )
		{
			currentMaxDot = localDot;
			currentMaxDotIter = iter;
		}
	}

	iter = m_spriteanims.find( "left" );
	if( iter != m_spriteanims.end() )
	{
		float localDot = DotProduct2D( localDirection, Vec2( 0.f, 1.f ) );
		if( localDot > currentMaxDot )
		{
			currentMaxDot = localDot;
			currentMaxDotIter = iter;
		}
	}

	iter = m_spriteanims.find( "right" );
	if( iter != m_spriteanims.end() )
	{
		float localDot = DotProduct2D( localDirection, Vec2( 0.f, -1.f ) );
		if( localDot > currentMaxDot )
		{
			currentMaxDot = localDot;
			currentMaxDotIter = iter;
		}
	}

	iter = m_spriteanims.find( "backLeft" );
	if( iter != m_spriteanims.end() )
	{
		float localDot = DotProduct2D( localDirection, Vec2( -SQRT_TWO, SQRT_TWO ) );
		if( localDot > currentMaxDot )
		{
			currentMaxDot = localDot;
			currentMaxDotIter = iter;
		}
	}

	iter = m_spriteanims.find( "backRight" );
	if( iter != m_spriteanims.end() )
	{
		float localDot = DotProduct2D( localDirection, Vec2( -SQRT_TWO, SQRT_TWO ) );
		if( localDot > currentMaxDot )
		{
			currentMaxDot = localDot;
			currentMaxDotIter = iter;
		}
	}

	iter = m_spriteanims.find( "back" );
	if( iter != m_spriteanims.end() )
	{
		float localDot = DotProduct2D( localDirection, Vec2( -1.f, 0.f ) );
		if( localDot > currentMaxDot )
		{
			currentMaxDot = localDot;
			currentMaxDotIter = iter;
		}
	}


	return currentMaxDotIter->second;
}

void SpriteAnimMap::AddSpriteIndexesToMapIfValid( std::string const& direction, std::string const& indexString )
{
	if( indexString != "INVALID" )
	{
		std::vector<int> spriteIndexes = GetSpriteIndexes( indexString );
		SpriteAnimDefinition* newSpriteAnimDef = new SpriteAnimDefinition( *m_spriteSheet, spriteIndexes, 30.f );
	
		m_spriteanims[direction] = newSpriteAnimDef;
	}
}

std::vector<int> SpriteAnimMap::GetSpriteIndexes( std::string const& indexString )
{
	Strings spriteIndexesStr = SplitStringOnDelimeter( indexString.c_str(), ',' );
	std::vector<int> spriteIndexes;

	for( size_t spriteStrIndex = 0; spriteStrIndex < spriteIndexesStr.size(); spriteStrIndex++ )
	{

		int spriteIndex = GetValueFromString( spriteIndexesStr[spriteStrIndex], 1 );
		spriteIndexes.push_back( spriteIndex );
	}

	return spriteIndexes;
}

EntitySpriteAnimStates::EntitySpriteAnimStates( XmlElement const& entityAppearanceElem )
{
	std::string spriteSheetStr = ParseXMLAttribute( entityAppearanceElem, "spriteSheet", "INVALID" );
	IntVec2 spriteSheetLayout = ParseXMLAttribute( entityAppearanceElem, "layout", IntVec2() );

	Texture* spriteTex = g_theRenderer->CreateOrGetTextureFromFile( spriteSheetStr.c_str() );
	SpriteSheet* spriteSheet = new SpriteSheet( *spriteTex, spriteSheetLayout );

	for( XmlElement const* element = entityAppearanceElem.FirstChildElement(); element; element = element->NextSiblingElement() )
	{
		std::string animStateName = element->Name();
		SpriteAnimMap* newSpriteAnimMap = new SpriteAnimMap( *element, spriteSheet );

		m_spriteAnimStates[animStateName] = newSpriteAnimMap;
	}


}

EntitySpriteAnimStates::~EntitySpriteAnimStates()
{

}

SpriteAnimDefinition const* EntitySpriteAnimStates::GetSpriteAnimDefinition( std::string entityAnimState, Vec2 const& localDirection ) const
{
	auto iter = m_spriteAnimStates.find( entityAnimState );

	if( iter != m_spriteAnimStates.end() )
	{
		SpriteAnimMap* animMap = iter->second;
		return animMap->GetSpriteAnimDefinition( localDirection );
	}
	else
	{
		return nullptr;
	}
}
