#pragma once
#include <vector>
#include "../../Manager/ResourceManager.h"

class AnimationController
{
public:
	enum PRIORITY
	{
		BASE,
		MIDDLE,
		TOP,
	};

	struct AnimationData
	{
		// アニメーション情報
		int modelId;		// リソース
		int animIndex;		// データ内アニメーション番号
		float playRate;		// 再生速度
		float totalTime;	// 合計時間
		PRIORITY priority;	// アニメーションの優先度

		// アタッチ情報
		int attachId = -1;
		float speed = 1.0f;
		float time = 0.0f;
		bool isLoop = true;
	};

	AnimationController(int model_id);
	~AnimationController();

	void Update();

	void AddFromMine(int anim_index, float play_rate, PRIORITY priority = PRIORITY::BASE);
	void AddFromOther(ResourceManager::SRC res, int anim_index, float play_rate, PRIORITY priority = PRIORITY::BASE);

	void Play(ResourceManager::SRC res, int anim_index, float speed = 1.0f, bool is_loop = true);
	void Stop(ResourceManager::SRC res, int anim_index);

private:
	int modelId_;

	int rootFrameIdx_;
	int upperFrameIdx_;
	std::pair<int, int> legFrameIdx_;

	std::vector<AnimationData> animData_;
	std::vector<AnimationData*> playData_;

	std::vector<AnimationData*>::iterator
		_Stop(const std::vector<AnimationData*>::iterator it);

};