#pragma once
#include <array>

class Gamepad
{
public:
	enum class PAD_MODEL
	{
		NONE = -1,

		NINTENDO,
		PLAY_STATION,
		XBOX,

		END
	};

	// 一律で規格化した生のボタン入力
	enum class PAD_INPUT
	{
		NONE,

		LSTICK_L, // 左ジョイスティック・左
		LSTICK_R, // 左ジョイスティック・右
		LSTICK_U, // 左ジョイスティック・上
		LSTICK_D, // 左ジョイスティック・下

		RSTICK_L, // 右ジョイスティック・左
		RSTICK_R, // 右ジョイスティック・右
		RSTICK_U, // 右ジョイスティック・上
		RSTICK_D, // 右ジョイスティック・下

		DPAD_L, // 方向キー・左
		DPAD_R, // 方向キー・右
		DPAD_U, // 方向キー・上
		DPAD_D, // 方向キー・下

		BUTTON_A, // Aボタン
		BUTTON_B, // Bボタン
		BUTTON_X, // Xボタン
		BUTTON_Y, // Yボタン

		BUTTON_SELECT,
		BUTTON_START,
		
		BUTTON_L1,
		BUTTON_R1,
		BUTTON_L2,
		BUTTON_R2,
		BUTTON_L3,
		BUTTON_R3,

		END
	};

	static constexpr const char* NOT_ENOUGH_BUTTON_NUM =
		"警告：接続しているゲームパッドのボタンの数が不足しています";

	static constexpr int PAD_INPUT_DIR_SIZE = 12;

	static constexpr const char* PAD_INPUT_DIR_NAME[PAD_INPUT_DIR_SIZE]
	{
		"Lスティック左",
		"Lスティック右",
		"Lスティック上",
		"Lスティック下",

		"Rスティック左",
		"Rスティック右",
		"Rスティック上",
		"Rスティック下",

		"方向キー左",
		"方向キー右",
		"方向キー上",
		"方向キー下"
	};

	static constexpr int PAD_INPUT_BTN_SIZE = 12;

	static constexpr const char* PAD_INPUT_BTN_NAME[PAD_INPUT_BTN_SIZE]
	{
		"Aボタン",
		"Bボタン",
		"Xボタン",
		"Yボタン",
		"SELECTボタン",
		"STARTボタン",
		"Lボタン",
		"Rボタン",
		"ZLボタン",
		"ZRボタン",
		"Lスティックボタン",
		"Rスティックボタン"
	};

	static constexpr const char* PAD_INPUT_BTN_NAME_PS[PAD_INPUT_BTN_SIZE]
	{
		"○ボタン",
		"×ボタン",
		"△ボタン",
		"□ボタン",
		"SELECTボタン",
		"STARTボタン",
		"L1ボタン",
		"R1ボタン",
		"L2ボタン",
		"R2ボタン",
		"L3ボタン",
		"R3ボタン"
	};

	static constexpr const char* PAD_INPUT_BTN_NAME_XBOX[PAD_INPUT_BTN_SIZE]
	{
		"Aボタン",
		"Bボタン",
		"Xボタン",
		"Yボタン",
		"SHAREボタン",
		"OPTIONボタン",
		"LBボタン",
		"RBボタン",
		"LTボタン",
		"RTボタン",
		"LSボタン",
		"RSボタン"
	};

	Gamepad(int pad_num);
	~Gamepad();

	void Update();

	int NowButton(PAD_INPUT in, bool keepLayout = false) const;
	int PrevButton(PAD_INPUT in, bool keepLayout = false) const;
	bool DownButton(PAD_INPUT in, bool keepLayout = false) const;
	bool UpButton(PAD_INPUT in, bool keepLayout = false) const;

	// padTypeNum_を返す
	int GetPadTypeNumber() const;
	// padTypeNum_からPAD_MODELを返す
	int GetPadTypeModel() const;

	bool IsNintendoJoypad() const;

private:
	// DirectInputのスティック入力最大値
	static constexpr double DINPUT_STICK_MAX = 1000;
	// XInputのスティック入力最大値
	static constexpr double XINPUT_STICK_MAX = SHRT_MAX;
	// XInputのスティック入力値をDirectInputに代入するための掛け率
	static constexpr double XINPUT_STICK_MULT = DINPUT_STICK_MAX / XINPUT_STICK_MAX;

	// スティック入力のデッドゾーン割合
	static constexpr double DEADZONE_PERCENTILE = 0.25;
	// DirectInputのデッドゾーン実数値
	static constexpr int DINPUT_DEADZONE = static_cast<int>(DINPUT_STICK_MAX * DEADZONE_PERCENTILE);
	// XInputのデッドゾーン実数値
	static constexpr int XINPUT_DEADZONE = static_cast<int>(XINPUT_STICK_MAX * DEADZONE_PERCENTILE);

	// このGamepadの内部番号
	const int PAD_NUM;

	// DxLib::GetJoypadType関数で得られたタイプ
	int padTypeNum_;

	using BUTTON_STATE_ARRAY = std::array<int, static_cast<size_t>(PAD_INPUT::END)>;
	BUTTON_STATE_ARRAY nowButton_;
	BUTTON_STATE_ARRAY prevButton_;

	void LayoutConversion(PAD_INPUT& in) const;

};

