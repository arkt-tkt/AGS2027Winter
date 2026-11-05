#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/InputManager.h"
#include "EndScene.h"

bool EndScene::Init()
{
    return true;
}

void EndScene::Update()
{
    if (InputManager::GetInstance().CheckDownMap(1, InputManager::TAGS::SYSTEM_OK))
    {
        nextScene_ = SceneBase::SCENE::GAME;
    }

    if (InputManager::GetInstance().CheckDownMap(1, InputManager::TAGS::SYSTEM_NG))
    {
        nextScene_ = SceneBase::SCENE::TITLE;
    }
}

void EndScene::Draw()
{
    std::string message;

    if (variable_ == 0)
    {
        message = "工作員: URS-US_026\n";
        message += "ステータス: 作戦行動中に死亡\n";
        message += "\n";
        message += "ゲームオーバー\n";
        message += "\n";
        message += "\n";
        message += "\n";
        message += "各決定キーでゲームをやり直します　各キャンセルキーでタイトル画面に戻ります\n";
    }

    DrawString(100, 100, message.c_str(), 0xffffffu);
}

void EndScene::DrawUI()
{
}

bool EndScene::Release()
{
    return true;
}