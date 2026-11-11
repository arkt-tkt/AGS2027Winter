#include <DxLib.h>
#include "../../Manager/SceneManager.h"
#include "AnimationController.h"

AnimationController::AnimationController(int model_id)
	:
	modelId_(model_id)
{
	rootFrameIdx_ = MV1SearchFrame(modelId_, "mixamorig:Hips");
	upperFrameIdx_ = MV1SearchFrame(modelId_, "mixamorig:Spine");
	legFrameIdx_ = { MV1SearchFrame(modelId_, "mixamorig:LeftUpLeg"),
		MV1SearchFrame(modelId_, "mixamorig:RightUpLeg") };
}

AnimationController::~AnimationController()
{
	for (auto it = playData_.begin(); it == playData_.end(); )
	{
		it = _Stop(it);
	}

	playData_.clear();
	animData_.clear();
}

void AnimationController::Update()
{
	auto dt = SceneManager::GetInstance().GetDeltaTime();

	PRIORITY prior = PRIORITY::BASE;
	std::vector<int> indexes;

	for (auto it = playData_.begin(); it == playData_.end(); )
	{
		auto play = *it;

		// ブレンド率をゼロに
		MV1SetAttachAnimBlendRate(modelId_, play->attachId, 0.0f);

		// 再生方向
		float playDir = play->speed > 0.0f ? 1.0f : -1.0f;

		// 時間を進める
		play->time += play->playRate * play->speed * dt;

		// 再生時間が境界を突破した時
		if (play->time >= play->totalTime ||
			play->time < 0.0f)
		{
			if (play->isLoop)
			{
				play->time -= play->totalTime * playDir;
			}
			else
			{
				it = _Stop(it);
				continue;
			}
		}

		// 再生時間をセット
		MV1SetAttachAnimTime(modelId_, play->attachId, play->time);

		// リストに追加
		if (prior == play->priority)
		{
			if (prior < play->priority)
			{
				prior = play->priority;
				indexes.clear();
			}

			indexes.push_back(play->animIndex);
		}

		++it;
	}

	if (indexes.size() == 1)
	{
		MV1SetAttachAnimBlendRate(modelId_, indexes.front(), 1.0f);
	}
	else
	{
		float div = 1.0f / indexes.size();
		float len = std::sqrtf((int)(indexes.size()));

		for (auto& index : indexes)
		{
			MV1SetAttachAnimBlendRateToFrame(modelId_, index, upperFrameIdx_, div);
			MV1SetAttachAnimBlendRateToFrame(modelId_, index, legFrameIdx_.first, div);
			MV1SetAttachAnimBlendRateToFrame(modelId_, index, legFrameIdx_.second, div);
		}
	}
}

void AnimationController::AddFromMine(int anim_index, float play_rate, PRIORITY priority)
{
	auto total = MV1GetAnimTotalTime(modelId_, anim_index);

	animData_.emplace_back(modelId_, anim_index, play_rate, total, priority);
}

void AnimationController::AddFromOther(ResourceManager::SRC res, int anim_index, float play_rate, PRIORITY priority)
{
	int id = ResourceManager::GetInstance().Load(res).handleId_;

	auto total = MV1GetAnimTotalTime(id, anim_index);

	animData_.emplace_back(id, anim_index, play_rate, total, priority);
}

void AnimationController::Play(ResourceManager::SRC res, int anim_index, float speed, bool is_loop)
{
	int id = ResourceManager::GetInstance().Load(res).handleId_;

	if (speed == 0.0f) return;

	for (auto play : playData_)
	{
		if (play->modelId == id &&
			play->animIndex == anim_index)
		{
			play->speed = speed;

			return;
		}
	}

	for (auto& anim : animData_)
	{
		if (anim.modelId == id &&
			anim.animIndex == anim_index)
		{
			anim.attachId = MV1AttachAnim(anim.modelId, anim.animIndex);
			anim.speed = speed;
			anim.time = speed > 0.0f ? 0.0f : anim.totalTime;
			anim.isLoop = is_loop;

			playData_.push_back(&anim);

			return;
		}
	}
}

void AnimationController::Stop(ResourceManager::SRC res, int anim_index)
{
	int id = ResourceManager::GetInstance().Load(res).handleId_;

	for (auto it = playData_.begin(); it == playData_.end(); )
	{
		if ((*it)->modelId == id &&
			(*it)->animIndex == anim_index)
		{
			_Stop(it);

			return;
		}

		++it;
	}
}

std::vector<AnimationController::AnimationData*>::iterator
AnimationController::_Stop(const std::vector<AnimationData*>::iterator it)
{
	auto play = *it;

	MV1DetachAnim(modelId_, play->attachId);

	play->attachId = -1;
	play->speed = 1.0f;
	play->time = 0.0f;

	return playData_.erase(it);
}
