#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/MathUtil.h"
#include "../../Manager/EnemyManager.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/ProjectileManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Object/Actor/Character/Player.h"
#include "GameScene.h"

bool GameScene::Init()
{
    using COL_TY = Collider3D::TYPE;

    stage_ = std::make_unique<Stage>(this);
    stage_->Init();

    player_ = std::make_shared<Player>(0);
    player_->Init();
    player_->AddAwayColliders(stage_->GetColliders());

    stage_->SetPlayerPtr(player_);

    auto& cam = SceneManager::GetInstance().GetCameraPtr();
    cam.SetFollowTarget(&player_->GetTransform());
    cam.AddAwayColliders(player_->GetTypedColliders(COL_TY::PLAYER));
    cam.AddAwayColliders(stage_->GetColliders());

    EnemyManager::CreateInstance(this);
    EnemyManager::GetInstance().SetPlayerPtr(player_);
    EnemyManager::GetInstance().AddAwayColliders(stage_->GetColliders());

    ProjectileManager::CreateInstance();

    return true;
}

void GameScene::Update()
{
    if (InputManager::GetInstance().CheckDownMap(1, InputManager::TAGS::SYSTEM_NG))
    {
        nextScene_ = SceneBase::SCENE::TITLE;
    }

    activeDevice_ = stage_->GetCaptureCount();

    auto dt = scnMng_.GetDeltaTime();

    auto spStabChange = (LOSS_STABILITY_PER_SECOND + GAIN_STABILITY_PER_SECOND * activeDevice_) * dt;
    stability_ = MathUtil::Clamp(stability_ + spStabChange, 0.0f, MAX_STABILITY);

    if (InputManager::GetInstance().CheckDownMap(
        player_->GetInputNumber(), InputManager::TAGS::CHANGE_VIEW))
    {
        scnMng_.GetCameraPtr().SwitchFollowMode();
    }

    ProjectileManager::GetInstance().Update();

    stage_->Update();

	EnemyManager::GetInstance().Update();

    player_->Update();
}

void GameScene::LateUpdate()
{
    EnemyManager::GetInstance().LateUpdate();

    player_->LateUpdate();
}

void GameScene::Draw()
{
    stage_->Draw();

    EnemyManager::GetInstance().Draw();

    player_->Draw();

    ProjectileManager::GetInstance().Draw();
}

void GameScene::DrawUI()
{
#ifdef _DEBUG
    auto col = 0xffffffu;
    if (stability_ <= 30.0f) col = 0xff4040u;
    DrawFormatString(20, 20, col, "Spacial Stability: %.1f%%", stability_);
    DrawFormatString(20, 40, 0xffffffu, "Active Device(s): %1d", activeDevice_);
#endif

    stage_->DrawUI();

    player_->DrawUI();
}

bool GameScene::Release()
{
    auto& cam = SceneManager::GetInstance().GetCameraPtr();
    cam.SetFollowTarget();
    cam.ResetAwayColliders();

    EnemyManager::DeleteInstance();
    ProjectileManager::DeleteInstance();

    return true;
}