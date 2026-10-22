#pragma once
#include "../ActorBase3D.h"

class Projectile : public ActorBase3D
{
public:
	struct Data
	{
		Collider3D::TYPE type;
		Position3 position;
		Vector3 velocity;
		float radius = 1.0f;
		float gravity = 0.1f;
		float damage = 10.0f;
	};

	void Update() override;
	void Draw() override;

	void SetData(Data dat) { dat_ = dat; }
	bool IsAlive() const { return hp_ > 0.0f; }

private:
	static constexpr float ALIVE_TIME_MAX = 1.5f;

	Data dat_;
	int hp_;
	float aliveTimer_ = 0.0f;

	void InitModel() override;
	void InitAnim() override {}
	void InitOther() override;

};
