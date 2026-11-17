#include "../../../Algorithm/PathFind.h"
#include "../../../Common/MathUtil.h"
#include "../../../Manager/FPSManager.h"
#include "CapturePoint.h"
#include "Platform.h"
#include "Resupply.h"
#include "Stage.h"

Stage::Stage(GameScene* ptr)
	:
	gameScene_(ptr)
{
}

void Stage::Init()
{
	platform_ = std::make_shared<Platform>();
	platform_->Init();

	capturePoints_.reserve(CAPTURE_POINT_NUM);
	for (int i = 0; i < CAPTURE_POINT_NUM; ++i)
	{
		auto capPoint = std::make_shared<CapturePoint>(this, i);
		capPoint->Init();
		capPoint->transform_.position = CAPTURE_POINT_POS[i];
		capPoint->AddAwayColliders(platform_->GetTypedColliders(Collider3D::TYPE::STAGE));
		capturePoints_.push_back(std::move(capPoint));
	}

	pathFind_ = std::make_shared<PathFind>();

	resupply_ = std::make_shared<Resupply>(this);
	resupply_->AddAwayColliders(platform_->GetTypedColliders(Collider3D::TYPE::STAGE));
	resupply_->Init();
}

void Stage::Update()
{
	auto dt = FPSManager::GetInstance().GetDeltaTime<float>();

	platform_->Update();

	for (auto& capPoint : capturePoints_)
	{
		capPoint->Update();
	}

	resupply_->Update();

	auto spStabChange = (LOSS_STABILITY_PER_SECOND + GAIN_STABILITY_PER_SECOND * GetCaptureCount()) * dt;
	stability_ = MathUtil::Clamp(stability_ + spStabChange, 0.0f, MAX_STABILITY);
}

void Stage::Draw()
{
	platform_->Draw();

#ifdef _DEBUG
	pathFind_->DrawDebug();
#endif

	for (auto& capPoint : capturePoints_)
	{
		capPoint->Draw();
	}

	resupply_->Draw();
}

void Stage::DrawUI()
{
	for (auto& capPoint : capturePoints_)
	{
		capPoint->DrawUI();
	}

	resupply_->DrawUI();
}

std::vector<std::shared_ptr<Collider3D>> Stage::GetColliders() const
{
	const auto GET_COL_TYPE = Collider3D::TYPE::STAGE;

	std::vector<std::shared_ptr<Collider3D>> ret =
		platform_->GetTypedColliders(GET_COL_TYPE);

	ret.reserve(ret.size() + capturePoints_.size());

	for (auto& capPoint : capturePoints_)
	{
		auto capColliders = capPoint->GetTypedColliders(GET_COL_TYPE);
		ret.insert(ret.end(), capColliders.begin(), capColliders.end());
	}

	auto resupColliders = resupply_->GetTypedColliders(GET_COL_TYPE);
	ret.insert(ret.end(), resupColliders.begin(), resupColliders.end());

	return ret;
}

int Stage::GetCaptureCount()
{
	int ret = 0;

	for (auto& capPoint : capturePoints_)
	{
		if (capPoint->IsActive()) ++ret;
	}

	return ret;
}
