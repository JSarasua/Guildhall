#include "Game/Loot.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/AABB2.hpp"

Loot::Loot( Vec2 const& initialPosition, WeaponDefinition const* lootWeapon ) :
	Entity( initialPosition, Vec2(), 0.f, 0.f )
{
	m_entityType = ENTITY_TYPE_LOOT;
	m_lootWeapon = lootWeapon;
	AABB2 weaponDrawBounds = m_lootWeapon->GetWeaponDrawBounds();
	weaponDrawBounds.GetInnerRadius();
	m_physicsRadius = 	weaponDrawBounds.GetInnerRadius();
}

WeaponDefinition const* Loot::GetLootWeapon() const
{
	return m_lootWeapon;
}

void Loot::Render() const
{
	AABB2 weaponUVs;
	m_lootWeapon->GetWeaponSpriteDef()->GetUVs( weaponUVs.mins, weaponUVs.maxs );
	Texture const& weaponTexture = m_lootWeapon->GetWeaponSpriteDef()->GetTexture();
	Rgba8 const& weaponTint = m_lootWeapon->GetWeaponTint();
	AABB2 weaponBounds = m_lootWeapon->GetWeaponDrawBounds();
	weaponBounds.Translate( m_position );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->BindTexture( &weaponTexture );
	g_theRenderer->DrawRotatedAABB2Filled( weaponBounds, weaponTint, weaponUVs.mins, weaponUVs.maxs, 0.f );


}

