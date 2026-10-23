#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/InputManager.h"
#include "TitleScene.h"

bool TitleScene::Init()
{
    return true;
}

void TitleScene::Update()
{
    if (InputManager::GetInstance().CheckDownMap(1, InputManager::TAGS::SYSTEM_OK))
    {
        nextScene_ = SceneBase::SCENE::GAME;
    }

    if (InputManager::GetInstance().CheckDownMap(1, InputManager::TAGS::SYSTEM_NG))
    {
        app_.Exit();
    }
}

void TitleScene::Draw()
{
	std::string titleMessage = "Project: UNKNOWN\n";
    titleMessage += "ver. alpha\n";
    titleMessage += "\n";
	titleMessage += "各決定キーでゲーム画面に移動します　各キャンセルキーでゲームを終了します\n";
	titleMessage += "ゲーム画面中、各キャンセルキーでタイトル画面に戻ります\n";

    DrawString(100, 100, titleMessage.c_str(), 0xffffffu);
}

void TitleScene::DrawUI()
{
}

bool TitleScene::Release()
{
    return true;
}