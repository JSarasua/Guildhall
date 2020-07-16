#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Game/EntityDefinition.hpp"

class Map;

class Portal : public Entity
{
public:
	Portal( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& pitchRollYawDegrees, std::string const& destMap, Vec2 const& destPos, float destYawOffset, Map* parentMap );

	virtual void Startup() override {}
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	std::string const& GetDestMap() const { return m_destMap; }
	Vec2 const& GetDestPosition() const { return m_destPos; }
	float GetDestYawOffset() const { return m_destYawOffset; }

private:
	std::vector<Vec3> GetBillboardedVertsCounterClockwise( std::string const& billboardType ) const;
	Vec2	GetLocalDirectionToMainCamera() const;

private:
	std::string m_destMap;
	Vec2 m_destPos;
	float m_destYawOffset = 0.f;
	Map* m_mapPortalIsIn = nullptr;
};