#include "../../../Common/DxLibUtil.h"
#include "../../../Manager/SceneManager.h"
#include "Projectile.h"

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

	velocity_.y -= dat_.gravity * scnMng_.GetDeltaTime();

	transform_.position += velocity_;

	transform_.Update();
}

void Projectile::Draw()
{
	if (!IsAlive()) return;

	DrawCapsule3D(Vec3ToVEC(transform_.position), Vec3ToVEC(transform_.position2nd),
		dat_.radius, 8, 0x00ffffu, 0xffffffu, TRUE);
}

void Projectile::InitModel()
{
	colliders_.emplace_back(
		std::make_shared<Collider3D>(
			dat_.type,
			dat_.radius)
	);
}

void Projectile::InitOther()
{
	velocity_ = dat_.velocity;

	hp_ = 1;
}
