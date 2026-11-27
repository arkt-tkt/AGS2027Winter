#include <algorithm>
#include "../Application.h"
#include "../Common/Geometry.h"
#include "../Common/MathUtil.h"
#include "InputManager.h"

InputManager* InputManager::instance_ = nullptr;


InputManager::InputManager(int pad_max)
	:
	PAD_NUM_MAX(MathUtil::Clamp(pad_max, 0, DX_INPUT_PAD16) + PAD_NUM_MIN)
{
	// inputMap_[0]はキーボード操作専用としてデバッグで使う
	inputMap_.resize(PAD_NUM_MAX);

	// gamepads_[0]は未使用とする（inputMap_と数を合わせるため）
	// ゲーム側が想定している最大プレイ人数分の領域を確保しておく
	gamepads_.resize(PAD_NUM_MAX);

	// キー入力の状態を初期化
	nowKey_.fill(false);
	prevKey_.fill(false);

	// DxLib上ではゲームパッドは1から始まる
	for (int i = PAD_NUM_MIN; i < gamepads_.size(); ++i)
	{
		gamepads_[i] = std::make_unique<Gamepad>(i);
	}

	SetMouseVisibility(true);

	InitInputMap();

	Update();
}

void InputManager::Update()
{
	UpdateKey();
	
	UpdateMouse();

	UpdatePad();
}

bool InputManager::Release()
{
	inputMap_.clear();
	gamepads_.clear();

	return true;
}

const bool& InputManager::GetMouseVisibility() const
{
	return mousePtrDisp_;
}

void InputManager::SetMouseVisibility(bool flag)
{
	SetMouseDispFlag(int(flag));

	if (mousePtrDisp_ != flag)
	{
		nowMousePos_ = { Application::RESOLUTION_WIDTH / 2, Application::RESOLUTION_HEIGHT / 2 };

		SetMousePoint(Application::RESOLUTION_WIDTH / 2, Application::RESOLUTION_HEIGHT / 2);
	}

	mousePtrDisp_ = flag;
}

void InputManager::AddorReplaceMap(int padNum, TAGS tag, INPUT_MAP map)
{
	if (!InputBoundCheck(padNum)) return;

	inputMap_[padNum][tag] = map;
}

void InputManager::AddMap(int padNum, TAGS tag, INPUT_MAP map)
{
	if (!InputBoundCheck(padNum)) return;

	if (inputMap_[padNum].find(tag) != inputMap_[padNum].end()) return;

	inputMap_[padNum].emplace(tag, map);
}

void InputManager::ReplaceMap(int padNum, TAGS tag, INPUT_MAP map)
{
	if (!InputBoundCheck(padNum)) return;

	auto it = inputMap_[padNum].find(tag);

	if (it != inputMap_[padNum].end())
		(*it).second = map;
}

void InputManager::ReplaceButtonMap(int padNum, TAGS tag, PAD_MAP_ARRAY replace)
{
	if (!InputBoundCheck(padNum)) return;

	auto it = inputMap_[padNum].find(tag);

	if (it != inputMap_[padNum].end())
		(*it).second.padMap = replace;
}

void InputManager::ReplaceButtonMap(int padNum, TAGS tag, Gamepad::PAD_INPUT replace, size_t index)
{
	if (!InputBoundCheck(padNum)) return;

	auto it = inputMap_[padNum].find(tag);

	if (it != inputMap_[padNum].end())
		(*it).second.padMap[index] = replace;
}

void InputManager::ReplaceKeyMap(int padNum, TAGS tag, KEY_MAP_ARRAY replace)
{
	if (!InputBoundCheck(padNum)) return;

	auto it = inputMap_[padNum].find(tag);

	if (it != inputMap_[padNum].end())
		(*it).second.keyMap = replace;
}

void InputManager::ReplaceKeyMap(int padNum, TAGS tag, int replace, size_t index)
{
	if (!InputBoundCheck(padNum)) return;

	auto it = inputMap_[padNum].find(tag);

	if (it != inputMap_[padNum].end())
		(*it).second.keyMap[index] = replace;
}

int InputManager::CheckNowMap(int padNum, TAGS tag) const
{
	if (!InputBoundCheck(padNum)) return false;

	auto it = inputMap_[padNum].find(tag);

	if (it != inputMap_[padNum].end())
	{
		int p1 = 0, p2 = 0;
		if (padNum > KEYBOARD_NUM)
		{
			p1 = gamepads_[padNum]->NowButton((*it).second.padMap[0]);
			p2 = gamepads_[padNum]->NowButton((*it).second.padMap[1]);
		}
		auto k1 = CheckNowKey((*it).second.keyMap[0]) ? KEY_MOUSE_POWER : 0;
		auto k2 = CheckNowKey((*it).second.keyMap[1]) ? KEY_MOUSE_POWER : 0;

		auto m = CheckNowMouse((*it).second.mouse) ? KEY_MOUSE_POWER : 0;
		return (std::max)({ p1, p2, k1, k2, m });
	}

	return 0;
}

