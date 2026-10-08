#pragma once
#include <string>
#include <vector>

class ScoreManager
{
public:
	static void CreateInstance(int pad_max) { if (instance_ == nullptr) instance_ = new ScoreManager(pad_max); }
	static ScoreManager& GetInstance() { return *instance_; }
	static void DeleteInstance() { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

private:
	static ScoreManager* instance_;

	ScoreManager(int pad_max);
	~ScoreManager() {}

	ScoreManager(const ScoreManager&) = delete;
	ScoreManager& operator=(const ScoreManager&) = delete;
	ScoreManager(ScoreManager&&) = delete;
	ScoreManager& operator=(ScoreManager&&) = delete;

public:
	struct ScoreStruct
	{
		unsigned int score = 0u; // 点数
		unsigned int prevScore = 0u; // 直前フレームの点数
		unsigned int extReserve = 0u; // 残機アップ
	};

	bool Init();
	bool Update();
	bool Release();

	// リセット関数
	void HardReset(); // アプリ開始時に使用
	void SoftReset(); // ゲーム開始時に使用
	void StageReset(); // ステージ開始時に使用

	const unsigned int GetScore(size_t index) const;
	const std::string GetScoreString(size_t index) const;
	const std::string GetScoreEmptyString(size_t index) const;

private:
	static constexpr unsigned int SCORE_MAX = 99999999; // スコア最大値
	static constexpr unsigned int SCORE_MAX_DIGIT = 8; // スコア最大桁数
	static constexpr unsigned int SCORE_MIN_DIGIT = 2; // スコア最小桁数（テキスト出力用、不足分はゼロで補う）
	static constexpr unsigned int HIGH_SCORE_INIT = 10000; // ハイスコア初期値

	// 1STと2NDは「決まった点数にスコアが到達した時だけ」条件を満たします
	// EVERYは「決まった点数の分だけスコアを稼ぐたび」条件を満たします
	static constexpr unsigned int EXTEND_1ST_INIT = 30000; // 1回目のエクステンド初期値
	static constexpr unsigned int EXTEND_2ND_INIT = 80000; // 2回目のエクステンド初期値
	static constexpr unsigned int EXTEND_EVERY_INIT = 80000; // それ以降のエクステンド初期値

	const int PAD_MAX;

	std::vector<ScoreStruct> structs_;

	unsigned int highScore_; // ハイスコア
	unsigned int extend1st_;
	unsigned int extend2nd_;
	unsigned int extendEvery_;

	bool ScoreUpdate(size_t index); // 点数範囲チェック＆更新
	void OtherUpdate(size_t index); // 

	bool BoundingCheck(size_t index) const; // 添え字範囲チェック

};

