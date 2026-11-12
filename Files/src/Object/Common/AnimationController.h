#pragma once
#include <vector>
#include "../../Manager/ResourceManager.h"

class AnimationController
{
public:
	enum PARTS
	{
		BASE,
		FULL_BODY,
		UPPER_BODY,
		LOWER_BODY,
	};

	struct Animation
	{
		// アニメーション情報
		int resId;		// リソース
		int animIndex;		// データ内アニメーション番号
		float playRate;		// 再生速度
		float totalTime;	// 合計時間
		PARTS parts;		// アニメーションさせる部位

		// アタッチ情報
		int attachId = -1;
		float speed = 1.0f;
		float time = 0.0f;
		bool isLoop = true;
	};

	AnimationController(int model_id);
	~AnimationController();

	void Update();

	// 登録したモデルからアニメーションを追加
	void AddFromMine(int anim_index, float play_rate, PARTS parts = BASE);
	// 別リソースからアニメーションを追加
	void AddFromOther(ResourceManager::SRC res, int anim_index, float play_rate, PARTS parts = BASE);

	// 指定アニメーションを再生
	void Play(ResourceManager::SRC res, int anim_index, float speed = 1.0f, bool is_loop = true);
	// 指定アニメーションを停止
	void Stop(ResourceManager::SRC res, int anim_index);
	// 全アニメーションを停止
	void StopAll();

	// プレイ中のアニメーションデータ配列の取得
	const auto& GetPlayData() { return playData_; }

private:
	int modelId_;

	int rootFrameIdx_;
	int upperFrameIdx_;
	std::pair<int, int> legFrameIdx_;

	std::vector<Animation> animData_;
	std::vector<Animation*> playData_;

	std::vector<Animation*>::iterator
		_Stop(const std::vector<Animation*>::iterator it);

};