int InputManager::CheckPrevMap(int padNum, TAGS tag) const
{
	if (!InputBoundCheck(padNum)) return false;

	auto it = inputMap_[padNum].find(tag);

	if (it != inputMap_[padNum].end())
	{
		int p1 = 0, p2 = 0;
		if (padNum > KEYBOARD_NUM)
		{
			p1 = gamepads_[padNum]->PrevButton((*it).second.padMap[0]);
			p2 = gamepads_[padNum]->PrevButton((*it).second.padMap[1]);
		}
		auto k1 = CheckPrevKey((*it).second.keyMap[0]) ? KEY_MOUSE_POWER : 0;
		auto k2 = CheckPrevKey((*it).second.keyMap[1]) ? KEY_MOUSE_POWER : 0;

		auto m = CheckPrevMouse((*it).second.mouse) ? KEY_MOUSE_POWER : 0;
		return (std::max)({ p1, p2, k1, k2, m });
	}

	return 0;
}

bool InputManager::CheckDownMap(int padNum, TAGS tag) const
{
	if (!InputBoundCheck(padNum)) return false;

	auto it = inputMap_[padNum].find(tag);

	if (it != inputMap_[padNum].end())
	{
		return !CheckPrevMap(padNum, tag) && CheckNowMap(padNum, tag);
	}

	return false;
}

bool InputManager::CheckUpMap(int padNum, TAGS tag) const
{
	if (!InputBoundCheck(padNum)) return false;

	auto it = inputMap_[padNum].find(tag);

	if (it != inputMap_[padNum].end())
	{
		return CheckPrevMap(padNum, tag) && !CheckNowMap(padNum, tag);
	}

	return false;
}

bool InputManager::CheckNowKey(int d) const
{
	return nowKey_[d];
}

bool InputManager::CheckPrevKey(int d) const
{
	return prevKey_[d];
}

bool InputManager::CheckDownKey(int d) const
{
	return CheckNowKey(d) && !CheckPrevKey(d);
}

bool InputManager::CheckUpKey(int d) const
{
	return !CheckNowKey(d) && CheckPrevKey(d);
}

Vector2 InputManager::GetMousePos() const
{
	return nowMousePos_;
}

Vector2 InputManager::GetMouseMoveLength() const
{
	return Vector2(
		nowMousePos_.x - Application::RESOLUTION_WIDTH / 2,
		nowMousePos_.y - Application::RESOLUTION_HEIGHT / 2);
}

bool InputManager::CheckNowMouse(int mouse_button_num) const
{
	return nowMouse_[mouse_button_num];
}

bool InputManager::CheckPrevMouse(int mouse_button_num) const
{
	return prevMouse_[mouse_button_num];
}

bool InputManager::CheckDownMouse(int mouse_button_num) const
{
	return nowMouse_[mouse_button_num] && !prevMouse_[mouse_button_num];
}

bool InputManager::CheckUpMouse(int mouse_button_num) const
{
	return !nowMouse_[mouse_button_num] && prevMouse_[mouse_button_num];
}

bool InputManager::IsExistPad(int pad_num) const
{
	// 0～9：接続あり、-1：接続なし
	return gamepads_.at(pad_num)->GetPadTypeNumber() >= 0;
}

