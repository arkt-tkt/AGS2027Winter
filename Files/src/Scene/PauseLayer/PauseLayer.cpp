#include <DxLib.h>
#include "../../Manager/InputManager.h"
#include "PauseLayer.h"

bool PauseLayer::Init()
{
	return true;
}

void PauseLayer::Update()
{
    if (InputManager::GetInstance().CheckDownMap(1, InputManager::TAGS::SYSTEM_OK))
    {
        nextScene_ = SceneBase::SCENE::TITLE;
    }

    if (InputManager::GetInstance().CheckDownMap(1, InputManager::TAGS::SYSTEM_NG))
    {
        nextScene_ = SceneBase::SCENE::GAME;
    }
}

void PauseLayer::Draw()
{

}

void PauseLayer::DrawUI()
{
}

bool PauseLayer::Release()
{
	return true;
}
