#include <DxLib.h>
#include "AutoText.h"

AutoText::AutoText(const char* text, float interval)
{
	ResetText(text, interval);
}

AutoText::~AutoText()
{
}

void AutoText::Update(float delta_time)
{
	timer_ += delta_time;
}

const char* AutoText::GetShowText()
{
	if (timer_ >= interval_)
	{
		timer_ -= interval_;

		// 「添え字」が「模写用テキストの長さ」より小さい
		if (drawCount_ < strlen(baseText_))
		{
			// 次の文字のバイト数を測る
			int bytes = GetCharBytes(DX_CHARCODEFORMAT_SHIFTJIS, &baseText_[drawCount_]);

			// バイト数の分だけforループ
			for (int i = 0; i < bytes; ++i)
			{
				// テキストを写す
				drawText_[drawCount_] = baseText_[drawCount_];
				// 添え字をインクリメントする
				++drawCount_;
			}
		}
		// 「模写完了後のループ用フラグ」が立っている
		else if (loopFlag_)
		{
			// _Dstが示すアドレスから_Sizeバイトまでを_Valで埋める
			memset(drawText_, 0, sizeof(char) * TEXT_LENGTH_MAX);
			// 添え字を0にセット
			drawCount_ = 0;
		}
	}

	// 描画用テキストを返す
	return drawText_;
}

void AutoText::ResetText(const char* text, float interval)
{
	// テキスト長が上限を突破していたら処理を中断
	if ((int)strlen(text) >= TEXT_LENGTH_MAX) return;

	baseText_ = text;

	// _Dstが示すアドレスから_Sizeバイトまでを_Valで埋める
	memset(drawText_, 0, sizeof(char) * TEXT_LENGTH_MAX);

	timer_ = 0.0f;

	interval_ = interval;
	// 添え字を0にセット
	drawCount_ = 0;
}