void InputManager::InitInputMap()
{
	// 入力マップの初期化はここで
	using btn = Gamepad::PAD_INPUT;

	InputManager::PAD_MAP_ARRAY nullBtns = { btn::NONE, btn::NONE };
	InputManager::KEY_MAP_ARRAY nullKeys = { 0x00, 0x00 };

	AddorReplaceMap(0, TAGS::DEBUG,
		{ nullBtns, { KEY_INPUT_INSERT, 0x00 } });

	for (int i = DX_INPUT_PAD1; i <= DX_INPUT_PAD16; ++i)
	{
		if (i > PAD_NUM_MAX)
		{
			break;
		}

		AddorReplaceMap(i, TAGS::MOVE_UP,
			{ { btn::LSTICK_U, btn::NONE }, { KEY_INPUT_W, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::MOVE_DOWN,
			{ { btn::LSTICK_D, btn::NONE }, { KEY_INPUT_S, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::MOVE_LEFT,
			{ { btn::LSTICK_L, btn::NONE }, { KEY_INPUT_A, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::MOVE_RIGHT,
			{ { btn::LSTICK_R, btn::NONE }, { KEY_INPUT_D, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::VIEW_UP,
			{ { btn::RSTICK_U, btn::NONE }, { KEY_INPUT_UP, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::VIEW_DOWN,
			{ { btn::RSTICK_D, btn::NONE }, { KEY_INPUT_DOWN, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::VIEW_LEFT,
			{ { btn::RSTICK_L, btn::NONE }, { KEY_INPUT_LEFT, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::VIEW_RIGHT,
			{ { btn::RSTICK_R, btn::NONE }, { KEY_INPUT_RIGHT, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::VIEW_CHANGE_MODE,
			{ { btn::BUTTON_R3, btn::NONE }, { KEY_INPUT_C, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::VIEW_SWITCH_SIDE,
			{ { btn::DPAD_L, btn::NONE }, { KEY_INPUT_TAB, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::SYSTEM_OK,
			{ { btn::BUTTON_START, btn::NONE }, { KEY_INPUT_RETURN, KEY_INPUT_SPACE }, 0 });

		AddorReplaceMap(i, TAGS::SYSTEM_NG,
			{ { btn::BUTTON_SELECT, btn::NONE }, { KEY_INPUT_ESCAPE, KEY_INPUT_BACK }, 0 });

		AddorReplaceMap(i, TAGS::SYSTEM_PAUSE,
			{ { btn::BUTTON_START, btn::NONE }, { KEY_INPUT_ESCAPE, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::INTERACT,
			{ { btn::BUTTON_RIGHT, btn::NONE }, { KEY_INPUT_E, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::JUMP,
			{ { btn::BUTTON_BOTTOM, btn::NONE }, { KEY_INPUT_SPACE, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::SPRINT,
			{ { btn::BUTTON_L3, btn::NONE }, { KEY_INPUT_LSHIFT, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::ATTACK_MAIN,
			{ { btn::BUTTON_R2, btn::NONE }, { KEY_INPUT_F, 0x00 }, 1 });

		AddorReplaceMap(i, TAGS::ATTACK_SUB,
			{ { btn::BUTTON_L2, btn::NONE }, { KEY_INPUT_Q, 0x00 }, 2 });

		AddorReplaceMap(i, TAGS::RELOAD,
			{ { btn::BUTTON_LEFT, btn::NONE }, { KEY_INPUT_R, 0x00 }, 0 });

		AddorReplaceMap(i, TAGS::ATTACK_CHANGE,
			{ { btn::DPAD_R, btn::NONE }, { KEY_INPUT_V, KEY_INPUT_B }, 3 });

		AddorReplaceMap(i, TAGS::WEAPON_SWITCH_F,
			{ { btn::BUTTON_R1, btn::NONE }, { KEY_INPUT_1, 0x00 }, 10 });

		AddorReplaceMap(i, TAGS::WEAPON_SWITCH_R,
			{ { btn::BUTTON_L1, btn::NONE }, { KEY_INPUT_2, 0x00 }, 9 });
	}
}

void InputManager::UpdateKey()
{
	prevKey_ = nowKey_;
	GetHitKeyStateAll(nowKey_.data());
}

void InputManager::UpdateMouse()
{
	prevMousePos_ = nowMousePos_;

	int intX, intY;
	GetMousePoint(&intX, &intY);
	nowMousePos_ = { static_cast<float>(intX), static_cast<float>(intY) };

	if (!mousePtrDisp_)
	{
		SetMousePoint(Application::RESOLUTION_WIDTH / 2, Application::RESOLUTION_HEIGHT / 2);
	}

	prevMouse_ = nowMouse_;
	nowMouse_ = {};

	// マウスボタンの入力状態を取得（各ビット毎に入力情報が入る）
	auto mb = GetMouseInput();
	
	int index = 0;
	nowMouse_[index] = false;
	for (int digit = MOUSE_INPUT_LEFT; digit <= MOUSE_INPUT_8; digit *= 2)
	{
		nowMouse_[++index] = mb & digit;
	}

	int mr = GetMouseWheelRotVol();
	if (mr > 0)
	{
		nowMouse_[++index] = mr;
		nowMouse_[++index] = 0;
	}
	else if (mr < 0)
	{
		nowMouse_[++index] = 0;
		nowMouse_[++index] = -mr;
	}
}

void InputManager::UpdatePad()
{
	// ゲームパッドの状態を更新
	// ※gamepads_[0]はnullptrなので接触しないように注意
	for (int i = PAD_NUM_MIN; i < gamepads_.size(); ++i)
	{
		gamepads_[i]->Update();
	}
}

bool InputManager::InputBoundCheck(int padNum) const
{
	return padNum >= KEYBOARD_NUM && padNum < PAD_NUM_MAX;
}
