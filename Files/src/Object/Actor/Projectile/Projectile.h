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
	void Release() override;

	Data GetData() const { return dat_; }
	void SetData(Data dat) { dat_ = dat; }
	bool IsCanHit(CharacterBase3D* chara) const;
	bool IsCanDest() const;
	void Hit(CharacterBase3D* chara);
	void HitEnv();

private:
	static constexpr float ALIVE_TIME_MAX = 1.5f;
	static constexpr int EFFECT_DRAW_FRAME_MAX = 10;

	// マネージャ
	ProjectileManager& projMng_;

	Data dat_;
	int effectPId_ = -1;
	int hp_ = 0;
	float aliveTimer_ = 0.0f;
	int effectTimer_ = 0;

	std::list<CharacterBase3D*> hitAways_;

	void InitParam() override;
	void InitModel() override;
	void InitAnim() override {}

};
