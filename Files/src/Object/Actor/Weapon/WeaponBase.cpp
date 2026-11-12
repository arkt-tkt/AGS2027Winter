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
#include "WeaponBase.h"

void WeaponBase::Update()
{
	auto dt = SceneManager::GetInstance().GetDeltaTime();

	if (editData_.fireRate < baseData_.fireRate)
	{
		editData_.fireRate += dt;
	}

	if (editData_.reloadTime)
	{
		editData_.reloadTime -= dt;

		if (editData_.reloadTime <= 0.0f)
		{
			editData_.reloadTime = 0.0f;
			
			int ammo = editData_.reserve < baseData_.clipSize ?
				editData_.reserve : baseData_.clipSize;

			int resSub = (std::min)(baseData_.clipSize + 1 - editData_.clipSize, baseData_.clipSize);

			editData_.reserve = (std::max)(editData_.reserve - resSub, 0);

			editData_.clipSize = (std::min)(editData_.clipSize + ammo, baseData_.clipSize + 1);
		}

		return;
	}
}

void WeaponBase::LateUpdate()
{
	TurnVisual();

	transform_.Update();
}

void WeaponBase::Draw()
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

void WeaponBase::DrawUI()
{
	DrawFormatString(100, 720, 0xffffffu, "AM %3d/%3d", editData_.clipSize, editData_.reserve);
}

void WeaponBase::Main()
{
	if (editData_.fireRate < baseData_.fireRate ||
		editData_.reloadTime ||
		editData_.clipSize == 0) return;

	// 弾-1
	--editData_.clipSize;

	// 発射クールタイム
	editData_.fireRate -= baseData_.fireRate;

	Vector3 dir;

	for (int i = 0; i < baseData_.shots; ++i)
	{
		// 基準となる方位を設定
		dir = (scnMng_.GetCameraPtr().GetTargetPosition() - transform_.position).Normalized();

		// 乱数生成器の参照
		auto& rng = Application::GetInstance().GetRNGPointer();

		// ランダム角度
		float angleRand = rng.GetRandReal();
		// ランダム方位角
		float azimuthRand = rng.GetRandReal();

		// 最大拡散度
		float cosMaxAngle = std::cos(baseData_.spread);
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
		dat.damage = editData_.damage;

		if (ProjectileManager::GetInstance().Create(dat))
		{
			AudioManager::GetInstance().Play(ResourceManager::SRC::SE_FIRE_AR);

			EnemyManager::GetInstance().TriggerSoundEvent(transform_.position, EnemyManager::SOUND_EVENT::GUNFIRE);
		}
		else
		{
			// これ以上空きが無いので、続行は無駄
			return;
		}
	}
}

void WeaponBase::Sub()
{
	if (editData_.reloadTime) return;

	// ADS処理
}

void WeaponBase::Alt()
{
}

void WeaponBase::Reload()
{
	if (editData_.clipSize > baseData_.clipSize ||
		editData_.reserve == 0) return;

	if (!editData_.reloadTime)
	{
		editData_.reloadTime = baseData_.reloadTime;
	}
}

void WeaponBase::Refill()
{
	editData_.reserve = baseData_.reserve;
}

float WeaponBase::GetReloadProg() const
{
	if (!editData_.reloadTime)
	{
		return 0.0f;
	}
	else
	{
		return (baseData_.reloadTime - editData_.reloadTime) / baseData_.reloadTime;
	}
}

void WeaponBase::InitParam()
{
	++editData_.clipSize;
	editData_.reloadTime = 0.0f;
}

void WeaponBase::InitModel()
{
	transform_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_WPN_AR_STG556);
	transform_.scale = 0.04f;
	transform_.position = 0.0f;
	transform_.localPosition = Position3(-1, -1, 10);

	transform_.Update();
}

void WeaponBase::TurnVisual()
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
