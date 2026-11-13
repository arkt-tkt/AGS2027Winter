#include <DxLib.h>
#include <map>
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

	using namespace std;

	PARTS upperLv = FULL_BODY, lowerLv = FULL_BODY;
	float upperPow = 0.0f, lowerPow = 0.0f;
	map<PARTS, pair<float, vector<AnimationController::Animation*>>> upperArr, lowerArr;

	// ※現状、速度は二の次でアニメーションのブレンドと遷移に全能力を注いで制作しています

	for (auto it = playData_.begin(); it != playData_.end(); )
	{
		auto play = *it;

		// ブレンド率をゼロに
		MV1SetAttachAnimBlendRate(modelId_, play->attachId, 0.0f);

		// 影響率を調整
		if (play->power < play->maxPower)
		{
			play->power += POWER_ADJUST_PER_SECOND * dt;
			play->power = (std::min)(play->power, play->maxPower);
		}
		else if (play->power > play->maxPower)
		{
			play->power -= POWER_ADJUST_PER_SECOND * dt;
			play->power = (std::max)(play->power, play->maxPower);

			if (play->power <= 0.0f)
			{
				it = _Detach(it);
				continue;
			}
		}

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
			else if (play->speed != 0.0f)
			{
				_Stop(it);
				play->speed = 0.0f;
			}
		}

		// 再生時間をセット
		MV1SetAttachAnimTime(modelId_, play->attachId, play->time);

		// リストに追加
		if (play->parts != LOWER_BODY)
		{
			// ブレンド倍率を加算していく(これは後で使う)
			upperArr[play->parts].first += play->power;
			// プレイデータのポインタを格納
			upperArr[play->parts].second.emplace_back(play);
		}

		if (play->parts != UPPER_BODY)
		{
			// ブレンド倍率を加算していく(これは後で使う)
			lowerArr[play->parts].first += play->power;
			// プレイデータのポインタを格納
			lowerArr[play->parts].second.emplace_back(play);
		}

		// 次の要素へ
		++it;
	}

	// 倍率
	float powerMult = 1.0f;

	// 逆方向から
	for (auto rit = upperArr.rbegin(); rit != upperArr.rend(); ++rit)
	{
		auto& [parts, pair] = *rit;

		for (auto& index : pair.second)
		{
			// ブレンド用に、倍率が0.0～1.0になるように調整
			float newPower = pair.first < 1.0f ? index->power : index->power / pair.first;

			// 背骨から先の全てに適用
			MV1SetAttachAnimBlendRateToFrame(modelId_, index->attachId, upperFrameIdx_,
				newPower * powerMult, TRUE);
		}

		powerMult -= pair.first;

		if (powerMult <= 0.0f) break;
	}

	// 倍率リセット
	powerMult = 1.0f;

	// やはり、逆方向から
	for (auto rit = lowerArr.rbegin(); rit != lowerArr.rend(); ++rit)
	{
		auto& [parts, pair] = *rit;

		for (auto& index : pair.second)
		{
			// ブレンド用に、倍率が0.0～1.0になるように調整
			float newPower = pair.first < 1.0f ? index->power : index->power / pair.first;

			// 腰と両脚に適用
			MV1SetAttachAnimBlendRateToFrame(modelId_, index->attachId, rootFrameIdx_,
				newPower * powerMult, FALSE);
			MV1SetAttachAnimBlendRateToFrame(modelId_, index->attachId, legFrameIdx_.first,
				newPower * powerMult, TRUE);
			MV1SetAttachAnimBlendRateToFrame(modelId_, index->attachId, legFrameIdx_.second,
				newPower * powerMult, TRUE);
		}

		powerMult -= pair.first;

		if (powerMult <= 0.0f) break;
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

void AnimationController::Play(ResourceManager::SRC res, int anim_index, bool is_loop, float power, float speed)
{
	int id = ResourceManager::GetInstance().Load(res).handleId_;

	if (power < 0.0f || speed == 0.0f) return;

	for (auto play : playData_)
	{
		if (play->resId == id && play->animIndex == anim_index)
		{
			play->maxPower = power;
			play->speed = speed;

			return;
		}
	}

	for (auto& anim : animData_)
	{
		if (anim.resId == id && anim.animIndex == anim_index)
		{
			anim.attachId = MV1AttachAnim(modelId_, anim.animIndex, anim.resId);
			anim.maxPower = power;
			anim.speed = speed;
			anim.time = speed > 0.0f ? 0.0f : anim.totalTime;
			anim.isLoop = is_loop;

			playData_.push_back(&anim);

			return;
		}
	}
}

void AnimationController::PlayInSeconds(ResourceManager::SRC res, int anim_index, float seconds, float power)
{
	int id = ResourceManager::GetInstance().Load(res).handleId_;

	for (auto play : playData_)
	{
		if (play->resId == id && play->animIndex == anim_index)
		{
			return;
		}
	}

	for (auto& anim : animData_)
	{
		if (anim.resId == id && anim.animIndex == anim_index)
		{
			anim.attachId = MV1AttachAnim(modelId_, anim.animIndex, anim.resId);
			anim.maxPower = power;
			anim.speed = anim.totalTime / anim.playRate / seconds; // 再生時間を指定秒数に合わせて調整
			anim.isLoop = false;

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
	for (auto it = playData_.begin(); it != playData_.end(); ++it)
	{
		_Stop(it);
	}
}

void AnimationController::_Stop(const std::vector<Animation*>::iterator it)
{
	(*it)->maxPower = 0.0f;
}

std::vector<AnimationController::Animation*>::iterator
AnimationController::_Detach(const std::vector<Animation*>::iterator it)
{
	auto play = *it;

	MV1DetachAnim(modelId_, play->attachId);

	play->attachId = -1;
	play->speed = 1.0f;
	play->time = 0.0f;

	return playData_.erase(it);
}
