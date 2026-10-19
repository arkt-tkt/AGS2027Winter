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

		// mixamorigÇÃ35î‘ÇÕâEéËéÒ
		transform_.position = VECToVec3(
			MV1GetFramePosition(parent_->modelId, 35));

		transform_.rotation =
			RotationMatrixY(parent_->rotation.y) *
			TransformSR(MATToMat4x4(MV1GetFrameLocalWorldMatrix(parent_->modelId, 35)), Vector3(0, 1, 0));
	}

	transform_.Update();
}

void WeaponBase::Draw()
{
	if (transform_.modelId == -1) return;

	transform_.Draw();
}

void WeaponBase::Main()
{
	if (editData_.reloadTime) return;

	editData_.clipSize = MathUtil::Clamp(editData_.clipSize - 1, 0, baseData_.clipSize);
}

void WeaponBase::Sub()
{
	if (editData_.reloadTime) return;

	// ADSèàóù
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
	transform_.scale = 0.1f;
	transform_.position = 0.0f;
	transform_.localPosition = Position3(0, 3, 0);
	transform_.localRotation.y = -DX_PI_F / 2.0f;

	transform_.Update();
}
