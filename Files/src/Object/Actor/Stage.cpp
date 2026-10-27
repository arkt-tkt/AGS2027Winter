#include <DxLib.h>
#include <cstdio>
#include "../../Common/DxLibUtil.h"
#include "../../Common/Geometry.h"
#include "../../Common/MathUtil.h"
#include "../../Manager/ProjectileManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "Stage.h"

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Update()
{
	auto qRot = AngleAxis(Vector3(0, 1, 0), 0.0001);
	skyDome_.rotation += qRot.ToEuler();
	skyDome_.rotation.y = MathUtil::RadIn2PI(skyDome_.rotation.y);

	skyDome_.Update();

	transform_.Update();

	Collision();
}

void Stage::Draw()
{
	skyDome_.Draw();

	transform_.Draw();
}

void Stage::InitParam()
{
	transform_.position = 0.0f;
	transform_.localPosition = Position3(0.0f, 800.0f, 0.0f);
	transform_.scale = 0.5f;
}

void Stage::InitModel()
{
	// ステージ
	transform_.modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_STAGE);

	for (const int& IDX : INVIS_FRAME_ARR)
	{
		MV1SetFrameVisible(transform_.modelId, IDX, FALSE);
	}

	int frames = MV1GetFrameNum(transform_.modelId);
	for (int i = 0; i < frames; ++i)
	{
		COLOR_F emi = MV1GetMaterialDifColor(transform_.modelId, i);
		emi.r *= 0.25f;
		emi.g *= 0.25f;
		emi.b *= 0.25f;
		MV1SetMaterialEmiColor(transform_.modelId, i, emi);
	}

	// コライダ
	colliders_.emplace_back(std::make_shared<Collider3D>(Collider3D::TYPE::STAGE, transform_.modelId));

	// スカイドーム
	skyDome_.modelId = ResourceManager::GetInstance().Load(ResourceManager::SRC::MODEL_SKYDOME).handleId_;
	skyDome_.position = 0.0f;
	skyDome_.localPosition = Position3(0.0f, 0.0f, 0.0f);
	skyDome_.scale = 50.0f;
	MV1SetMaterialEmiColor(skyDome_.modelId, 0, COLOR_F(0.2f, 0.2f, 0.2f, 1.0f));

	// 一度だけ更新
	skyDome_.Update();
}

void Stage::InitAnim()
{
}

void Stage::Collision()
{
	using C_TY = Collider3D::TYPE;

	auto cols = GetTypedWeakColliders(C_TY::STAGE);

	for (auto& col : cols)
	{
		auto colPtr = col.lock();
		auto& projs = ProjectileManager::GetInstance().GetActives();

		for (auto& proj : projs)
		{
			C_TY cTyArr[] = { C_TY::PROJECTILE_PLAYER, C_TY::PROJECTILE_ENEMY };

			for (int i = 0; i < _countof(cTyArr); ++i)
			{
				auto projCols = proj->GetTypedWeakColliders(cTyArr[i]);

				for (auto& projCol : projCols)
				{
					auto hits = MV1CollCheck_Capsule(
						colPtr->GetColliderData().modelId, -1,
						Vec3ToVEC(projCol.lock()->GetColliderData().pos1),
						Vec3ToVEC(projCol.lock()->GetColliderData().pos2),
						projCol.lock()->GetColliderData().r);

					if (hits.HitNum)
					{
						proj->Hit();
					}

					// 検出した地面ポリゴン情報の後始末
					MV1CollResultPolyDimTerminate(hits);
				}
			}
		}
	}
}
