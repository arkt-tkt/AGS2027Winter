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

void Player::Draw()
{
	if (!isAlive_) return;

	object3D_.Draw();

#ifdef _DEBUG
	auto lHPos = MV1GetFramePosition(object3D_.handleId, 11);
	DrawLine3D(lHPos, Vec3ToVEC(scnMng_.GetCameraPtr().GetTargetPosition()), 0xffff00u);

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

void Player::InitModel()
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
		object3D_.position + Vector3(0, 20, 0),
		object3D_.position + Vector3(0, 64, 0),
		8.0f);

	lineCollider_ = {
		object3D_.position + Vector3(0, -4, 0),
		object3D_.position + Vector3(0, 54, 0),
	};
}

void Player::InitAnim()
{
	anim_ = std::make_unique<Animation>(object3D_.handleId);
	anim_->AddFromOther(ResourceManager::SRC::ANIME_RIFLE_IDLE, 0, 30.0f);
	anim_->Play(ResourceManager::SRC::ANIME_RIFLE_IDLE, 0);
}

void Player::Move()
{
	if (!isAlive_) return;

	auto dt = scnMng_.GetDeltaTime();

	auto& ins = InputManager::GetInstance();
	using tag = InputManager::TAGS;

	const auto CAM_ROT_Y = scnMng_.GetCameraPtr().GetAngles().y;
	const auto ACC = float(accel_ * scnMng_.GetDeltaTime());
	const auto ACCELERATION = isSprinting_ ? 63.0f : 42.0f;
	const auto FRICTION = 10.0f;
	const auto MAX_SPD = isSprinting_ ? 3.75f : 2.5f;

	Vector3 moveDir;

	if (ins.CheckNowMap(INPUT_NUM, tag::MOVE_UP)) moveDir.z++; // 上移動の処理
	if (ins.CheckNowMap(INPUT_NUM, tag::MOVE_DOWN)) moveDir.z--; // 下移動の処理
	if (ins.CheckNowMap(INPUT_NUM, tag::MOVE_LEFT)) moveDir.x--; // 左移動の処理
	if (ins.CheckNowMap(INPUT_NUM, tag::MOVE_RIGHT)) moveDir.x++; // 右移動の処理

	float decaySpeed = std::exp(-FRICTION * dt);
	velocity_.x *= decaySpeed;
	velocity_.z *= decaySpeed;

	// 移動方向をカメラの向きに同期
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

	// 正面をカメラの向きに同期
	object3D_.rotation.y =
		LookRotation(scnMng_.GetCameraPtr().GetTargetPosition() - object3D_.position).ToEuler().y +
		std::abs(scnMng_.GetCameraPtr().GetAngles().x * 0.5f);

	// マップから落下した場合
	if (object3D_.position.y <= -1000.0f)
	{
		object3D_.position.y = 0.0f;
	}

	Quaternion qRot = Quaternion().Euler(-scnMng_.GetCameraPtr().GetAnglesDiff().x, 0, 0);

	MATRIX mat = MGetIdent();

	mat = MV1GetFrameLocalMatrix(object3D_.handleId, 6);
	MV1SetFrameUserLocalMatrix(object3D_.handleId, 6, MMult(mat, Mat4x4ToMAT(qRot.ToMatrix())));

	mat = MV1GetFrameLocalMatrix(object3D_.handleId, 8);
	MV1SetFrameUserLocalMatrix(object3D_.handleId, 8, MMult(mat, Mat4x4ToMAT(qRot.ToMatrix())));

	mat = MV1GetFrameLocalMatrix(object3D_.handleId, 32);
	MV1SetFrameUserLocalMatrix(object3D_.handleId, 32, MMult(mat, Mat4x4ToMAT(qRot.ToMatrix())));
}
