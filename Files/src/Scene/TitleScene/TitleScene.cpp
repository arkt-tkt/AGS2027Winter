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
	std::string message = "Project: UNKNOWN\n";
    message += "ver. alpha\n";
    message += "\n";
	message += "各決定キーでゲーム画面に移動します　各キャンセルキーでゲームを終了します\n";
	message += "ゲーム画面中、各キャンセルキーでタイトル画面に戻ります\n";
    message += "\n";
    message += "\n";
    message += "\n";
    message += "ゲーム進行の方法\n";
    message += "HPがゼロにならないように、敵を撃退して下さい。\n";
    message += "\n";
    message += "貴方の武器はアサルトライフルです。射程距離は無限ですが、弾は有限です。\n";
    message += "弾薬が減ってきたら、「R」のアイコンに向かって補給して下さい。\n";
    message += "\n";
    message += "ゲーム中は自動で「空間安定度」が下がります。\n";
    message += "空間安定度が下がりすぎると、継続的にダメージを受けます。\n";
    message += "\n";
    message += "空間安定度を上げるには、発電機を起動してください。\n";
    message += "発電機はマップ上に5つ存在し、「G」のアイコンで表示されます。\n";
    message += "\n";
    message += "約4分間生存すればゲームクリアーです。\n";

    DrawString(100, 100, message.c_str(), 0xffffffu);
}

void TitleScene::DrawUI()
{
}

bool TitleScene::Release()
{
    return true;
}