#pragma once

class Fader {
public:
	enum class FADE_MODE {
		NONE,
		FADE_OUT,
		FADE_IN,
	};

	enum class PROC {
		NONE,
		DELAY,
		FADE,
		WAIT,
	};

	Fader();
	~Fader();

	void SetFadeMode(FADE_MODE mode, float time, float delay = 0.0f, float wait = 0.0f);
	void Update();
	void Draw();

	// 現在のフェードモードを取得する
	FADE_MODE GetFadeMode() const;
	// 現在のフェード処理の状態を取得する
	PROC GetNowProc() const;
	// 強制的にフェードモードを設定する
	void ForceSetMode(FADE_MODE);
	// フェード処理が完了しているか
	bool IsFadeEnd() const;

private:
	static constexpr float MAX_ALPHA = 255.0f;

	FADE_MODE mode_;
	PROC proc_;
	float time_;
	float delay_;
	float wait_;

	float delayTimer_;
	float waitTimer_;
	bool isFadeEnd_;

	unsigned int color_;
	float alpha_;
};

