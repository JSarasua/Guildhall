#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <string>

class SpriteSheet;
class SpriteAnimDefinition;

//Contains front, left, right, etc for a "Walk"
struct SpriteAnimMap
{
public:
	SpriteAnimMap( XmlElement const& entityAnimStateElem, SpriteSheet* spriteSheet );
	~SpriteAnimMap();

	SpriteAnimDefinition const* GetSpriteAnimDefinition( Vec2 const& localDirection ) const;
	void AddSpriteIndexesToMapIfValid( std::string const& direction, std::string const& indexString );
	std::vector<int> GetSpriteIndexes( std::string const& indexString );
private:
	SpriteSheet* m_spriteSheet = nullptr;
	std::map< std::string, SpriteAnimDefinition* > m_spriteanims;
};

//Contains Walk, Death, etc
struct EntitySpriteAnimStates
{
public:
	EntitySpriteAnimStates( XmlElement const& entityAppearanceElem );
	~EntitySpriteAnimStates();

	SpriteAnimDefinition const* GetSpriteAnimDefinition( std::string entityAnimState, Vec2 const& localDirection ) const;

private:
	std::map< std::string, SpriteAnimMap* > m_spriteAnimStates;
};



class EntityDefinition
{
public:
	EntityDefinition() = delete;
	EntityDefinition( XmlElement const& element );
	~EntityDefinition() {}

	static void InitializeEntityDefinitions( XmlElement const& rootEntityDefinitionElement );
	static EntityDefinition const* GetEntityDefinitionByName( std::string const& entityName );
	static std::map< std::string, EntityDefinition* > s_definitions;

	float GetEyeHeight() const;
	float GetHeight() const;
	float GetMass() const;
	float GetPhysicsRadius() const;
	float GetWalkSpeed() const;
	bool IsPushedByWalls() const;
	bool IsPushedByEntities() const;
	bool CanPushEntities() const;

	std::string const& GetName() const;
	std::string const& GetType() const;

	Vec2 const& GetDrawSize() const { return m_drawSize; };
	std::string const& GetBillboardType() const { return m_billBoardType; }
	EntitySpriteAnimStates const* GetSpriteAnimStates() const { return m_spriteAnimStates; }

protected:
	std::string m_name;
	std::string m_type;
	bool m_isValid = false;

	bool m_canBePushedByWalls = false;
	bool m_canBePushedByEntities = false;
	bool m_canPushEntities = false;
	float m_mass = 1.f;
	float m_height = 0.f;
	float m_eyeHeight = 0.f;
	float m_physicsRadius = 0.f;
	float m_walkSpeed = 0.f;
	
	Vec2 m_drawSize;
	std::string m_billBoardType;

	EntitySpriteAnimStates* m_spriteAnimStates = nullptr;

};