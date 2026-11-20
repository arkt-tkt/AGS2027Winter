#include "../../../Common/DxLibUtil.h"
#include "../../../Common/MathUtil.h"
#include "../../../Manager/AudioManager.h"
#include "../../../Manager/EnemyManager.h"
#include "../../../Manager/ProjectileManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Utility/RNG.h"
#include "../../../Application.h"
#include "../Projectile/Projectile.h"
#include "ItemBase.h"

void ItemBase::Update()
{
	auto dt = SceneManager::GetInstance().GetDeltaTime();

	if (shotTimer_ < baseData_.fireRate)
	{
		shotTimer_ += dt;
	}

	if (fireCount_ > 0 && fireCount_ < fireCountTarget_)
	{
		Main(-1, InputManager::TAGS::NONE);

		if (fireCount_ >= fireCountTarget_)
		{
			fireCount_ = fireCountTarget_ = -1;
		}
	}

	if (reloadProg_)
	{
		reloadProg_ -= dt;

		if (reloadProg_ <= 0.0f)
		{
			reloadProg_ = 0.0f;
			
			int ammo = ammoReserve_ < baseData_.clipsize ?
				ammoReserve_ : baseData_.clipsize;

			int resSub = (std::min)(baseData_.clipsize + 1 - ammoClip_, baseData_.clipsize);

			ammoReserve_ = (std::max)(ammoReserve_ - resSub, 0);

			ammoClip_ = (std::min)(ammoClip_ + ammo, baseData_.clipsize + 1);
		}

		return;
	}
}

void ItemBase::LateUpdate()
{
	TurnVisual();

	transform_.Update();
}

void ItemBase::Draw()
{
	if (transform_.modelId == -1) return;

	transform_.Draw();

#ifdef _DEBUG
	DrawLine3D(Vec3ToVEC(transform_.position), Vec3ToVEC(scnMng_.GetCameraPtr().GetTargetPosition()), 0xff0000u);

	auto basePos = MV1GetFramePosition(parent_->modelId, 35);
	auto vx35 = VTransformSR(VECTOR(40, 0, 0), MV1GetFrameLocalWorldMatrix(parent_->modelId, 35));
	auto vy35 = VTransformSR(VECTOR(0, 40, 0), MV1GetFrameLocalWorldMatrix(parent_->modelId, 35));
	auto vz35 = VTransformSR(VECTOR(0, 0, 40), MV1GetFrameLocalWorldMatrix(parent_->modelId, 35));

	DrawLine3D(basePos, VAdd(basePos, vx35), 0xff0000u);
	DrawLine3D(basePos, VAdd(basePos, vy35), 0xff00u);
	DrawLine3D(basePos, VAdd(basePos, vz35), 0xffu);
#endif
}

void ItemBase::DrawUI()
{
	DrawFormatString(100, 720, 0xffffffu, "AM %3d/%3d", ammoClip_, ammoReserve_);
}

