#include <EffekseerForDXLib.h>
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
	if (hp_ <= 0)
	{
		--effectTimer_;
		return;
	}

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

	SetPosPlayingEffekseer3DEffect(effectPId_,
		transform_.position.x, transform_.position.y, transform_.position.z);

	UpdateEffekseer3D();

	UpdateColliders();
}

void Projectile::Draw()
{
	//DrawEffekseer3D_Draw(effectPId_);

#ifdef _DEBUG
	if (hp_ <= 0) return;

	DrawCapsule3D(Vec3ToVEC(transform_.position), Vec3ToVEC(transform_.position2nd),
		dat_.radius, 4, 0x00ffffu, 0xffffffu, FALSE);
#endif
}

void Projectile::Release()
{
	hitAways_.clear();
	StopEffekseer3DEffect(effectPId_);

	ActorBase3D::Release();
}

bool Projectile::IsCanHit(CharacterBase3D* chara) const
{
	if (hp_ <= 0) return false;
	if (!chara) return false;
	if (std::find(hitAways_.begin(), hitAways_.end(), chara) != hitAways_.end()) return false;

	return true;
}

bool Projectile::IsCanDest() const
{
	return effectTimer_ <= 0;
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
	effectTimer_ = EFFECT_DRAW_FRAME_MAX;
}

void Projectile::InitModel()
{
	effectPId_ = PlayEffekseer3DEffect(
		resMng_.Load(ResourceManager::SRC::EFFEKSEER_BULLET).handleId_);

	SetPosPlayingEffekseer3DEffect(effectPId_,
		transform_.position.x, transform_.position.y, transform_.position.z);
	
	UpdateEffekseer3D();

	colliders_.emplace_back(
		std::make_shared<Collider3D>(
			dat_.type,
			dat_.radius)
	);
}
