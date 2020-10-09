#pragma once
#include "Game/Entity.hpp"

class WeaponDefinition;

class Loot : public Entity
{
friend class Map;
public:
	Loot( Vec2 const& initialPosition, WeaponDefinition const* lootWeapon );
	~Loot() {}

	virtual void Startup() override {}
	virtual void Render() const override;
	
	WeaponDefinition const* GetLootWeapon() const;
public:
	WeaponDefinition const* m_lootWeapon = nullptr;
};