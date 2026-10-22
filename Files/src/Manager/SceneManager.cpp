#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Scene/TitleScene/TitleScene.h"
#include "../Scene/GameScene/GameScene.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "ScoreManager.h"
#include "AudioManager.h"

SceneManager* SceneManager::instance_ = nullptr;

bool SceneManager::Init()
{
	InitClass();
	InitParam();
	return true;
}

void SceneManager::Update()
{
	// Faderの更新
	fader_->Update();

	// Faderの状態を確認して次に進む／スキップする
	if (!CheckFader())
	{
		// 中身が無い状態で動かすと危ないので
		if (sceneList_.empty()) return;

		// 配列末尾のポインタを取る
		auto back = sceneList_.back().get();

		// ポーズ判定の更新
		prevPause_ = isPause_;
		isPause_ = back->GetMyScene() == SceneBase::SCENE::PAUSE ? true : false;

		// アクティブなシーンだけ更新する
		back->Update();

		// カメラの更新
		camera_->Update();

		back->LateUpdate();

		// ポーズ中以外はエフェクトを更新する
		if (!isPause_) UpdateEffekseer3D();

		// 次のシーンを得る
		auto next = back->GetNextScene();

		// 他のシーンに移行させたいかどうか
		if (back->GetMyScene() != next)
		{
			// シーン遷移開始
			ChangeScene(next);
		}
	}
	else
	{
		// カメラの更新
		camera_->Update();
	}
}

void SceneManager::Draw()
{
	camera_->BeforeDraw();

	// 非アクティブのシーンも描画する
	for (auto it = sceneList_.begin(); it != sceneList_.end(); ++it)
	{
		(*it)->Draw();
	}

	// フェーダー等の画面エフェクトを挟む
	fader_->Draw();

	// SetDrawScreen関数を呼んだので
	camera_->BeforeDraw();

	// ここからUI系描画
	for (auto it = sceneList_.begin(); it != sceneList_.end(); ++it)
	{
		(*it)->DrawUI();
	}

#ifdef _DEBUG
	camera_->DebugDraw();
#endif
}

bool SceneManager::Release()
{
	// シーンが無くなるまで解放する
	while (sceneList_.size() > 0) ReleaseScene();

	// リストの後始末
	sceneList_.clear();

	return true;
}

void SceneManager::ReleaseScene()
{
	// 現在アクティブなシーンを解放して削除
	sceneList_.back()->Release();

	// リストからも削除
	sceneList_.pop_back();
}

SceneBase* SceneManager::GetTopScene() const
{
	if (!sceneList_.empty()) {
		return sceneList_.back().get();
	}
	return nullptr;
}

void SceneManager::SetNextSceneVariable(unsigned int u) { nextSceneVariable_ = u; }

bool SceneManager::IsPause() const { return isPause_; }

bool SceneManager::PrevPause() const { return prevPause_; }

bool SceneManager::InitClass()
{
	camera_ = std::make_unique<Camera>();

	fader_ = std::make_unique<Fader>();

	return true;
}

void SceneManager::InitParam()
{
	// 背景色
	SetBackgroundColor(0x08, 0x00, 0x10);

	// 最初はタイトル画面から
	ChangeScene(SceneBase::SCENE::TITLE);

	//postEffectScreen_ = MakeScreen(Application::RESOLUTION_WIDTH, Application::RESOLUTION_HEIGHT);

	nextSceneVariable_ = 0u;
}

void SceneManager::ChangeScene(SceneBase::SCENE scene)
{
	waitSceneId_ = scene;

	// 現在のシーンがない（つまり起動直後である）場合、即座に遷移する
	//if (sceneList_.empty() || scene == SceneBase::SCENE::LOAD)
	if (sceneList_.empty())
	{
		// フェードモードの設定を削除し、即座にシーンを構築
		DoChangeScene(waitSceneId_);

		// 次回のシーン遷移用の変数をリセット（念のため）
		waitSceneId_ = SceneBase::SCENE::NONE;

		return;
	}

	// ポーズシーンへの移行は即時実行
	if (waitSceneId_ == SceneBase::SCENE::PAUSE)
	{
		// ポーズSEを再生
		//AudioManager::GetInstance().Play(ResourceManager::SRC::SE_PAUSE);

		DoChangeScene(waitSceneId_);
		return;
	}

	// ポーズシーンから特定のシーンへは即座に移行
	if (sceneList_.back()->GetMyScene() == SceneBase::SCENE::PAUSE)
	{
		if (waitSceneId_ == SceneBase::SCENE::GAME)
		{
			// ポーズSEを再生
			//AudioManager::GetInstance().Play(ResourceManager::SRC::SE_PAUSE_CANCEL);
			DoChangeScene(waitSceneId_);
			return;
		}
	}

	fader_->SetFadeMode(Fader::FADE_MODE::FADE_OUT, 0.5f);
}

bool SceneManager::CheckFader()
{
	// 現在のフェードモードを取得
	auto fmode = fader_->GetFadeMode();

	switch (fmode)
	{
	case Fader::FADE_MODE::FADE_OUT:
		// 処理が完了次第
		if (fader_->IsFadeEnd())
		{
			if (sceneList_.back()->GetMyScene() == SceneBase::SCENE::GAME)
			{
			}

			// シーンを切り替える
			DoChangeScene(waitSceneId_);

			// シーンリストの中身があり、かつ元のシーンが「無し」でない場合だけ
			if (!sceneList_.empty() &&
				sceneList_.back()->GetMyScene() != SceneBase::SCENE::NONE)
			{
				// シーン待ち状態を解除
				waitSceneId_ = SceneBase::SCENE::NONE;

				// フェードモードをフェードインに
				fader_->SetFadeMode(Fader::FADE_MODE::FADE_IN, 0.5f);
			}
			else
			{
				Application::GetInstance().Exit();
				return true;
			}
		}
		break;
	case Fader::FADE_MODE::FADE_IN:
		// 処理が完了次第
		if (fader_->IsFadeEnd())
		{
			// フェードモードを無しに
			fader_->SetFadeMode(Fader::FADE_MODE::NONE, 0.0f);
		}
		break;
	default:
		// フェードモード無しの場合、
		// この関数の後の処理に移行させるためにfalseを返す
		return false;
	}

	// 例外的な処理は、このスコープ内でまとめて記述してくれると嬉しいです
	{
	}

	return true;
}

void SceneManager::DoChangeScene(SceneBase::SCENE scene)
{
	std::unique_ptr<SceneBase> ret = nullptr;
	unsigned int var = nextSceneVariable_;

	if (scene != SceneBase::SCENE::PAUSE)
	{
		while (!sceneList_.empty())
		{
			// 現在アクティブなシーンが目標のシーンなら、関数から抜ける
			if (sceneList_.back()->GetMyScene() == scene)
			{
				sceneList_.back()->ResetNextScene();
				return;
			}

			// 現在のシーンを解放
			else ReleaseScene();
		}

		// シーンを切り替える
		switch (scene)
		{
		case SceneBase::SCENE::TITLE:
			ret = std::make_unique<TitleScene>();
			break;
		case SceneBase::SCENE::GAME:
			ret = std::make_unique<GameScene>();
			break;
		}
	}
	else
	{
		// ポーズシーン
		//ret = std::make_unique<PauseScene>();
	}

	if (ret != nullptr)
	{
		ret->Assign(scene, var);
		ret->Init();
		sceneList_.push_back(std::move(ret));

		nextSceneVariable_ = 0u;
	}

	return;
}

unsigned int SceneManager::GetNextSceneVariable() const
{
	return nextSceneVariable_;
}