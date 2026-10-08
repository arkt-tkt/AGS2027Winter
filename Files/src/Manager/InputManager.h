#pragma once
#include <array>
#include <map>
#include <memory>
#include <vector>
#include <DxLib.h>
#include "../Common/Geometry.h"
#include "Gamepad.h"

class InputManager
{
public:
	/// シングルトン
	static void CreateInstance(int pad_max) { if (instance_ == nullptr) instance_ = new InputManager(pad_max); }
	static InputManager& GetInstance() { return *instance_; }
	static void DeleteInstance() { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

private:
	/// シングルトン
	static InputManager* instance_;

	InputManager(int pad_max);
	~InputManager() {}

	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

public:
	enum class TAGS
	{
		NONE,

		DEBUG,
		
		SYSTEM_OK,
		SYSTEM_NG,
		SYSTEM_PAUSE,

		MOVE_BACK,
		MOVE_DOWN,
		MOVE_FRONT,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_UP,

		ATTACK_MAIN,
		ATTACK_SUB,
		ATTACK_SPECIAL,

		JUMP,
		SPRINT,
	};

	// ここに書かれているタグに登録されたボタンは、
	// 任天堂系ゲームパッド用再配置処理の影響を受けない
	static constexpr TAGS PAD_KEEP_LAYOUT_ARRAY[]
	{
		TAGS::SYSTEM_OK,
		TAGS::SYSTEM_NG,
		TAGS::SYSTEM_PAUSE
	};

	using KEY_STATE_ARRAY = std::array<char, 256ULL>;
	using MOUSE_STATE_MAP = std::map<int, bool>;

	using PAD_MAP_ARRAY = std::array<Gamepad::PAD_INPUT, 2ULL>;
	using KEY_MAP_ARRAY = std::array<int, 2ULL>;

	struct INPUT_MAP
	{
		PAD_MAP_ARRAY padMap;
		KEY_MAP_ARRAY keyMap;
	};

	void Update();
	bool Release();

	void AddorReplaceMap(int padNum, TAGS tag, INPUT_MAP map);
	void AddMap(int padNum, TAGS tag, INPUT_MAP map);
	void ReplaceMap(int padNum, TAGS tag, INPUT_MAP map);

	void ReplaceButtonMap(int padNum, TAGS tag, PAD_MAP_ARRAY replace);
	void ReplaceButtonMap(int padNum, TAGS tag, Gamepad::PAD_INPUT replace, size_t index);

	void ReplaceKeyMap(int padNum, TAGS tag, KEY_MAP_ARRAY replace);
	void ReplaceKeyMap(int padNum, TAGS tag, int replace, size_t index);

	int CheckNowMap(int padNum, TAGS tag) const;
	int CheckPrevMap(int padNum, TAGS tag) const;
	bool CheckDownMap(int padNum, TAGS tag) const;
	bool CheckUpMap(int padNum, TAGS tag) const;

	bool CheckNowKey(int DxLib_KEYcode) const;
	bool CheckPrevKey(int DxLib_KEYcode) const;
	bool CheckDownKey(int DxLib_KEYcode) const;
	bool CheckUpKey(int DxLib_KEYcode) const;

	Vector2 GetMousePos() const;
	Vector2 GetMouseMoveLength() const;

	bool CheckNowMouse(int DxLib_MOUSEcode) const;
	bool CheckPrevMouse(int DxLib_MOUSEcode) const;
	bool CheckDownMouse(int DxLib_MOUSEcode) const;
	bool CheckUpMouse(int DxLib_MOUSEcode) const;

	// 指定した番号のゲームパッドの接続状態を返す
	bool IsGamepadConnected(int pad_num) const;

private:
	static constexpr int KEYBOARD_NUM = 0; // キーボード入力を割り当てる番号（添字）
	static constexpr int PAD_NUM_MIN = 1; // パッド番号の最低値
	static constexpr int KEY_MOUSE_POWER = 1000;

	const int PAD_NUM_MAX; // パッド番号の最大値

	std::vector<std::map<TAGS, INPUT_MAP>> inputMap_; // タグと入力マップの対応表
	std::vector<std::unique_ptr<Gamepad>> gamepads_; // Gamepad クラスのポインタ配列

	KEY_STATE_ARRAY nowKey_; // 現在のキー情報
	KEY_STATE_ARRAY prevKey_; // 直前のキー情報

	Vector2 prevMousePos_; // マウスポインタの座標
	Vector2 nowMousePos_; // マウスポインタの座標
	MOUSE_STATE_MAP nowMouse_; // 現在のマウスボタン情報
	MOUSE_STATE_MAP prevMouse_; // 直前のマウスボタン情報

	void InitInputMap(); // 入力マップの初期設定

	void UpdateKey(); // キーボード更新
	void UpdateMouse(); // マウス更新
	void UpdatePad(); // ジョイパッド更新
	
	bool CheckNeedKeepLayout(int padNum, TAGS tag) const;
	bool InputBoundCheck(int padNum) const;

};
