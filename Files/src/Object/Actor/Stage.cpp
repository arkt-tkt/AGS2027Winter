#include "CapturePoint.h"
#include "Platform.h"
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
		auto capPoint = std::make_shared<CapturePoint>(this);
		capPoint->Init();
		capPoint->transform_.position = CAPTURE_POINT_POS[i];
		capPoint->AddAwayColliders(platform_->GetTypedColliders(Collider3D::TYPE::STAGE));
		capturePoints_.push_back(std::move(capPoint));
	}
}

void Stage::Update()
{
	platform_->Update();

	for (auto& capPoint : capturePoints_)
	{
		capPoint->Update();
	}
}

void Stage::Draw()
{
	platform_->Draw();

	for (auto& capPoint : capturePoints_)
	{
		capPoint->Draw();
	}
}

void Stage::DrawUI()
{
	for (auto& capPoint : capturePoints_)
	{
		capPoint->DrawUI();
	}
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
