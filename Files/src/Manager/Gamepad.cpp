#include <DxLib.h>
#include "../Common/MathUtil.h"
#include "Gamepad.h"

Gamepad::Gamepad(int pad_num)
	:
	PAD_NUM(pad_num)
{
	nowButton_.fill(0);
	prevButton_.fill(0);
	
	padTypeNum_ = GetJoypadType(PAD_NUM);
}

Gamepad::~Gamepad()
{
}

void Gamepad::Update()
{
	padTypeNum_ = GetJoypadType(PAD_NUM);

	// 未接続状態なら処理の無駄なので早期return
	if (padTypeNum_ == -1) return;

	// DirectInput
	DINPUT_JOYSTATE dIn = {};
	GetJoypadDirectInputState(PAD_NUM, &dIn);

	// XInput（XBOX対応コントローラー専用）
	XINPUT_STATE xIn = {};
	bool xFlg = CheckJoypadXInput(PAD_NUM);
	if (xFlg) GetJoypadXInputState(PAD_NUM, &xIn);

	// 更新
	prevButton_ = nowButton_;
	nowButton_.fill(0);

	// 左スティック
	if (!xFlg)
	{
		// DirectInput
		// X軸
		if (dIn.X < -DINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::LSTICK_L] = std::abs(dIn.X);
		else if (dIn.X > DINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::LSTICK_R] = std::abs(dIn.X);
		// Y軸
		if (dIn.Y < -DINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::LSTICK_U] = std::abs(dIn.Y);
		else if (dIn.Y > DINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::LSTICK_D] = std::abs(dIn.Y);
	}
	else
	{ // XInput
		// X軸
		if (xIn.ThumbLX < -XINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::LSTICK_L] = (int)std::abs(xIn.ThumbLX * XINPUT_STICK_MULT);
		else if (xIn.ThumbLX > XINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::LSTICK_R] = (int)std::abs(xIn.ThumbLX * XINPUT_STICK_MULT);
		// Y軸
		if (xIn.ThumbLY > XINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::LSTICK_U] = (int)std::abs(xIn.ThumbLY * XINPUT_STICK_MULT);
		else if (xIn.ThumbLY < -XINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::LSTICK_D] = (int)std::abs(xIn.ThumbLY * XINPUT_STICK_MULT);
	}

	// 右スティック
	if (!xFlg)
	{
		// DirectInput
		// DirectInputは、右スティック専用のパラメーターを持たない（RxやRyなどは本来、スティックの回転量である）ため
		// ここに記述してあるプログラムでは反応しない場合がある

		// X軸
		if (dIn.Rx < -DINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::RSTICK_L] = std::abs(dIn.Rx);
		else if (dIn.X > DINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::RSTICK_R] = std::abs(dIn.Rx);
		// Y軸
		if (dIn.Ry < -DINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::RSTICK_U] = std::abs(dIn.Ry);
		else if (dIn.Ry > DINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::RSTICK_D] = std::abs(dIn.Ry);
	}
	else
	{
		// XInput
		// X軸
		if (xIn.ThumbRX < -XINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::RSTICK_L] = (int)std::abs(xIn.ThumbRX * XINPUT_STICK_MULT);
		else if (xIn.ThumbRX > XINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::RSTICK_R] = (int)std::abs(xIn.ThumbRX * XINPUT_STICK_MULT);
		// Y軸
		if (xIn.ThumbRY < -XINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::RSTICK_U] = (int)std::abs(xIn.ThumbRY * XINPUT_STICK_MULT);
		else if (xIn.ThumbRY > XINPUT_DEADZONE)
			nowButton_[(int)PAD_INPUT::RSTICK_D] = (int)std::abs(xIn.ThumbRY * XINPUT_STICK_MULT);
	}

	// ハットスイッチ／十字キー
	if (dIn.POV[0] != 0xFFFFFFFFU)
	{
		// POVは、出力に弧度×100のunsigned int型1つを用いているため
		// 取り出した値を100で割った後、弧度からラジアンに変換すると良い
		double rad = MathUtil::DegToRad(dIn.POV[0] / 100.0);
		// 計算時の誤差に対するマージン
		double margin = 0.01;

		// 本来はx = cos、y = sinだが、0が上から始まるので
		// cosとsinを入れ替えた方が良い
		double sinRet = std::sin(rad);
		double cosRet = std::cos(rad);

		// X軸
		if (sinRet < -margin)
			nowButton_[(int)PAD_INPUT::DPAD_L] = DINPUT_STICK_MAX;
		else if (sinRet > margin)
			nowButton_[(int)PAD_INPUT::DPAD_R] = DINPUT_STICK_MAX;
		// Y軸
		if (cosRet > margin)
			nowButton_[(int)PAD_INPUT::DPAD_U] = DINPUT_STICK_MAX;
		else if (cosRet < -margin)
			nowButton_[(int)PAD_INPUT::DPAD_D] = DINPUT_STICK_MAX;
	}

	// Buttons（ボタン）
	if (!xFlg)
	{ // DirectInput
		auto btnNum = GetJoypadButtonNum(PAD_NUM);

		switch (GetPadTypeModel())
		{
		case static_cast<int>(Gamepad::PAD_MODEL::NINTENDO):
			if (btnNum >= 12)
			{
				nowButton_[(int)PAD_INPUT::BUTTON_A] = dIn.Buttons[0]; // A
				nowButton_[(int)PAD_INPUT::BUTTON_B] = dIn.Buttons[1]; // B
				nowButton_[(int)PAD_INPUT::BUTTON_X] = dIn.Buttons[2]; // X
				nowButton_[(int)PAD_INPUT::BUTTON_Y] = dIn.Buttons[3]; // Y
				nowButton_[(int)PAD_INPUT::BUTTON_SELECT] = dIn.Buttons[8];
				nowButton_[(int)PAD_INPUT::BUTTON_START] = dIn.Buttons[9];
				nowButton_[(int)PAD_INPUT::BUTTON_L1] = dIn.Buttons[4];
				nowButton_[(int)PAD_INPUT::BUTTON_R1] = dIn.Buttons[5];
				nowButton_[(int)PAD_INPUT::BUTTON_L2] = dIn.Buttons[6];
				nowButton_[(int)PAD_INPUT::BUTTON_R2] = dIn.Buttons[7];
				nowButton_[(int)PAD_INPUT::BUTTON_L3] = dIn.Buttons[10];
				nowButton_[(int)PAD_INPUT::BUTTON_R3] = dIn.Buttons[11];
			}
			else if (btnNum >= 10)
			{
				nowButton_[(int)PAD_INPUT::BUTTON_A] = dIn.Buttons[0]; // A
				nowButton_[(int)PAD_INPUT::BUTTON_B] = dIn.Buttons[1]; // B
				nowButton_[(int)PAD_INPUT::BUTTON_X] = dIn.Buttons[2]; // X
				nowButton_[(int)PAD_INPUT::BUTTON_Y] = dIn.Buttons[3]; // Y
				nowButton_[(int)PAD_INPUT::BUTTON_SELECT] = dIn.Buttons[8];
				nowButton_[(int)PAD_INPUT::BUTTON_START] = dIn.Buttons[9];
				nowButton_[(int)PAD_INPUT::BUTTON_L1] = dIn.Buttons[4];
				nowButton_[(int)PAD_INPUT::BUTTON_R1] = dIn.Buttons[5];
				nowButton_[(int)PAD_INPUT::BUTTON_L2] = dIn.Buttons[6];
				nowButton_[(int)PAD_INPUT::BUTTON_R2] = dIn.Buttons[7];
			}
			else if (btnNum >= 8)
			{
				nowButton_[(int)PAD_INPUT::BUTTON_A] = dIn.Buttons[0]; // A
				nowButton_[(int)PAD_INPUT::BUTTON_B] = dIn.Buttons[1]; // B
				nowButton_[(int)PAD_INPUT::BUTTON_X] = dIn.Buttons[2]; // X
				nowButton_[(int)PAD_INPUT::BUTTON_Y] = dIn.Buttons[3]; // Y
				nowButton_[(int)PAD_INPUT::BUTTON_SELECT] = dIn.Buttons[6];
				nowButton_[(int)PAD_INPUT::BUTTON_START] = dIn.Buttons[7];
				nowButton_[(int)PAD_INPUT::BUTTON_L1] = dIn.Buttons[4];
				nowButton_[(int)PAD_INPUT::BUTTON_R1] = dIn.Buttons[5];
			}
			break;
		case static_cast<int>(Gamepad::PAD_MODEL::PLAY_STATION):
			nowButton_[(int)PAD_INPUT::BUTTON_A] = dIn.Buttons[1]; // ×
			nowButton_[(int)PAD_INPUT::BUTTON_B] = dIn.Buttons[2]; // ○
			nowButton_[(int)PAD_INPUT::BUTTON_X] = dIn.Buttons[0]; // □
			nowButton_[(int)PAD_INPUT::BUTTON_Y] = dIn.Buttons[3]; // △
			nowButton_[(int)PAD_INPUT::BUTTON_SELECT] = dIn.Buttons[8];
			nowButton_[(int)PAD_INPUT::BUTTON_START] = dIn.Buttons[9];
			nowButton_[(int)PAD_INPUT::BUTTON_L1] = dIn.Buttons[4];
			nowButton_[(int)PAD_INPUT::BUTTON_R1] = dIn.Buttons[5];
			nowButton_[(int)PAD_INPUT::BUTTON_L2] = dIn.Buttons[6];
			nowButton_[(int)PAD_INPUT::BUTTON_R2] = dIn.Buttons[7];
			nowButton_[(int)PAD_INPUT::BUTTON_L3] = dIn.Buttons[10];
			nowButton_[(int)PAD_INPUT::BUTTON_R3] = dIn.Buttons[11];
			break;
		default:
			break;
		}
	}
	else
	{ // XInput
		// Buttons[0～3]は方向キーに割り当てられている
		nowButton_[(int)PAD_INPUT::BUTTON_A] = xIn.Buttons[12]; // A
		nowButton_[(int)PAD_INPUT::BUTTON_B] = xIn.Buttons[13]; // B
		nowButton_[(int)PAD_INPUT::BUTTON_X] = xIn.Buttons[14]; // X
		nowButton_[(int)PAD_INPUT::BUTTON_Y] = xIn.Buttons[15]; // Y
		nowButton_[(int)PAD_INPUT::BUTTON_SELECT] = xIn.Buttons[5];
		nowButton_[(int)PAD_INPUT::BUTTON_START] = xIn.Buttons[4];
		nowButton_[(int)PAD_INPUT::BUTTON_L1] = xIn.Buttons[8];
		nowButton_[(int)PAD_INPUT::BUTTON_R1] = xIn.Buttons[9];
		nowButton_[(int)PAD_INPUT::BUTTON_L2] = xIn.LeftTrigger;
		nowButton_[(int)PAD_INPUT::BUTTON_R2] = xIn.RightTrigger;
		nowButton_[(int)PAD_INPUT::BUTTON_L3] = xIn.Buttons[6];
		nowButton_[(int)PAD_INPUT::BUTTON_R3] = xIn.Buttons[7];
	}
}

