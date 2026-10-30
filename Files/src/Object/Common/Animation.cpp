#include <DxLib.h>
#include "../../Common/DxLibUtil.h"
#include "../../Manager/SceneManager.h"
#include "Animation.h"

Animation::Animation(int modelId)
{
	modelId_ = modelId;

	playDatas_.reserve(PLAYDATA_RESERVE);
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

void Animation::Update()
{
	// 経過時間の取得
	const float dt = SceneManager::GetInstance().GetDeltaTime();

#if false
	// 再生
	playAnim_.step += (playAnim_.speed * dt * speed_);

	// アニメーション終了判定
	bool isEnd = false;
	if (playAnim_.step >= playAnim_.totalTime)
	{
		isEnd = true;
	}

	if (isEnd)
	{
		// アニメーションが終了したら
		if (isLoop_)
		{
			// ループ再生
			playAnim_.step -= playAnim_.totalTime;
		}
		else
		{
			// ループしない
			playAnim_.step = playAnim_.totalTime;
		}
	}

	// アニメーション設定
	MV1SetAttachAnimTime(modelId_, playAnim_.attachIdx, playAnim_.step);
#else
	int count = 0;

	for (auto it = playDatas_.begin(); it != playDatas_.end(); )
	{
		auto& playData = (*it);

		// 再生
		(*it).playAnim.step += ((*it).playAnim.speed * dt * speed_);

		// アニメーション終了判定
		bool isEnd = false;
		if ((*it).playAnim.step >= (*it).playAnim.totalTime)
		{
			isEnd = true;
		}

		if (isEnd)
		{
			// アニメーションが終了したら
			if ((*it).isLoop)
			{
				// ループ再生
				(*it).playAnim.step -= (*it).playAnim.totalTime;
			}
			else
			{
				// ループしない
				(*it).playAnim.step = (*it).playAnim.totalTime;
			}
		}

		if (playDatas_.size() > 1 && count == 0)
		{
			(*it).blendRate -= dt / TRANSITION_TIME;

			if ((*it).blendRate <= 0.0f)
			{
				// モデルからアニメーションを外す
				MV1DetachAnim(modelId_, (*it).playAnim.attachIdx);

				it = playDatas_.erase(it);
				continue;
			}
		}
		else
		{
			(*it).blendRate = (std::min)((*it).blendRate + dt / TRANSITION_TIME, 1.0f);
		}

		// アニメーション設定
		MV1SetAttachAnimTime(modelId_, (*it).playAnim.attachIdx, (*it).playAnim.step);

		MV1SetAttachAnimBlendRate(modelId_, (*it).playAnim.attachIdx, (*it).blendRate);

		++it;
		++count;
	}
#endif
}

void Animation::Play(ResourceManager::SRC src, int type, bool isLoop)
{
#if false
	if (playType_ == std::pair{ src, type }) return;

	if (playType_.second != -1)
	{
		// モデルからアニメーションを外す
		MV1DetachAnim(modelId_, playAnim_.attachIdx);
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
#else
	if (playDatas_.size() >= PLAYDATA_RESERVE) return;

	if (playDatas_.size() > 0 &&
		playDatas_.back().playType == std::pair{src, type}) return;

	PlayData playData;

	// アニメーション種別を変更
	playData.playType.first = src;
	playData.playType.second = type;
	playData.playAnim = anims_[src][type];

	// 初期化
	playData.playAnim.step = 0.0f;

	// モデルにアニメーションを付ける
	playData.playAnim.attachIdx = AttachAnim(playData.playAnim.model);

	// アニメーション総時間の取得
	playData.playAnim.totalTime = MV1GetAttachAnimTotalTime(modelId_, playData.playAnim.attachIdx);

	// アニメーションループ
	playData.isLoop = isLoop;

	playData.blendRate = playDatas_.empty() ? 1.0f : 0.0f;

	playDatas_.push_back(playData);
#endif
}

bool Animation::IsPlaying() const
{
	return playAnim_.step != playAnim_.totalTime;
}

bool Animation::IsPlaying(ResourceManager::SRC src, int type) const
{
	return playType_.first == src && playType_.second == type && playAnim_.step != playAnim_.totalTime;
}

Vector3 Animation::GetAnimLocalPos(int frame) const
{
	return VECToVec3(MV1GetAttachAnimFrameLocalPosition(modelId_, playAnim_.attachIdx, frame));
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

int Animation::DetachAnim(int anim_model)
{
	return 0;
}
