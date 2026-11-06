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
    titleMessage += "\n";
    titleMessage += "\n";
    titleMessage += "\n";
    titleMessage += "ゲーム進行の方法\n";
    titleMessage += "HPがゼロにならないように、敵を撃退して下さい。\n";
    titleMessage += "\n";
    titleMessage += "貴方の武器はアサルトライフルです。射程距離は無限ですが、弾は有限です。\n";
    titleMessage += "弾薬が減ってきたら、「R」のアイコンに向かって補給して下さい。\n";
    titleMessage += "\n";
    titleMessage += "ゲーム中は自動で「空間安定度」が下がります。\n";
    titleMessage += "空間安定度が下がりすぎると、継続的にダメージを受けます。\n";
    titleMessage += "\n";
    titleMessage += "空間安定度を上げるには、発電機を起動してください。\n";
    titleMessage += "発電機はマップ上に5つ存在し、「G」のアイコンで表示されます。\n";
    titleMessage += "\n";
    titleMessage += "約4分間生存すればゲームクリアーです。\n";

    DrawString(100, 100, titleMessage.c_str(), 0xffffffu);
}

void TitleScene::DrawUI()
{
}

bool TitleScene::Release()
{
    return true;
}