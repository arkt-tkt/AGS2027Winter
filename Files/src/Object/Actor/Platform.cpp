#include <DxLib.h>
#include <cstdio>
#include "../../Common/DxLibUtil.h"
#include "../../Common/Geometry.h"
#include "../../Common/MathUtil.h"
#include "../../Manager/ProjectileManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "Platform.h"

Platform::Platform()
{
}

Platform::~Platform()
{
}

void Platform::Update()
{
	auto qRot = AngleAxis(Vector3(0, 1, 0), 0.0001);
	skyDome_.rotation += qRot.ToEuler();
	skyDome_.rotation.y = MathUtil::RadIn2PI(skyDome_.rotation.y);

	skyDome_.Update();

	transform_.Update();

	Collision();
}

void Platform::Draw()
{
	skyDome_.Draw();

	transform_.Draw();

#ifdef _DEBUG
	const Vector3 PATHFIND_NODE_LOC[] =
	{
		// 道路
		{ -700, 0, 1420 },
		{ -700, 0, 980 },
		{ -700, 0, 670 },
		{ -700, 0, 70 },
		{ -700, 0, -890 },
		{ -700, 0, -1765 },
		{ -700, 0, -2150 },

		// 庁舎前　※未完
		{ -700, 0, 1700 },
		{ 225, 0, 825 },
		{ 0, 0, 1350 },
		{ 0, 0, 1820 },
		{ 255, 0, 1820 },

		// 駐車場
		{ -300, 0, -50 },
		{ 120, 0, -50 },
		{ 450, 0, -100 },
		{ 570, 0, -235 },
		{ -300, 0, -890 },
		{ 120, 0, -890 },
		{ 255, 0, -890 },
		{ 570, 0, -800 },

		// 公園
		{ 255, 0, -1220 },
		{ -100, 0, -1500 },
		{ -465, 0, -1765 },
		{ 255, 0, -1765 },

		// 路地裏(X+)
		{ 850, 0, 1820 },
		{ 1150, 0, 1500 },
		{ 1150, 0, 740 },
		{ 1150, 0, 550 },
		{ 940, 0, 650 },
		{ 660, 0, 650 },
		{ 1150, 0, -550 },

		// ガソスタ
		{ -1050, 0, 90 },
		{ -1050, 0, 670 },
		{ -1280, 0, 100 },
		{ -1280, 0, 520 },
		{ -1280, 0, 670 },
		{ -1450, 0, 110 },
		{ -1600, 0, 420 },
		{ -1660, 0, 120 },
		{ -1700, 0, -140 },
		{ -1750, 0, 420 },

		// 路地裏(X-)　※未完
	};

	for (const auto& node : PATHFIND_NODE_LOC)
	{
		DrawSphere3D(Vec3ToVEC(node), 5.0f, 8, 0xc0c0c0u, 0xffffffu, TRUE);
	}
#endif
}

void Platform::InitParam()
{
	transform_.position = 0.0f;
	transform_.localPosition = Position3(0.0f, 900.0f, 0.0f);
	transform_.scale = 0.5f;
}

void Platform::InitModel()
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

void Platform::InitAnim()
{
}

void Platform::Collision()
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
