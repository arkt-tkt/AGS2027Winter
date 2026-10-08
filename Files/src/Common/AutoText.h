#pragma once
#include <array>
#include <string>

class AutoText
{
public:
	// コンストラクタ
	AutoText(const char* text, float interval);
	// デフォルトコンストラクタは使用不可
	inline AutoText() = delete;
	// デストラクタ
	~AutoText();

	// 更新処理
	void Update(float delta_time);

	// 現時点でのテキストを取得
	const char* GetShowText();

	// 描画するテキストを再定義
	void ResetText(const char* text, float interval);

	// 模写完了後のループ用フラグ
	bool loopFlag_ = false;

private:
	// 許容される最大文字列長(複数バイト文字がある事に注意)
	static constexpr int TEXT_LENGTH_MAX = 512;

	float timer_ = 0.0f;
	float interval_ = 0.0f;

	// 複写用テキスト
	const char* baseText_ = "";
	// 描画用テキスト(実際には1バイト単位の配列)
	char drawText_[TEXT_LENGTH_MAX] = {};

	// 次に描画する文字列内のバイト数、添え字として使う
	size_t drawCount_ = 0;

};
