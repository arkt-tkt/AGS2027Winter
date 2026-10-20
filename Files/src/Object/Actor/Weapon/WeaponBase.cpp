#include "../../../Common/DxLibUtil.h"
#include "../../../Common/MathUtil.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "WeaponBase.h"

void WeaponBase::Update()
{
	if (editData_.reloadTime)
	{
		auto dt = SceneManager::GetInstance().GetDeltaTime();

		editData_.reloadTime -= dt;

		if (editData_.reloadTime <= 0.0f)
		{
			editData_.reloadTime = 0.0f;
			editData_.clipSize = baseData_.clipSize;
		}
	}

	if (parent_->modelId != -1)
	{
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

	transform_.Update();
}

void WeaponBase::Draw()
{
	if (transform_.modelId == -1) return;

	transform_.Draw();

	auto basePos = MV1GetFramePosition(parent_->modelId, 35);
	auto vx35 = VTransformSR(VECTOR(40, 0, 0), MV1GetFrameLocalWorldMatrix(parent_->modelId, 35));
	auto vy35 = VTransformSR(VECTOR(0, 40, 0), MV1GetFrameLocalWorldMatrix(parent_->modelId, 35));
	auto vz35 = VTransformSR(VECTOR(0, 0, 40), MV1GetFrameLocalWorldMatrix(parent_->modelId, 35));

	DrawLine3D(basePos, VAdd(basePos, vx35), 0xff0000u);
	DrawLine3D(basePos, VAdd(basePos, vy35), 0xff00u);
	DrawLine3D(basePos, VAdd(basePos, vz35), 0xffu);
}

void WeaponBase::Main()
{
	if (editData_.reloadTime) return;

	editData_.clipSize = MathUtil::Clamp(editData_.clipSize - 1, 0, baseData_.clipSize);
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
	transform_.modelId = resMng_.Load(ResourceManager::SRC::MODEL_WPN_AR_AR1).handleId_;
	transform_.scale = 0.04f;
	transform_.position = 0.0f;
	transform_.localPosition = Position3(-2, -1, 10);

	transform_.Update();
}
