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

	if (shotTimer_ > 0.0f)
	{
		shotTimer_ -= dt;
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
			
			int ammo = ammoReserve_ < baseData_.clipSize ?
				ammoReserve_ : baseData_.clipSize;

			int maxClip = baseData_.chambering ?
				baseData_.clipSize + baseData_.chambering : baseData_.clipSize;

			int resSub = (std::min)(maxClip - ammoClip_, baseData_.clipSize);

			ammoReserve_ = (std::max)(ammoReserve_ - resSub, 0);

			ammoClip_ = (std::min)(ammoClip_ + ammo, maxClip);
		}
	}

	Collision();
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
	// 銃口
	auto muzzlePos = transform_.position + Quaternion().Euler(transform_.rotation) * baseData_.muzzle;
	DrawSphere3D(Vec3ToVEC(muzzlePos), 0.5f, 8, 0xff00ffu, 0xffffffu, FALSE);
	DrawSphere3D(Vec3ToVEC(muzzlePos), 5.0f, 4, 0xff00ffu, 0xffffffu, FALSE);

	// 射線
	DrawLine3D(Vec3ToVEC(muzzlePos), Vec3ToVEC(scnMng_.GetCameraPtr().GetTargetPosition()), 0xff00ffu);

	// オフセット位置
	auto lPos = VScale(Vec3ToVEC(HAND_OFFSET), 0.4f);
	auto oPos = MV1GetFramePosition(parent_->modelId, 35);
	auto lwMat = MV1GetFrameLocalWorldMatrix(parent_->modelId, 35);

	auto pos = VTransformSR(lPos, lwMat);
	auto vx = VTransformSR(VAdd(lPos, VECTOR(40, 0, 0)), lwMat);
	auto vy = VTransformSR(VAdd(lPos, VECTOR(0, 40, 0)), lwMat);
	auto vz = VTransformSR(VAdd(lPos, VECTOR(0, 0, 40)), lwMat);

	DrawLine3D(VAdd(oPos, pos), VAdd(oPos, vx), 0xff0000u);
	DrawLine3D(VAdd(oPos, pos), VAdd(oPos, vy), 0xff00u);
	DrawLine3D(VAdd(oPos, pos), VAdd(oPos, vz), 0xffu);
	DrawSphere3D(VAdd(oPos, pos), 1.5f, 8, 0xffff00u, 0xffffffu, TRUE);
#endif
}

