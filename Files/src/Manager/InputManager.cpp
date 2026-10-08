#include <algorithm>
#include "../Application.h"
#include "../Common/Geometry.h"
#include "InputManager.h"

InputManager* InputManager::instance_ = nullptr;


InputManager::InputManager(int pad_max)
	:
	PAD_NUM_MAX(std::clamp(pad_max, pad_max + 1, DX_INPUT_PAD16 + 1))
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

	InitInputMap();

	SetMouseDispFlag(FALSE);
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
			bool layoutChange = CheckNeedKeepLayout(padNum, tag);

			p1 = gamepads_[padNum]->NowButton((*it).second.padMap[0], layoutChange);
			p2 = gamepads_[padNum]->NowButton((*it).second.padMap[1], layoutChange);
		}
		auto k1 = CheckNowKey((*it).second.keyMap[0]) ? KEY_MOUSE_POWER : 0;
		auto k2 = CheckNowKey((*it).second.keyMap[1]) ? KEY_MOUSE_POWER : 0;
		return (std::max)({ p1, p2, k1, k2 });
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
			bool keepLayout = CheckNeedKeepLayout(padNum, tag);

			p1 = gamepads_[padNum]->PrevButton((*it).second.padMap[0], keepLayout);
			p2 = gamepads_[padNum]->PrevButton((*it).second.padMap[1], keepLayout);
		}
		auto k1 = CheckPrevKey((*it).second.keyMap[0]) ? KEY_MOUSE_POWER : 0;
		auto k2 = CheckPrevKey((*it).second.keyMap[1]) ? KEY_MOUSE_POWER : 0;
		return (std::max)({ p1, p2, k1, k2 });
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

bool InputManager::CheckNowMouse(int DxLib_MOUSEcode) const
{
	return nowMouse_.at(DxLib_MOUSEcode);
}

bool InputManager::CheckPrevMouse(int DxLib_MOUSEcode) const
{
	return prevMouse_.at(DxLib_MOUSEcode);
}

bool InputManager::CheckDownMouse(int DxLib_MOUSEcode) const
{
	return nowMouse_.at(DxLib_MOUSEcode) && !prevMouse_.at(DxLib_MOUSEcode);
}

bool InputManager::CheckUpMouse(int DxLib_MOUSEcode) const
{
	return !nowMouse_.at(DxLib_MOUSEcode) && prevMouse_.at(DxLib_MOUSEcode);
}

bool InputManager::IsGamepadConnected(int pad_num) const
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
			{ { btn::DPAD_U, btn::LSTICK_U }, { KEY_INPUT_W, KEY_INPUT_UP } });

		AddorReplaceMap(i, TAGS::MOVE_DOWN,
			{ { btn::DPAD_D, btn::LSTICK_D }, { KEY_INPUT_S, KEY_INPUT_DOWN } });

		AddorReplaceMap(i, TAGS::MOVE_LEFT,
			{ { btn::DPAD_L, btn::LSTICK_L }, { KEY_INPUT_A, KEY_INPUT_LEFT } });

		AddorReplaceMap(i, TAGS::MOVE_RIGHT,
			{ { btn::DPAD_R, btn::LSTICK_R }, { KEY_INPUT_D, KEY_INPUT_RIGHT } });

		AddorReplaceMap(i, TAGS::SYSTEM_OK,
			{ { btn::BUTTON_A, btn::NONE }, { KEY_INPUT_RETURN, KEY_INPUT_SPACE } });

		AddorReplaceMap(i, TAGS::SYSTEM_NG,
			{ { btn::BUTTON_B, btn::NONE }, { KEY_INPUT_ESCAPE, KEY_INPUT_BACK } });

		AddorReplaceMap(i, TAGS::SYSTEM_PAUSE,
			{ { btn::BUTTON_START, btn::NONE }, { KEY_INPUT_ESCAPE, 0x00 } });

		AddorReplaceMap(i, TAGS::JUMP,
			{ { btn::BUTTON_A, btn::NONE }, { KEY_INPUT_SPACE, 0x00 } });

		AddorReplaceMap(i, TAGS::SPRINT,
			{ { btn::BUTTON_L3, btn::NONE }, { KEY_INPUT_LSHIFT, 0x00 } });
	}
}

void InputManager::UpdateKey()
{
	prevKey_ = nowKey_;
	GetHitKeyStateAll(nowKey_.data());
}

void InputManager::UpdateMouse()
{
	int intX, intY;
	GetMousePoint(&intX, &intY);
	prevMousePos_ = nowMousePos_;
	nowMousePos_ = { static_cast<float>(intX), static_cast<float>(intY) };
	SetMousePoint(Application::RESOLUTION_WIDTH / 2, Application::RESOLUTION_HEIGHT / 2);

	prevMouse_ = nowMouse_;
	nowMouse_ = {};

	// マウスボタンの入力状態を取得（各ビット毎に入力情報が入る）
	auto mb = GetMouseInput();
	
	// とりあえず標準的なボタン3つ＋サイドボタン2つに対応
	for (int i = MOUSE_INPUT_LEFT; i <= MOUSE_INPUT_5; i *= 2)
	{
		nowMouse_[i] = mb & i;
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

bool InputManager::CheckNeedKeepLayout(int padNum, TAGS tag) const
{
	bool tagHit = false;

	for (auto p : PAD_KEEP_LAYOUT_ARRAY)
	{
		if (p == tag)
		{
			tagHit = true;
			break;
		}
	}

	return tagHit;
}

bool InputManager::InputBoundCheck(int padNum) const
{
	return padNum >= KEYBOARD_NUM && padNum < PAD_NUM_MAX;
}
