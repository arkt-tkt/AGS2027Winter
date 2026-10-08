#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "Fader.h"

Fader::Fader() :
	mode_(FADE_MODE::NONE),
	proc_(PROC::NONE),
	delay_(0.0f),
	time_(0.0f),
	wait_(0.0f),
	delayTimer_(0.0f),
	waitTimer_(0.0f),
	isFadeEnd_(true),
	color_(0x0U),
	alpha_(0.0f)
{
}

Fader::~Fader()
{
}

void Fader::SetFadeMode(FADE_MODE mode, float time, float delay, float wait)
{
	mode_ = mode;
	time_ = time;
	delay_ = delay;
	wait_ = wait;

	delayTimer_ = 0.0f;
	waitTimer_ = 0.0f;

	if (mode_ != FADE_MODE::NONE)
	{
		isFadeEnd_ = false;
	}
}

void Fader::Update()
{
	if (isFadeEnd_) return;

	// デルタタイム
	auto dt = SceneManager::GetInstance().GetDeltaTime();

	switch (mode_)
	{
	default:
	case FADE_MODE::NONE:
		isFadeEnd_ = true;
		break;
	case FADE_MODE::FADE_OUT:
	case FADE_MODE::FADE_IN:
		// 遅延中
		if (delayTimer_ < delay_)
		{
			delayTimer_ += dt;
			proc_ = PROC::DELAY;
			return;
		}

		// フェード中
		if (mode_ == FADE_MODE::FADE_OUT)
		{ // フェードアウト
			if (alpha_ < MAX_ALPHA)
			{
				alpha_ += MAX_ALPHA / time_ * dt;

				if (alpha_ > MAX_ALPHA) alpha_ = MAX_ALPHA;

				proc_ = PROC::FADE;
				return;
			}
		}
		else
		{ // フェードイン
			if (alpha_ > 0.0f)
			{
				alpha_ -= MAX_ALPHA / time_ * dt;

				if (alpha_ < 0.0f) alpha_ = 0.0f;

				proc_ = PROC::FADE;
				return;
			}
		}

		// 待機中
		if (waitTimer_ < wait_)
		{
			waitTimer_ += dt;
			proc_ = PROC::WAIT;
			return;
		}

		// 処理完了
		isFadeEnd_ = true;
		break;
	}
}

void Fader::Draw()
{
	switch (mode_)
	{
	default:
	case FADE_MODE::NONE:
		return;
	case FADE_MODE::FADE_OUT:
	case FADE_MODE::FADE_IN:
		// 描画ブレンドモードを「αブレンド」に設定
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));

		// ウインドウサイズを取得
		int sw = Application::RESOLUTION_WIDTH, sh = Application::RESOLUTION_HEIGHT;

		// ウインドウ全体を単色で描画
		DrawBox(0, 0, sw, sh, color_, true);

		// 描画ブレンドモードを解除
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		break;
	}
}

Fader::FADE_MODE Fader::GetFadeMode() const { return mode_; }

Fader::PROC Fader::GetNowProc() const
{
	// 処理が完了している場合は、直前の処理も無しで返す
	if (mode_ == FADE_MODE::NONE) return PROC::NONE;

	return proc_;
}

void Fader::ForceSetMode(FADE_MODE fmode)
{
	switch (fmode)
	{
	default:
		return;
	case FADE_MODE::FADE_OUT:
		alpha_ = MAX_ALPHA;
		break;
	case FADE_MODE::FADE_IN:
		alpha_ = 0.0F;
		break;
	}

	SetFadeMode(FADE_MODE::NONE, 0U);
}

bool Fader::IsFadeEnd() const { return isFadeEnd_; }
