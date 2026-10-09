#include <DxLib.h>
#include "../../Manager/SceneManager.h"
#include "Animation.h"

Animation::Animation(int modelId)
{
	modelId_ = modelId;
}

Animation::~Animation()
{
	MV1DetachAnim(modelId_, anims_[playType_.first][playType_.second].attachIdx);

	for (auto& models : anims_)
	{
		for (const auto& anim : models.second)
		{
			MV1DeleteModel(anim.second.model);
		}
		models.second.clear();
	}
	anims_.clear();
}

void Animation::AddFromMine(int type, float speed)
{
	AnimData anim;

	auto src = ResourceManager::SRC::NONE;
	anim.model = -1;
	anim.animType = type;
	anim.speed = speed;

	if (anims_[src].count(type) == 0)
	{
		// 追加
		anims_[src].emplace(type, anim);
	}
	else
	{
		// 入れ替え
		anims_[src][type].model = anim.model;
		anims_[src][type].animType = anim.animType;
		anims_[src][type].attachIdx = -1;
		anims_[src][type].totalTime = 0.0f;
	}
}

void Animation::AddFromOther(ResourceManager::SRC src, int type, float speed)
{
	AnimData anim;

	anim.model = ResourceManager::GetInstance().LoadModelDuplicate(src);
	anim.animType = type;
	anim.speed = speed;

	if (anims_[src].count(type) == 0)
	{
		// 追加
		anims_[src].emplace(type, anim);
	}
	else
	{
		// 入れ替え
		anims_[src][type].model = anim.model;
		anims_[src][type].animType = anim.animType;
		anims_[src][type].attachIdx = -1;
		anims_[src][type].totalTime = 0.0f;
	}
}

void Animation::Play(ResourceManager::SRC src, int type, bool isLoop)
{
	if (playType_.second != -1)
	{
		// モデルからアニメーションを外す
		MV1DetachAnim(modelId_, anims_[playType_.first][playType_.second].attachIdx);
	}
	
	// アニメーション種別を変更
	playType_.first = src;
	playType_.second = type;
	playAnim_ = anims_[src][type];
	
	// 初期化
	playAnim_.step = 0.0f;

	// モデルにアニメーションを付ける
	playAnim_.attachIdx = AttachAnim(playAnim_.model);

	// アニメーション総時間の取得
	playAnim_.totalTime = MV1GetAttachAnimTotalTime(modelId_, playAnim_.attachIdx);

	// アニメーションループ
	isLoop_ = isLoop;
}

void Animation::Update()
{
	// 経過時間の取得
	float dt = SceneManager::GetInstance().GetDeltaTime();

	// 再生
	playAnim_.step += (dt * playAnim_.speed);

	// アニメーション終了判定
	bool isEnd = false;
	if (playAnim_.step > playAnim_.totalTime)
	{
		isEnd = true;
	}

	if (isEnd)
	{
		// アニメーションが終了したら
		if (isLoop_)
		{
			// ループ再生
			playAnim_.step = 0.0f;
		}
		else
		{
			// ループしない
			playAnim_.step = playAnim_.totalTime;
		}
	}

	// アニメーション設定
	MV1SetAttachAnimTime(modelId_, playAnim_.attachIdx, playAnim_.step);
}

void Animation::SetPlayingAnimSpeed(float speed)
{
	speed_ = speed;
}

int Animation::AttachAnim(int anim_model)
{
	int attachIdx = -1;

	int animIdx = 0;
	// アニメーションの数を取得
	if (MV1GetAnimNum(anim_model) > 1)
	{
		// アニメーションが複数保存されていたら、番号1を指定
		animIdx = 1;
	}

	if (anim_model == -1)
	{
		// 同じファイルにアニメーションが存在する
		attachIdx = MV1AttachAnim(modelId_, playType_.second);
	}
	else
	{
		// 別ファイルにアニメーションが分けられている
		attachIdx = MV1AttachAnim(modelId_, animIdx, anim_model);
	}

	return attachIdx;
}
