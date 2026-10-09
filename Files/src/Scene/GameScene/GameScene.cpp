#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/MathUtil.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Object/Player.h"
#include "GameScene.h"

bool GameScene::Init()
{
    stage_ = std::make_unique<Stage>();
    stage_->Init();

    player_ = std::make_shared<Player>(0);
    player_->Init();
    player_->AddAwayCollider(stage_->GetStageObject().collider);

    auto& cam = SceneManager::GetInstance().GetCameraPtr();
    cam.SetFollowTarget(&player_->GetObject3D());
    cam.AddAwayCollider(stage_->GetStageObject().collider);

    return true;
}

void GameScene::Update()
{
    if (InputManager::GetInstance().CheckDownMap(1, InputManager::TAGS::SYSTEM_NG))
    {
        nextScene_ = SceneBase::SCENE::TITLE;
    }

    if (InputManager::GetInstance().CheckDownKey(KEY_INPUT_M))
    {
        ++activeDevice_;
    }
    if (InputManager::GetInstance().CheckDownKey(KEY_INPUT_N))
    {   
        --activeDevice_;
    }
    activeDevice_ = MathUtil::Clamp(activeDevice_, 0, 5);

    auto dt = scnMng_.GetDeltaTime();

    auto spStabChange = (LOSS_STABILITY_PER_SECOND + GAIN_STABILITY_PER_SECOND * activeDevice_) * dt;
    stability_ = MathUtil::Clamp(stability_ + spStabChange, 0.0f, MAX_STABILITY);

    stage_->Update();

    player_->Update();
}

void GameScene::Draw()
{
    stage_->Draw();

    player_->Draw();
}

void GameScene::DrawUI()
{
    auto col = 0xffffffu;
    if (stability_ <= 30.0f) col = 0xff4040u;
    DrawFormatString(20, 20, col, "Spacial Stability: %.0f%%", stability_);
    DrawFormatString(20, 40, 0xffffffu, "Active Device(s): %1d", activeDevice_);
}

bool GameScene::Release()
{
    auto& cam = SceneManager::GetInstance().GetCameraPtr();
    cam.SetFollowTarget();
    cam.ResetAwayColliders();

    return true;
}