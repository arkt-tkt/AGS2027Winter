#include <DxLib.h>
#include <cmath>
#include "ScoreManager.h"

ScoreManager* ScoreManager::instance_ = nullptr;

ScoreManager::ScoreManager(int pad_max) :
	PAD_MAX(pad_max)
{
	// まず初期化
	HardReset();
}

bool ScoreManager::Init()
{
	// 外部ファイルからの読み込み等

	return true;
}

bool ScoreManager::Update()
{
	bool ret = false;

	for (size_t i = 0u; i < structs_.size(); ++i)
	{
		if (ScoreUpdate(i)) ret = true;
	}

	return ret;
}

bool ScoreManager::Release()
{
	return true;
}

void ScoreManager::HardReset()
{
	SoftReset();

	highScore_ = 0u;
}

void ScoreManager::SoftReset()
{
	StageReset();

	for (size_t i = 0; i < structs_.size(); ++i)
	{
		structs_[i] = {};
	}
}

void ScoreManager::StageReset()
{
	for (size_t i = 0; i < structs_.size(); ++i)
	{
	}
}

const unsigned int ScoreManager::GetScore(size_t index) const
{
	if (!BoundingCheck(index)) return 0u;

	return structs_[index].score;
}

const std::string ScoreManager::GetScoreString(size_t index) const
{
	if (!BoundingCheck(index)) return std::string();

	// 数値から文字列に変換
	std::string ret = std::to_string(structs_[index].score);

	// 桁数が足りない場合、先頭に0を追加する
	while (ret.length() < SCORE_MIN_DIGIT)
	{
		ret = "0" + ret;
	}
	// 最大桁数まで、先頭に空白を追加する
	while (ret.length() < SCORE_MAX_DIGIT)
	{
		ret = " " + ret;
	}

	return ret;
}

const std::string ScoreManager::GetScoreEmptyString(size_t index) const
{
	if (!BoundingCheck(index)) return std::string();

	// 数値から文字列に変換
	std::string scTxt = std::to_string(structs_[index].score);
	// 返り値用のテキスト
	std::string ret;

	// 現在のスコアの桁数まで、先頭に空白を追加する
	while (ret.length() < scTxt.length())
	{
		ret += " ";
	}
	// 最大桁数まで、先頭に0を追加する
	while (ret.length() < SCORE_MAX_DIGIT)
	{
		ret = "0" + ret;
	}

	return ret;
}

bool ScoreManager::ScoreUpdate(size_t index)
{
	if (!BoundingCheck(index)) return false;

	bool ret = false;

	// スコアオーバーフロー防止
	if (structs_[index].score > SCORE_MAX)
		structs_[index].score = SCORE_MAX;

	// ハイスコア更新
	if (structs_[index].score > highScore_)
		highScore_ = structs_[index].score;

	// エクステンド
	// 1回目
	if (structs_[index].score >= extend1st_ &&
		structs_[index].prevScore < extend1st_)
	{
		++structs_[index].extReserve;
		ret = true;
	}

	// 2回目以降
	if (structs_[index].score >= extend2nd_)
	{
		// 2回目
		if (structs_[index].prevScore < extend2nd_)
		{
			++structs_[index].extReserve;
			ret = true;
		}

		// 3回目以降
		int n1 = (structs_[index].prevScore - extend2nd_) / extendEvery_;
		int n2 = (structs_[index].score - extend2nd_) / extendEvery_;

		if (!(n1 < 0 || n2 < 0))
		{
			int res = n2 - n1;
			if (res > 0)
			{
				structs_[index].extReserve += res;
				ret = true;
			}
		}
	}

	// スコア更新
	structs_[index].prevScore = structs_[index].score;

	return ret;
}

void ScoreManager::OtherUpdate(size_t index)
{


	return;
}

bool ScoreManager::BoundingCheck(size_t index) const
{
	if (index >= structs_.size()) return false;

	return true;
}
