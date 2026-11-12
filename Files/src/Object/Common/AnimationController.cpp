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
	StopAll();

	playData_.clear();
	animData_.clear();
}

void AnimationController::Update()
{
	auto dt = SceneManager::GetInstance().GetDeltaTime();

	PARTS upperLv = BASE;
	std::vector<int> upperArr;
	PARTS legsLv = BASE;
	std::vector<int> legsArr;

	for (auto it = playData_.begin(); it != playData_.end(); )
	{
		auto play = *it;

		// ブレンド率をゼロに
		MV1SetAttachAnimBlendRate(modelId_, play->attachId, 0.0f);

		// 時間を進める
		play->time += play->playRate * play->speed * dt;

		// 再生時間が境界を突破した時
		if (play->time >= play->totalTime ||
			play->time < 0.0f)
		{
			// 再生方向
			float playDir = play->speed > 0.0f ? 1.0f : -1.0f;

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
		if (upperLv <= play->parts && play->parts != LOWER_BODY)
		{
			if (upperLv < play->parts)
			{
				upperLv = play->parts;
				upperArr.clear();
			}

			upperArr.push_back(play->attachId);
		}
		
		if (legsLv <= play->parts && play->parts != UPPER_BODY)
		{
			if (legsLv < play->parts)
			{
				legsLv = play->parts;
				legsArr.clear();
			}

			legsArr.push_back(play->attachId);
		}

		++it;
	}

	float div = 1.0f / upperArr.size();

	for (auto& index : upperArr)
	{
		MV1SetAttachAnimBlendRateToFrame(modelId_, index, upperFrameIdx_, div, TRUE);
	}

	div = 1.0f / legsArr.size();

	for (auto& index : legsArr)
	{
		MV1SetAttachAnimBlendRateToFrame(modelId_, index, rootFrameIdx_, div, FALSE);
		MV1SetAttachAnimBlendRateToFrame(modelId_, index, legFrameIdx_.first, div, TRUE);
		MV1SetAttachAnimBlendRateToFrame(modelId_, index, legFrameIdx_.second, div, TRUE);
	}
}

void AnimationController::AddFromMine(int anim_index, float play_rate, PARTS parts)
{
	auto total = MV1GetAnimTotalTime(modelId_, anim_index);

	animData_.emplace_back(modelId_, anim_index, play_rate, total, parts);
}

void AnimationController::AddFromOther(ResourceManager::SRC res, int anim_index, float play_rate, PARTS parts)
{
	int id = ResourceManager::GetInstance().Load(res).handleId_;

	auto total = MV1GetAnimTotalTime(id, anim_index);

	animData_.emplace_back(id, anim_index, play_rate, total, parts);
}

void AnimationController::Play(ResourceManager::SRC res, int anim_index, float speed, bool is_loop)
{
	int id = ResourceManager::GetInstance().Load(res).handleId_;

	if (speed == 0.0f) return;

	for (auto play : playData_)
	{
		if (play->resId == id && play->animIndex == anim_index)
		{
			play->speed = speed;

			return;
		}
	}

	for (auto& anim : animData_)
	{
		if (anim.resId == id && anim.animIndex == anim_index)
		{
			anim.attachId = MV1AttachAnim(modelId_, anim.animIndex, anim.resId);
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

	for (auto it = playData_.begin(); it != playData_.end(); )
	{
		if ((*it)->resId == id &&
			(*it)->animIndex == anim_index)
		{
			_Stop(it);

			return;
		}

		++it;
	}
}

void AnimationController::StopAll()
{
	for (auto it = playData_.begin(); it != playData_.end(); )
	{
		it = _Stop(it);
	}
}

std::vector<AnimationController::Animation*>::iterator
AnimationController::_Stop(const std::vector<Animation*>::iterator it)
{
	auto play = *it;

	MV1DetachAnim(modelId_, play->attachId);

	play->attachId = -1;
	play->speed = 1.0f;
	play->time = 0.0f;

	return playData_.erase(it);
}
