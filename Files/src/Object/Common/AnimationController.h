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
		int resId;			// リソース
		int animIndex;		// データ内アニメーション番号
		float playRate;		// 再生速度
		float totalTime;	// 合計時間
		PARTS parts;		// アニメーションさせる部位

		// アタッチ情報
		int attachId = -1;
		float power = 0.0f;
		float maxPower = 1.0f;
		float speed = 1.0f;
		float time = 0.0f;
		bool isLoop = true;
	};

	AnimationController(int model_id);
	~AnimationController();

	// 更新処理　人力ほぼ100%から繰り出されし華麗なるアニメーションミックスをその目で確かめてみろ！
	void Update();

	// 登録したモデルからアニメーションを追加
	void AddFromMine(int anim_index, float play_rate, PARTS parts = BASE);
	// 別リソースからアニメーションを追加
	void AddFromOther(ResourceManager::SRC res, int anim_index, float play_rate, PARTS parts = BASE);

	// 指定アニメーションを再生
	void Play(ResourceManager::SRC res, int anim_index, bool is_loop = true, float power = 1.0f, float speed = 1.0f);
	// 指定アニメーションを指定秒数に収まるように再生(ループ無し、逆再生無し)
	void PlayInSeconds(ResourceManager::SRC res, int anim_index, float seconds, float power = 1.0f);
	// 指定アニメーションを停止
	void Stop(ResourceManager::SRC res, int anim_index);
	// 全アニメーションを停止
	void StopAll();

	// プレイ中のアニメーションデータ配列の取得
	const auto& GetPlayData() { return playData_; }

private:
	static constexpr float POWER_ADJUST_PER_SECOND = 3.3f;

	int modelId_;

	int rootFrameIdx_;
	int upperFrameIdx_;
	std::pair<int, int> legFrameIdx_;

	std::vector<Animation> animData_;
	std::vector<Animation*> playData_;

	// アニメーションを徐々に止める
	void _Stop(const std::vector<Animation*>::iterator it);

	// アニメーションをデタッチ(強制停止にも使える)
	std::vector<Animation*>::iterator
		_Detach(const std::vector<Animation*>::iterator it);

};