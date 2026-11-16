#pragma once
#include "../ActorBase3D.h"

class ProjectileManager;

class Projectile : public ActorBase3D
{
	friend ProjectileManager;

public:
	struct Data
	{
		Collider3D::TYPE type;
		Position3 position;
		Vector3 velocity;
		float radius = 1.0f;
		float gravity = 0.1f;
		float damage;
	};

	Projectile(ProjectileManager& mng);

	void Update() override;
	void Draw() override;
	void DrawUI() override;

	Data GetData() const { return dat_; }
	void SetData(Data dat) { dat_ = dat; }
	bool IsAlive() const { return hp_ > 0.0f; }
	void Hit() { --hp_; }

private:
	static constexpr float ALIVE_TIME_MAX = 1.5f;

	// マネージャ
	ProjectileManager& projMng_;

	Data dat_;
	int hp_;
	float aliveTimer_ = 0.0f;

	void InitParam() override;
	void InitModel() override;
	void InitAnim() override {}

};
