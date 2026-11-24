#pragma once
#include <list>
#include "../ActorBase3D.h"

class CharacterBase3D;
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
		float damage;
		int penetration = 0;
		float radius = 1.0f;
		float gravity = 0.1f;
	};

	Projectile(ProjectileManager& mng);

	void Update() override;
	void Draw() override;

	Data GetData() const { return dat_; }
	void SetData(Data dat) { dat_ = dat; }
	bool IsAlive() const { return hp_ > 0; }
	bool IsCanHit(CharacterBase3D* chara);
	void Hit(CharacterBase3D* chara);
	void HitEnv();

private:
	static constexpr float ALIVE_TIME_MAX = 1.5f;

	// マネージャ
	ProjectileManager& projMng_;

	Data dat_;
	int hp_;
	float aliveTimer_ = 0.0f;

	std::list<CharacterBase3D*> hitAways_;

	void InitParam() override;
	void InitModel() override;
	void InitAnim() override {}

};
