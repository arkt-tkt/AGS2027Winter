#include <DxLib.h>
#include <cstdio>
#include "../Common/Geometry.h"
#include "../Common/MathUtil.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "Stage.h"

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Init()
{
	stage_.handleId = ResourceManager::GetInstance().Load(ResourceManager::SRC::MODEL_STAGE).handleId_;
	stage_.position = 0.0f;
	stage_.localPosition = Position3(0.0f, 0.0f, 0.0f);
	stage_.scale = 0.5f;
	stage_.collider = std::make_shared<Collider3D>(Collider3D::TYPE::STAGE, stage_.handleId);

	skyDome_.handleId = ResourceManager::GetInstance().Load(ResourceManager::SRC::MODEL_SKYDOME).handleId_;
	skyDome_.position = 0.0f;
	skyDome_.localPosition = Position3(0.0f, 120.0f, 0.0f);
	skyDome_.scale = 120.0f;

	Update();
}

void Stage::Update()
{
	stage_.Update();
	
	auto qRot = AngleAxis(Vector3(0, 1, 0), 0.0001);
	skyDome_.rotation += qRot.ToEuler();
	skyDome_.rotation.y = MathUtil::RadIn2PI(skyDome_.rotation.y);

	skyDome_.Update();
}

void Stage::Draw()
{
	stage_.Draw();
	skyDome_.Draw();
}

const Object3D& Stage::GetStageObject() const
{
	return stage_;
}