int Gamepad::NowButton(PAD_INPUT in, bool keepLayout) const
{
	if (!keepLayout && IsNintendoJoypad())
	{
		LayoutConversion(in);
	}

	return nowButton_[(int)in];
}

int Gamepad::PrevButton(PAD_INPUT in, bool keepLayout) const
{
	if (!keepLayout && IsNintendoJoypad())
	{
		LayoutConversion(in);
	}

	return prevButton_[(int)in];
}

bool Gamepad::DownButton(PAD_INPUT in, bool keepLayout) const
{
	if (!keepLayout && IsNintendoJoypad())
	{
		LayoutConversion(in);
	}

	return NowButton(in, true) && !PrevButton(in, true);
}

bool Gamepad::UpButton(PAD_INPUT in, bool keepLayout) const
{
	if (!keepLayout && IsNintendoJoypad())
	{
		LayoutConversion(in);
	}

	return !NowButton(in, true) && PrevButton(in, true);
}

int Gamepad::GetPadTypeNumber() const
{
	return padTypeNum_;
}

int Gamepad::GetPadTypeModel() const
{
	Gamepad::PAD_MODEL ret = PAD_MODEL::NONE;

	switch (padTypeNum_)
	{
	case DX_PADTYPE_OTHER: // 0
	case DX_PADTYPE_SWITCH_JOY_CON_L: // 6
	case DX_PADTYPE_SWITCH_JOY_CON_R: // 7
	case DX_PADTYPE_SWITCH_PRO_CTRL: // 8
	case DX_PADTYPE_SWITCH_HORI_PAD: // 9
		ret = PAD_MODEL::NINTENDO;
		break;
	case DX_PADTYPE_DUAL_SHOCK_3: // 3
	case DX_PADTYPE_DUAL_SHOCK_4: // 4
	case DX_PADTYPE_DUAL_SENSE: // 5
		ret = PAD_MODEL::PLAY_STATION;
		break;
	case DX_PADTYPE_XBOX_360: // 1
	case DX_PADTYPE_XBOX_ONE: // 2
		ret = PAD_MODEL::XBOX;
		break;
	default:
		ret = PAD_MODEL::NONE;
		break;
	}

	return 0;
}

bool Gamepad::IsNintendoJoypad() const
{
	return GetPadTypeModel() == static_cast<int>(PAD_MODEL::NINTENDO);
}

void Gamepad::LayoutConversion(Gamepad::PAD_INPUT& in) const
{
	switch (in)
	{
	case PAD_INPUT::BUTTON_A:
		in = PAD_INPUT::BUTTON_B;
		break;
	case PAD_INPUT::BUTTON_B:
		in = PAD_INPUT::BUTTON_A;
		break;
	case PAD_INPUT::BUTTON_X:
		in = PAD_INPUT::BUTTON_Y;
		break;
	case PAD_INPUT::BUTTON_Y:
		in = PAD_INPUT::BUTTON_X;
		break;
	}
}
