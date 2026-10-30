#include "../../../Common/DxLibUtil.h"
#include "../../../Common/MathUtil.h"
#include "../../../Manager/ProjectileManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
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
			editData_.clipSize = baseData_.clipSize;
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
	DrawFormatString(20, 720, 0xffffffu, "AM %3d", editData_.clipSize);
}

void WeaponBase::Main()
{
	if (editData_.fireRate < baseData_.fireRate ||
		editData_.reloadTime) return;

	if (--editData_.clipSize == 0)
	{
		Reload();
	}

	editData_.fireRate -= baseData_.fireRate;

	Projectile::Data dat;
	dat.type = Collider3D::TYPE::PROJECTILE_PLAYER;
	dat.position = transform_.position;
	dat.velocity = (scnMng_.GetCameraPtr().GetTargetPosition() - transform_.position).Normalized() * baseData_.velocity;
	dat.damage = editData_.damage;

	ProjectileManager::GetInstance().Create(dat);
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
	if (!editData_.reloadTime)
	{
		editData_.reloadTime = baseData_.reloadTime;
	}
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

void WeaponBase::InitModel()
{
	transform_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_WPN_AR_AR1);
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
