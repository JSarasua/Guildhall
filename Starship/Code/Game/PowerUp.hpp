#pragma once
#include "Entity.hpp"

class PowerUp : public Entity
{
public:
	PowerUp() {}
	PowerUp( Game* game );
	~PowerUp() {}

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	void ActivatePowerUp( Vec2 initialPosition, int powerUp);
	int GetPowerUp();
private:
	int m_WhichPowerUp = 0;

};