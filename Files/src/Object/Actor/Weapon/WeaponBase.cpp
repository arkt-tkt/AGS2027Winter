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

	Vector3 dir = (scnMng_.GetCameraPtr().GetTargetPosition() - transform_.position).Normalized();

	{
		// 0.0 ～ 1.0 の一様実数分布
		auto rand = Application::GetInstance().GetRNGPointer().GetRandReal();

		// 1. 立体角的に均一に分布させるための z (cos(theta)) の計算
		// [cosMaxAngle, 1.0] の範囲から線形に選ぶことで、円錐内に均等に点が散る
		auto cosAngle = std::cos(baseData_.spread);
		float z = cosAngle + (1.0f - cosAngle) * rand;
		float sinTheta = std::sqrt(1.0f - z * z);

		// 2. 方位角 phi を 0 から 2*PI の間でランダムに決定
		float phi = 2.0f * MathUtil::PI_F * rand;
		float x = sinTheta * std::cos(phi);
		float y = sinTheta * std::sin(phi);

		// 3. 基準ベクトル `dir` を中心としたローカル座標系（直交基底 TBN）を作成
		Vector3 up = { 0.0f, 1.0f, 0.0f };
		// 基準ベクトルがワールドの真上・真下を向いている場合のフォールバック
		if (std::abs(Dot(dir, up)) > 0.9999f)
		{
			up = { 0.0f, 0.0f, 1.0f };
		}

		Vector3 tangent = (Cross(up, dir)).Normalized();
		Vector3 bitangent = Cross(dir, tangent);

		// 4. ローカル座標 (x, y, z) をワールド座標系の基準ベクトル向きに変形
		// ローカルの Z軸が dir、X軸が tangent、Y軸が bitangent に対応する
		Vector3 result = {
			tangent.x * x + bitangent.x * y + dir.x * z,
			tangent.y * x + bitangent.y * y + dir.y * z,
			tangent.z * x + bitangent.z * y + dir.z * z
		};

		dir = result.Normalized();
	}

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
