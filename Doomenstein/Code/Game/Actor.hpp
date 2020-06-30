#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Game/EntityDefinition.hpp"

class Actor : public Entity
{
public:
	Actor( EntityDefinition const* entityDef, Vec2 const& initialPosition, Vec3 const& pitchRollYawDegrees );

	virtual void Startup() override {}
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

private:
	std::vector<Vec3> GetBillboardedVertsCounterClockwise( std::string const& billboardType ) const;
};