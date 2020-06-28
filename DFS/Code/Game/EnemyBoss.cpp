#include "Game/Entity.hpp"
#include "Engine/Time/Timer.hpp"

class EnemyBoss : public Entity
{
	friend class Map;
public:
	EnemyBoss() = delete;
	~EnemyBoss();

	virtual void Startup() override;
	virtual void Update( float deltaSeconds );
	virtual void Render() const override;

protected:
	Timer m_actionTimer;
};



//Circle of bullets
//Random fire
//laser beam rotating