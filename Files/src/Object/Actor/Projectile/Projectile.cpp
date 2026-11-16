#include "../../../Common/DxLibUtil.h"
#include "../../../Manager/SceneManager.h"
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

void Projectile::DrawUI()
{
	auto drawPos = transform_.position + (transform_.position2nd - transform_.position) / 2.0f;

	auto scPos = VECToVec3(ConvWorldPosToScreenPos(Vec3ToVEC(drawPos)));

	if (scnMng_.GetCameraPtr().IsInCameraBounds(scPos))
	{
		DrawStringToCenterPos(scPos.x, scPos.y, std::to_string(dat_.damage).c_str(), 0xffffffu);
	}
}

void Projectile::InitParam()
{
	transform_.position = transform_.position2nd = dat_.position;
	velocity_ = dat_.velocity;

	hp_ = 1;
	aliveTimer_ = 0.0f;
}

void Projectile::InitModel()
{
	colliders_.emplace_back(
		std::make_shared<Collider3D>(
			dat_.type,
			dat_.radius)
	);
}
