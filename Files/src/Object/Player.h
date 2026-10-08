#pragma once
#include <list>
#include <memory>
#include <numbers>
#include "ActorBase3D.h"

class Player : public ActorBase3D
{
public:
	Player(int player_num);
	virtual ~Player();

	virtual void Init() override;
	virtual void Draw() override;

	void Spawn();
	int GetInputNumber() const;

	// プレイヤー番号（0始まり）
	int GetPlayerNumber() const { return PLAYER_NUM; }

private:
	static constexpr float PLAYER_SCALE = 0.4f;
	static constexpr Position3 PLAYER_LOCAL_POSITION = { 0.0f, 0.0f, 0.0f };
	static constexpr Position3 PLAYER_POSITION_INIT = { 0.0f, 0.0f, 0.0f };

	static constexpr int INPUT_INDEX_ADJUSTER = 1;

	static constexpr float INVINCIBLE_TIME = 3.0f;

	static constexpr float MOVE_SPEED = 8.0f * 60.0f;

	const int PLAYER_NUM;
	const int INPUT_NUM;

	virtual void Move() override;

};
