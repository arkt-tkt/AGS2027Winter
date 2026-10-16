#include <DxLib.h>
#include <cstdio>
#include "../../Common/Geometry.h"
#include "../../Common/MathUtil.h"
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
}

void Stage::Draw()
{
	skyDome_.Draw();

	transform_.Draw();
}

void Stage::InitModel()
{
	transform_.handleId = ResourceManager::GetInstance().Load(ResourceManager::SRC::MODEL_STAGE).handleId_;
	transform_.position = 0.0f;
	transform_.localPosition = Position3(0.0f, 800.0f, 0.0f);
	transform_.scale = 0.5f;
	colliders_.emplace_back(std::make_shared<Collider3D>(Collider3D::TYPE::STAGE, transform_.handleId));

	int frames = MV1GetFrameNum(transform_.handleId);
	for (int i = 0; i < frames; ++i)
	{
		COLOR_F emi = MV1GetMaterialDifColor(transform_.handleId, i);
		emi.r *= 0.25f;
		emi.g *= 0.25f;
		emi.b *= 0.25f;
		MV1SetMaterialEmiColor(transform_.handleId, i, emi);
	}

	skyDome_.handleId = ResourceManager::GetInstance().Load(ResourceManager::SRC::MODEL_SKYDOME).handleId_;
	skyDome_.position = 0.0f;
	skyDome_.localPosition = Position3(0.0f, 0.0f, 0.0f);
	skyDome_.scale = 50.0f;
	MV1SetMaterialEmiColor(skyDome_.handleId, 0, COLOR_F(0.2f, 0.2f, 0.2f, 1.0f));
}

void Stage::InitAnim()
{
}
