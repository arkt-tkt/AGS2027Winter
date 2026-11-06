#include "../../../Common/DxLibUtil.h"
#include "../../../Common/MathUtil.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SceneManager.h"
#include "../Character/Player.h"
#include "Stage.h"
#include "Resupply.h"

Resupply::Resupply(Stage* ptr)
	:
	stage_(*ptr),
	CharacterBase3D()
{
}

Resupply::~Resupply()
{
}

void Resupply::Draw()
{
	// モデル
	MV1DrawModel(transform_.modelId);

	// オーバーレイ表示
	Vector3 pos = transform_.position + Vector3(0, 20, 0);

	VECTOR scPos = ConvWorldPosToScreenPos(Vec3ToVEC(pos));

	if (!MathUtil::Within(scPos.z, 0.0f, 1.0f)) return;

	auto col = activeTimer_ > 0.0f ? 0x808080u : 0xffffffu;

	DrawStringToCenterPos(MathUtil::Round(scPos.x), MathUtil::Round(scPos.y), "R", col);
}

void Resupply::DrawUI()
{
	if (inRange_ && activeTimer_ <= 0.0f)
	{
		DrawString(80, 900, "Eキー/右ボタン: 長押しで弾薬を補充する", 0xffffffu);
	}
}

void Resupply::Activate()
{
	auto pl = stage_.GetPlayerPtr();
	
	pl->GetEquippedWeaponPtr()->Refill();

	activeTimer_ = 20.0f;
}

void Resupply::InitModel()
{
	transform_.modelId = resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_AMMO_CRATE);
	transform_.scale = Vector3(SCALE, SCALE, SCALE);
	transform_.position = Position3(-700, 90, 2050);

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

	lineCollider_ = { Vector3(0, -4, 0), Vector3(0, 24, 0) };
}

void Resupply::Move()
{
	auto dt = scnMng_.GetDeltaTime();

	auto plPos = stage_.GetPlayerPtr()->GetTransform().position;

	if (activeTimer_ > 0.0f)
	{
		activeTimer_ = (std::max)(activeTimer_ - scnMng_.GetDeltaTime(), 0.0f);
	}
	else
	{
		if ((plPos - transform_.position).LengthSquare() <= 55.0f * 55.0f)
		{
			inRange_ = true;

			if (!interactTimer_ &&
				InputManager::GetInstance().CheckDownMap(
					stage_.GetPlayerPtr()->GetInputNumber(), InputManager::TAGS::INTERACT))
			{
				interactTimer_ += dt;
			}
			else if (interactTimer_ &&
				InputManager::GetInstance().CheckNowMap(
					stage_.GetPlayerPtr()->GetInputNumber(), InputManager::TAGS::INTERACT))
			{
				interactTimer_ += dt;
			}
			else
			{
				interactTimer_ = 0.0f;
			}
		}
		else
		{
			inRange_ = false;
			interactTimer_ = 0.0f;
		}

		if (interactTimer_ >= INTERACT_TIME)
		{
			Activate();
			inRange_ = false;
			interactTimer_ = 0.0f;
		}
	}

	// 本移動
	transform_.position += velocity_;
}
