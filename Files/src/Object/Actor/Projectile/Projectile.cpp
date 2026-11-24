#include "../../../Common/DxLibUtil.h"
#include "../../../Manager/SceneManager.h"
#include "../Character/CharacterBase3D.h"
#include "Projectile.h"

Projectile::Projectile(ProjectileManager& mng)
	:
	projMng_(mng),
	ActorBase3D()
{
}

void Projectile::Update()
{
	if (!IsAlive()) return;

	const auto& dt = scnMng_.GetDeltaTime();

	aliveTimer_ += dt;

	if (aliveTimer_ >= ALIVE_TIME_MAX)
	{
		hp_ = 0;
		return;
	}

	transform_.position2nd = transform_.position;

	velocity_.y -= dat_.gravity;

	transform_.position += velocity_ * dt;

	transform_.Update();
	UpdateColliders();
}

void Projectile::Draw()
{
	if (!IsAlive()) return;

	DrawCapsule3D(Vec3ToVEC(transform_.position), Vec3ToVEC(transform_.position2nd),
		dat_.radius, 16, 0x00ffffu, 0xffffffu, TRUE);
}

bool Projectile::IsCanHit(CharacterBase3D* chara)
{
	if (!chara) return false;
	if (std::find(hitAways_.begin(), hitAways_.end(), chara) != hitAways_.end()) return false;

	return true;
}

void Projectile::Hit(CharacterBase3D* chara)
{
	hitAways_.push_back(chara);
	hp_--;
}

void Projectile::HitEnv()
{
	hp_ = 0;
}

void Projectile::InitParam()
{
	transform_.position = transform_.position2nd = dat_.position;
	velocity_ = dat_.velocity;

	hp_ = dat_.penetration + 1;
	aliveTimer_ = 0.0f;

	hitAways_.clear();
}

void Projectile::InitModel()
{
	colliders_.emplace_back(
		std::make_shared<Collider3D>(
			dat_.type,
			dat_.radius)
	);
}