bool ItemBase::Main(int pad_num, InputManager::TAGS tag)
{
	const auto& ins = InputManager::GetInstance();

	bool ret = false;

	if (ins.CheckNowMap(pad_num, tag)) ret = true;

	if (shotTimer_ > 0.0f) return ret;

	if (reloadProg_ || ammoClip_ == 0 || muzzleLock_)
	{
		fireCount_ = fireCountTarget_ = -1;
		return ret;
	}

	if (fireCount_ < fireCountTarget_)
	{
		ret = true;

		if (pad_num != -1) return ret;

		++fireCount_;
	}
	else if (pad_num > 0)
	{
		int fireRound = baseData_.fireMode[firemode_];

		if (fireRound <= -1 && !ins.CheckNowMap(pad_num, tag) ||
			fireRound > 0 && !ins.CheckDownMap(pad_num, tag))
		{
			return ret;
		}

		ret = true;

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
	shotTimer_ += _FireRate();

	// 反動
	Vector3 recShake = {
		baseData_.recoilShake.x * rng.GetRandReal(-1.0f, 1.0f),
		baseData_.recoilShake.y * rng.GetRandReal(-1.0f, 1.0f),
		baseData_.recoilShake.z * rng.GetRandReal(-1.0f, 1.0f)
	};
	scnMng_.GetCameraPtr().Kick(baseData_.recoilKick + recShake);

	// 銃口の位置を取得しておく
	auto muzzlePos = transform_.position + Quaternion().Euler(transform_.rotation) * baseData_.muzzle;

	Vector3 dir;

	for (int i = 0; i < baseData_.shots; ++i)
	{
		// 基準となる方位を設定
		dir = (scnMng_.GetCameraPtr().GetTargetPosition() - muzzlePos).Normalized();

		// ランダム角度
		float angleRand = rng.GetRandReal();
		// ランダム方位角
		float azimuthRand = rng.GetRandReal();

		// 最大拡散度
		float cosMaxAngle = std::cos(baseData_.dispersionRad);
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
		dat.position = muzzlePos;
		dat.velocity = dir * baseData_.velocity;
		dat.damage = baseData_.damage;
		dat.penetration = baseData_.penetration;

		if (ProjectileManager::GetInstance().CreateProjectile(dat))
		{
			AudioManager::GetInstance().Play(ResourceManager::SRC::SE_FIRE_AR);

			EnemyManager::GetInstance().TriggerSoundEvent(transform_.position, EnemyManager::SOUND_EVENT::GUNFIRE);
		}
		else
		{
			// これ以上空きが無いので、続行は無駄
			break;
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
	if (ammoClip_ >= baseData_.clipSize + baseData_.chambering ||
		ammoReserve_ == 0) return;

	if (!reloadProg_)
	{
		reloadProg_ = baseData_.reloadTime;
	}
}

void ItemBase::CycleFiremode()
{
	if (fireCount_ < fireCountTarget_ ||
		shotTimer_ > 0.0f) return;

	firemode_ = (firemode_ + 1) % (unsigned int)baseData_.fireMode.size();

	_SetFiremodeLabel();

	AudioManager::GetInstance().Play(ResourceManager::SRC::SE_SWITCH_FIREMODE);
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
		return (baseData_.reloadTime - reloadProg_) / baseData_.reloadTime;
	}
}

void ItemBase::InitParam()
{
	ammoClip_ = baseData_.clipSize +
		baseData_.chambering;

	ammoReserve_ = baseData_.reserve;

	_SetFiremodeLabel();
}

void ItemBase::InitModel()
{
	transform_.modelId = resMng_.LoadModelDuplicate(
		static_cast<ResourceManager::SRC>(
			static_cast<int>(ResourceManager::SRC::MODEL_WPN_AR_SG553) + baseData_.model));
	transform_.scale = baseData_.scale;
	transform_.position = 0.0f;
	transform_.localPosition = baseData_.offset;

	transform_.Update();
}

void ItemBase::Collision()
{
	muzzleLock_ = false;

	for (const auto& c : awayColliders_)
	{
		auto& dat = c.lock()->GetColliderData();

		if (dat.type != Collider3D::TYPE::STAGE || dat.modelId == -1) continue;

		// 銃口
		auto muzzlePos = transform_.position + Quaternion().Euler(transform_.rotation) * baseData_.muzzle;

		// 手
		auto lPos = VScale(Vec3ToVEC(HAND_OFFSET), 0.4f);
		auto oPos = MV1GetFramePosition(parent_->modelId, 35);
		auto lwMat = MV1GetFrameLocalWorldMatrix(parent_->modelId, 35);
		auto hPos = VAdd(oPos, VTransformSR(lPos, lwMat));

		auto hits = MV1CollCheck_Capsule(
			dat.modelId, -1,
			Vec3ToVEC(muzzlePos),
			hPos,
			2.0f);

		if (hits.HitNum)
		{
			muzzleLock_ = true;
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}

void ItemBase::TurnVisual()
{
	if (parent_->modelId == -1) return;

	using namespace DxLibUtil;

	// 一般的なmixamorigの35番は右手首
	auto lPos = VScale(Vec3ToVEC(HAND_OFFSET), 0.4f);
	auto oPos = MV1GetFramePosition(parent_->modelId, 35);
	auto lwMat = MV1GetFrameLocalWorldMatrix(parent_->modelId, 35);
	auto pos = VTransformSR(lPos, lwMat);
	transform_.position = VECToVec3(VAdd(oPos, pos));

	auto basePos = VECToVec3(MV1GetFramePosition(parent_->modelId, 35));
	Vector3 forward;

	if (reloadProg_)
	{
		// ※行列はジンバルロックか何かを起こしてそうだったので、
		// 多少強引だがLookRotation関数で銃口の方向を向かせる
		auto vec = VECTOR(0, 1, 0);
		vec = VTransformSR(vec, lwMat);

		forward = (basePos + VECToVec3(vec) - basePos).Normalized();
	}
	else
	{
		// 右手から注視点へのベクトルを取り、
		// その方向に銃口を向ける
		auto& lookPos = scnMng_.GetCameraPtr().GetTargetPosition();

		forward = (lookPos - basePos).Normalized();
	}

	Quaternion qRot = LookRotation(forward);
	transform_.rotation = qRot.ToEuler();
}

void ItemBase::_SetFiremodeLabel()
{
	if (baseData_.fireMode[firemode_] == -1)
	{
		firemodeLabel_ = FIREMODE_LABEL_AUTO;
	}
	else if (baseData_.fireMode[firemode_] == -2)
	{
		firemodeLabel_ = FIREMODE_LABEL_AUTO_ALT;
	}
	else if (baseData_.fireMode[firemode_] == 1)
	{
		firemodeLabel_ = FIREMODE_LABEL_SEMI;
	}
	else
	{
		firemodeLabel_ = std::to_string(baseData_.fireMode[firemode_]) + FIREMODE_LABEL_BURST;
	}
}

float ItemBase::_FireRate()
{
	if (!MathUtil::Within(baseData_.fireMode[firemode_], -1, 1) &&
		baseData_.fireRate.size() == 2)
	{
		return baseData_.fireRate[1];
	}

	return baseData_.fireRate[0];
}

Vector3 ItemBase::GetEuler(const Matrix4x4& m)
{
	return {
		   std::asin(MathUtil::Clamp(m.m[2][1], -1.0f, 1.0f)),
		   -std::atan2(m.m[2][0], m.m[2][2]),
		   -std::atan2(m.m[0][1], m.m[1][1])
	};
}
