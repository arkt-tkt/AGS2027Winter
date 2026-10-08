#include <DxLib.h>
#include "../Application.h"
#include "../Common/DxLibUtil.h"
#include "../Common/MathUtil.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "Player.h"

Player::Player(int player_num)
	:
	PLAYER_NUM(player_num),
	INPUT_NUM(PLAYER_NUM + INPUT_INDEX_ADJUSTER),
	ActorBase3D(true)
{
}

Player::~Player()
{
}

void Player::Init()
{
	auto i = static_cast<int>(ResourceManager::SRC::MODEL_PLAYER);

	object3D_.handleId = resMng_.LoadModelDuplicate(
		ResourceManager::SRC(static_cast<ResourceManager::SRC>(i + PLAYER_NUM)));
	object3D_.scale = PLAYER_SCALE;
	object3D_.position = PLAYER_POSITION_INIT;
	object3D_.localPosition = PLAYER_LOCAL_POSITION;
	object3D_.localRotation.y = DX_PI_F;

	object3D_.collider = std::make_shared<Collider3D>(
		Collider3D::TYPE::PLAYER,
		object3D_.position + Vector3(0, 12, 0),
		object3D_.position + Vector3(0, 64, 0),
		8.0f);

	lineCollider_ = {
		object3D_.position + Vector3(0, -4, 0),
		object3D_.position + Vector3(0, 54, 0),
	};

	object3D_.Update();
}

void Player::Draw()
{
	if (!isAlive_) return;

	object3D_.Draw();

#ifdef _DEBUG
	DrawFormatString(200, 200, 0xffffffu, "Velocity: %.3f\n(X: %.3f, Z: %.3f)",
		velocity_.XZ().Length(), velocity_.x, velocity_.z);
#endif
}

void Player::Spawn()
{
	object3D_.position = PLAYER_POSITION_INIT;

	isAlive_ = true;
	hp_ = 1;
	invincible_ = 3.0f;

	object3D_.Update();
}

int Player::GetInputNumber() const
{
	return INPUT_NUM;
}

void Player::Move()
{
	if (!isAlive_) return;

	auto dt = scnMng_.GetDeltaTime();

	auto& ins = InputManager::GetInstance();
	using tag = InputManager::TAGS;

	const auto CAM_ROT_Y = scnMng_.GetCameraPtr().GetAngles().y;
	const auto ACC = float(accel_ * scnMng_.GetDeltaTime());
	const auto ACCELERATION = isSprinting_ ? 96.0f : 64.0f;
	const auto FRICTION = 12.0f;
	const auto MAX_SPD = isSprinting_ ? 7.5f : 5.0f;

	Vector3 moveDir;

	if (ins.CheckNowMap(INPUT_NUM, tag::MOVE_UP)) moveDir.z++; // è„à⁄ìÆÇÃèàóù
	if (ins.CheckNowMap(INPUT_NUM, tag::MOVE_DOWN)) moveDir.z--; // â∫à⁄ìÆÇÃèàóù
	if (ins.CheckNowMap(INPUT_NUM, tag::MOVE_LEFT)) moveDir.x--; // ç∂à⁄ìÆÇÃèàóù
	if (ins.CheckNowMap(INPUT_NUM, tag::MOVE_RIGHT)) moveDir.x++; // âEà⁄ìÆÇÃèàóù

	float decaySpeed = std::exp(-FRICTION * dt);
	velocity_.x *= decaySpeed;
	velocity_.z *= decaySpeed;

	// à⁄ìÆï˚å¸ÇÉJÉÅÉâÇÃå¸Ç´Ç…ìØä˙
	if (moveDir.XZ().LengthSquare() > 0.0f)
	{
		Quaternion quaMat = AngleAxis(Vector3(0, 1, 0), CAM_ROT_Y);
		Vector3 moveSpeed = (quaMat * moveDir).Normalized() * ACCELERATION * dt;
		velocity_.x += moveSpeed.x;
		velocity_.z += moveSpeed.z;
	}

	isSprinting_ = ins.CheckNowMap(INPUT_NUM, tag::SPRINT) ? true : false;

	if (velocity_.XZ().LengthSquare() > MAX_SPD * MAX_SPD)
	{
		float mult = float(double(MAX_SPD) * MAX_SPD / velocity_.XZ().LengthSquare());

		velocity_.x *= mult;
		velocity_.z *= mult;
	}

	if (ins.CheckNowMap(INPUT_NUM, tag::JUMP))
	{
		Jump();
	}

	// ê≥ñ ÇÉJÉÅÉâÇÃå¸Ç´Ç…ìØä˙
	object3D_.rotation.y = CAM_ROT_Y;
}
