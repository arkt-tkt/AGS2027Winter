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
}

void TitleScene::DrawUI()
{
    DrawString(20, 20, "Project: UNKNOWN\nver. alpha", 0xffffffu);
}

bool TitleScene::Release()
{
    return true;
}