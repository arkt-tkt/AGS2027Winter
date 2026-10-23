#pragma once
#include <list>
#include <memory>
#include <numbers>
#include "../Weapon/WeaponBase.h"
#include "CharacterBase3D.h"

class Player : public CharacterBase3D
{
public:
	Player(int player_num);
	virtual ~Player();

	void Draw() override;
	void DrawUI() override;

	// プレイヤー番号（0始まり）
	const int& GetPlayerNumber() const { return PLAYER_NUM; }
	const int& GetInputNumber() const { return INPUT_NUM; }

private:
	static constexpr float PLAYER_SCALE = 0.4f;
	static constexpr Position3 PLAYER_POSITION_INIT = { 0.0f, 0.0f, 0.0f };
	static constexpr Position3 PLAYER_POSITION_LOCAL = { 0.0f, 0.0f, 0.0f };

	static constexpr int ROTATABLE_FRAME_INDEX[] = { 2, 3, 4 };

	static constexpr int INPUT_INDEX_ADJUSTER = 1;

	static constexpr float INVINCIBLE_TIME = 3.0f;

	const int PLAYER_NUM;
	const int INPUT_NUM;

	std::unique_ptr<WeaponBase> wpn_;

	void InitParam() override;
	void InitModel() override;
	void InitAnim() override;
	void InitOther() override;

	void Move() override;
	void AnimationSwitch() override;
	void SubClass() override;

	void TurnVisual() override;

};