bool ItemBase::Main(int pad_num, InputManager::TAGS tag)
{
	bool ret = false;

	if (shotTimer_ < baseData_.fireRate) return ret;

	if (reloadProg_ || ammoClip_ == 0)
	{
		fireCount_ = fireCountTarget_ = -1;
		return ret;
	}

	if (fireCount_ < fireCountTarget_)
	{
		if (pad_num != -1) return ret;

		++fireCount_;
	}
	else if (pad_num > 0)
	{
		const auto& ins = InputManager::GetInstance();

		int fireRound = baseData_.firemode[firemode_];

		if (fireRound == -1 && !ins.CheckNowMap(pad_num, tag) ||
			fireRound > 0 && !ins.CheckDownMap(pad_num, tag))
		{
			return ret;
		}

		fireCountTarget_ = fireRound;
		fireCount_ = fireRound > 0 ? 1 : -1;
	}
	else
	{
		return ret;
	}

	// 乱数生成器の参照
	auto& rng = Application::GetInstance().GetRNGPointer();

	// 弾-1
	--ammoClip_;

	// 発射クールタイム
	shotTimer_ -= baseData_.fireRate;

	// 反動
	scnMng_.GetCameraPtr().Kick(
		Vector3(-MathUtil::PI_F * 0.012f, MathUtil::PI_F * rng.GetRandReal(-0.003f, 0.003f), 0));

	Vector3 dir;

	for (int i = 0; i < baseData_.shots; ++i)
	{
		// 基準となる方位を設定
		dir = (scnMng_.GetCameraPtr().GetTargetPosition() - transform_.position).Normalized();

		// ランダム角度
		float angleRand = rng.GetRandReal();
		// ランダム方位角
		float azimuthRand = rng.GetRandReal();

		// 最大拡散度
		float cosMaxAngle = std::cos(baseData_.dispersion);
		// ローカルZ軸の移動量
		float z = 1.0f - (1.0f - cosMaxAngle) * angleRand;

		// ねじれ角がゼロの時の、Z軸を除いた移動量
		float sinTheta = std::sqrt(1.0f - z * z);
		// ねじれ角()
		float phi = azimuthRand * MathUtil::PI_F * 2.0f;

		// sinThetaをcos/sinで処理し、ローカルX/Y軸の移動量を出す
		Vector3 localDir =
		{
			sinTheta * std::cos(phi),
			sinTheta * std::sin(phi),
			z
		};

		// 最後に、現在の向きへの回転を用いて補正
		dir = (LookRotation(dir) * localDir).Normalized();

		Projectile::Data dat;
		dat.type = Collider3D::TYPE::PROJECTILE_PLAYER;
		dat.position = transform_.position;
		dat.velocity = dir * baseData_.velocity;
		dat.damage = baseData_.damage;

		if (ProjectileManager::GetInstance().CreateProjectile(dat))
		{
			AudioManager::GetInstance().Play(ResourceManager::SRC::SE_FIRE_AR);

			EnemyManager::GetInstance().TriggerSoundEvent(transform_.position, EnemyManager::SOUND_EVENT::GUNFIRE);

			ret = true;
		}
		else
		{
			// これ以上空きが無いので、続行は無駄
			return ret;
		}
	}

	return ret;
}

void ItemBase::Sub()
{
	if (reloadProg_) return;

	// ADS処理
}

void ItemBase::Alt()
{
}

void ItemBase::Reload()
{
	if (ammoClip_ > baseData_.clipsize ||
		ammoReserve_ == 0) return;

	if (!reloadProg_)
	{
		reloadProg_ = baseData_.reload;
	}
}

void ItemBase::CycleFiremode()
{
	firemode_ = (firemode_ + 1) % (unsigned int)baseData_.firemode.size();
}

void ItemBase::Refill()
{
	ammoReserve_ = baseData_.reserve;
}

float ItemBase::GetReloadProg() const
{
	if (!reloadProg_)
	{
		return 0.0f;
	}
	else
	{
		return (baseData_.reload - reloadProg_) / baseData_.reload;
	}
}

void ItemBase::InitParam()
{
	ammoClip_ = baseData_.clipsize + 1;
	ammoReserve_ = baseData_.reserve;
	shotTimer_ = baseData_.fireRate;
}

void ItemBase::InitModel()
{
	transform_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_WPN_AR_STG556);
	transform_.scale = 0.04f;
	transform_.position = 0.0f;
	transform_.localPosition = Position3(-1, -1, 10);

	transform_.Update();
}

void ItemBase::TurnVisual()
{
	if (parent_->modelId == -1) return;

	using namespace DxLibUtil;

	// mixamorigの35番は右手首、40番は右手人差し指の付け根
	auto rhPos = VECToVec3(MV1GetFramePosition(parent_->modelId, 35));
	auto rhIndexPos = VECToVec3(MV1GetFramePosition(parent_->modelId, 40));
	transform_.position = rhPos + (rhIndexPos - rhPos) / 2.0f;

	// 右手から注視点へのベクトルを取り、
	// その方向に銃口を向ける
	auto basePos = VECToVec3(MV1GetFramePosition(parent_->modelId, 35));
	auto& lookPos = scnMng_.GetCameraPtr().GetTargetPosition();

	auto forward = (lookPos - basePos).Normalized();

	auto qRot = LookRotation(forward);
	transform_.rotation = qRot.ToEuler();
}
