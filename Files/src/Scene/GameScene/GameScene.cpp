#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/MathUtil.h"
#include "../../Manager/AudioManager.h"
#include "../../Manager/EnemyManager.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/ProjectileManager.h"
#include "../../Manager/ResourceManager.h"
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
    EnemyManager::GetInstance().SetPathFindPtr(stage_->GetPathFindPtr());
    EnemyManager::GetInstance().AddAwayColliders(stage_->GetColliders());

    ProjectileManager::CreateInstance();

	AudioManager::GetInstance().Play(ResourceManager::SRC::MUSIC_BGM);

    return true;
}

void GameScene::Update()
{
    if (InputManager::GetInstance().CheckDownMap(1, InputManager::TAGS::SYSTEM_NG))
    {
        nextScene_ = SceneBase::SCENE::TITLE;
    }
    else if (!player_->IsAlive())
    {
        nextScene_ = SceneBase::SCENE::END;
    }

    auto dt = scnMng_.GetDeltaTime();

    // 空間安定度
    {
        activeDevice_ = stage_->GetCaptureCount();

        auto spStabChange = (LOSS_STABILITY_PER_SECOND + GAIN_STABILITY_PER_SECOND * activeDevice_) * dt;
        stability_ = MathUtil::Clamp(stability_ + spStabChange, 0.0f, MAX_STABILITY);

        if (stability_ < 15.0f)
        {
            auto spatialDmg = (15.0f - stability_) / 3.0f * dt;

            player_->CalcHP(-spatialDmg);
        }
    }

    // ステージ進行
    {
        surviveTime_ += stability_ >= 100.0f ? dt * 2.0f : dt;

        if (surviveTime_ >= SURVIVAL_TIME)
        {
            scnMng_.SetNextSceneVariable(1);

            nextScene_ = SceneBase::SCENE::END;
        }
    }

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
    DrawFormatString(20, 20, col, "注意: 空間安定度が15%%未満になると、継続ダメージを受けます");
    DrawFormatString(20, 80, 0xffffffu, "起動中の装置: %1d\n(空間安定度を高めます　起動している数に比例して効果も強くなります)", activeDevice_);
    DrawFormatString(20, 140, 0xffffffu, "作戦進行度: %.1f%%\n(100%%に達するとゲームクリアーです)", surviveTime_ / SURVIVAL_TIME * 100.0f);
#endif

    stage_->DrawUI();

    player_->DrawUI();

    DrawFormatString(100, 760, 0xffffffu, "空間安定度\n   %3d", MathUtil::Round(stability_));
}

bool GameScene::Release()
{
    AudioManager::GetInstance().Stop(ResourceManager::SRC::MUSIC_BGM);

    auto& cam = SceneManager::GetInstance().GetCameraPtr();
    cam.SetFollowTarget();
    cam.ResetAwayColliders();

    EnemyManager::DeleteInstance();
    ProjectileManager::DeleteInstance();

    return true;
}