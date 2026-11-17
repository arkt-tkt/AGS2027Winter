#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/DxLibUtil.h"
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
    auto& resLoad = ResourceManager::GetInstance();
    using COL_TY = Collider3D::TYPE;

    stage_ = std::make_unique<Stage>(this);
    stage_->Init();

    player_ = std::make_shared<Player>(0);
    player_->Init();
    player_->AddAwayColliders(stage_->GetColliders());

    stage_->SetPlayerPtr(player_);

    auto& cam = SceneManager::GetInstance().GetCameraPtr();
    cam.SetFollowTransform(&player_->GetTransform());
    cam.AddAwayColliders(player_->GetTypedColliders(COL_TY::PLAYER));
    cam.AddAwayColliders(stage_->GetColliders());

    EnemyManager::CreateInstance(this);
    EnemyManager::GetInstance().SetPlayerPtr(player_);
    EnemyManager::GetInstance().SetPathFindPtr(stage_->GetPathFindPtr());
    EnemyManager::GetInstance().AddAwayColliders(stage_->GetColliders());

    ProjectileManager::CreateInstance();

    AudioManager::GetInstance().Play(ResourceManager::SRC::MUSIC_BGM);

    auto& shaderProf = resLoad.Load(ResourceManager::SRC::SHADER_SCREEN);
    screenEffect_ = std::make_unique<PostEffect>(
        shaderProf.handleId_, shaderProf.constBufferId_, shaderProf.constBufferSize_);
    screenEffect_->SetUseTextures({ Application::GetInstance().GetPreDrawScreen() });
    screenEffect_->MakeSquareVertex(Vector2(0, 0), Application::RESOLUTION);
    screenEffect_->AddConstantBuffer({ 0.0f, 4.0f, 1.0f, 0.75f });
    screenEffect_->AddConstantBuffer({ 1.0f, 0.25f, 0.25f, 0.25f });
    screenEffect_->AddConstantBuffer({ 0.0f, 1.0f, 0.1f, 0.6f });

    effectScreenId_ = MakeScreen(int(Application::RESOLUTION.x), int(Application::RESOLUTION.y));

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

    auto stability = stage_->GetSpStability();

    // 空間安定度
    {
        if (stability < 15.0f)
        {
            auto spatialDmg = (15.0f - stability) / 3.0f * dt;

            player_->CalcHP(-spatialDmg);
        }
    }

    // ステージ進行
    {
        surviveTime_ += stability >= 100.0f ? dt * 2.0f : dt;

        if (surviveTime_ >= SURVIVAL_TIME)
        {
            scnMng_.SetNextSceneVariable(1);

            nextScene_ = SceneBase::SCENE::END;
        }
    }

    if (InputManager::GetInstance().CheckDownMap(
        player_->GetInputNumber(), InputManager::TAGS::VIEW_CHANGE_MODE))
    {
        scnMng_.GetCameraPtr().SwitchFollowMode();
    }

    if (InputManager::GetInstance().CheckDownMap(
        player_->GetInputNumber(), InputManager::TAGS::VIEW_SWITCH_SIDE))
    {
        scnMng_.GetCameraPtr().SwitchCameraSide();
    }

    ProjectileManager::GetInstance().Update();

    stage_->Update();

	EnemyManager::GetInstance().Update();

    player_->Update();

    {
        auto rate = player_->GetDamageTimer() / CharacterBase3D::DAMAGE_TIMER_MAX;

        unsigned int col1 = 0x404040u;
        unsigned int col2 = 0xc00000u;
        auto vigCol = UInt2ColorF(LerpColor(col1, col2, rate));
        
        screenEffect_->EditConstantBuffer(1, { 0.5f, vigCol.r, vigCol.g, vigCol.b });
    }
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

    // ここから画面効果(ポストエフェクト)
    SetDrawScreen(effectScreenId_);

    screenEffect_->Draw();

    SetDrawScreen(Application::GetInstance().GetPreDrawScreen());

    DrawGraph(0, 0, effectScreenId_, TRUE);
}

void GameScene::DrawUI()
{
#ifdef _DEBUG
    auto col = 0xffffffu;
    if (stage_->GetSpStability() <= 30.0f) col = 0xff4040u;
    DrawFormatString(20, 20, col, "注意: 空間安定度が15%%未満になると、継続ダメージを受けます");
    DrawFormatString(20, 80, 0xffffffu, "起動中の装置: %1d\n(空間安定度を高めます　起動している数に比例して効果も強くなります)", stage_->GetCaptureCount());
    DrawFormatString(20, 140, 0xffffffu, "作戦進行度: %.1f%%\n(100%%に達するとゲームクリアーです)", surviveTime_ / SURVIVAL_TIME * 100.0f);
#endif

    stage_->DrawUI();

    ProjectileManager::GetInstance().DrawUI();

    player_->DrawUI();

    DrawFormatString(100, 760, 0xffffffu, "空間安定度\n   %3d", MathUtil::Round(stage_->GetSpStability()));
}

bool GameScene::Release()
{
    DeleteGraph(effectScreenId_);

    AudioManager::GetInstance().Stop(ResourceManager::SRC::MUSIC_BGM);

    auto& cam = SceneManager::GetInstance().GetCameraPtr();
    cam.SetFollowTransform();
    cam.ResetAwayColliders();

    EnemyManager::DeleteInstance();
    ProjectileManager::DeleteInstance();

    return true;
}