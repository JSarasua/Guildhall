#pragma once
#include "Entity.hpp"
#include "Engine/Core/Rgba8.hpp"


class Debris: public Entity
{
public:
	Debris() {}
	Debris(Rgba8 color, Vec2 initialVelocity, Game* game);
	Debris(Game* game);
	~Debris() {}



	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	void ActivateDebris( Rgba8 color, Vec2 initialVelocity, Vec2 initialPosition );


private:
	Rgba8 m_color;
	
};