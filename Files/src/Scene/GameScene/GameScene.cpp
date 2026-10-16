#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/MathUtil.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Object/Actor/Character/Player.h"
#include "GameScene.h"

bool GameScene::Init()
{
    using CTY = Collider3D::TYPE;

    stage_ = std::make_unique<Stage>();
    stage_->Init();

    player_ = std::make_shared<Player>(0);
    player_->Init();
    player_->AddAwayColliders(stage_->GetTypedColliders(CTY::STAGE));

    auto& cam = SceneManager::GetInstance().GetCameraPtr();
    cam.SetFollowTarget(&player_->GetTransform());
    cam.AddAwayColliders(player_->GetTypedColliders(CTY::PLAYER));
    cam.AddAwayColliders(stage_->GetTypedColliders(CTY::STAGE));

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


    if (InputManager::GetInstance().CheckDownKey(KEY_INPUT_C))
    {
        scnMng_.GetCameraPtr().SwitchFollowMode();
    }

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
    DrawFormatString(20, 20, col, "Spacial Stability: %.1f%%", stability_);
    DrawFormatString(20, 40, 0xffffffu, "Active Device(s): %1d", activeDevice_);
}

bool GameScene::Release()
{
    auto& cam = SceneManager::GetInstance().GetCameraPtr();
    cam.SetFollowTarget();
    cam.ResetAwayColliders();

    return true;
}