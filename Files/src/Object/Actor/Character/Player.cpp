#include <DxLib.h>
#include "../../../Application.h"
#include "../../../Common/DxLibUtil.h"
#include "../../../Common/MathUtil.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "Player.h"

Player::Player(int player_num)
	:
	PLAYER_NUM(player_num),
	INPUT_NUM(PLAYER_NUM + INPUT_INDEX_ADJUSTER),
	CharacterBase3D()
{
}

Player::~Player()
{
}

void Player::Draw()
{
	if (!isAlive_) return;

#ifdef _DEBUG
	DrawLine3D(
		Vec3ToVEC(transform_.position + lineCollider_.first),
		Vec3ToVEC(transform_.position + lineCollider_.second),
		0xff8000u);

	auto lHPos = MV1GetFramePosition(transform_.handleId, 11);
	DrawLine3D(lHPos, Vec3ToVEC(scnMng_.GetCameraPtr().GetTargetPosition()), 0xffff00u);

	DrawFormatString(200, 200, 0xffffffu, "Velocity: %.3f\n(X: %.3f, Z: %.3f)",
		velocity_.XZ().Length(), velocity_.x, velocity_.z);
#endif

	TurnBody();

	CharacterBase3D::Draw();
}

void Player::Spawn()
{
	transform_.position = PLAYER_POSITION_INIT;

	isAlive_ = true;
	hp_ = 1;
	invincible_ = 3.0f;

	transform_.Update();
}

void Player::InitModel()
{
	auto i = static_cast<int>(ResourceManager::SRC::MODEL_PLAYER);

	transform_.handleId = resMng_.LoadModelDuplicate(
		ResourceManager::SRC(static_cast<ResourceManager::SRC>(i + PLAYER_NUM)));
	transform_.scale = PLAYER_SCALE;
	transform_.position = PLAYER_POSITION_INIT;
	transform_.localPosition = PLAYER_LOCAL_POSITION;
	transform_.localRotation.y = DX_PI_F;

	colliders_.emplace_back(
		std::make_shared<Collider3D>(
			Collider3D::TYPE::COLLIDER,
			transform_.position + Vector3(0, 24, 0),
			transform_.position + Vector3(0, 60, 0),
			8.0f)
	);
	colliders_.emplace_back(
		std::make_shared<Collider3D>(
			Collider3D::TYPE::PLAYER,
			transform_.handleId)
	);

	lineCollider_ = {
		Vector3(0, -4, 0),
		Vector3(0, 68, 0),
	};

	MV1SetMaterialEmiColor(transform_.handleId, 0, COLOR_F(0.2f, 0.2f, 0.2f, 1.0f));
}

void Player::InitAnim()
{
	anim_ = std::make_unique<Animation>(transform_.handleId);
	anim_->AddFromOther(ResourceManager::SRC::ANIME_RIFLE_IDLE, 0, 30.0f);
	anim_->Play(ResourceManager::SRC::ANIME_RIFLE_IDLE, 0);
}

void Player::Move()
{
	if (!isAlive_) return;

	auto dt = scnMng_.GetDeltaTime();

	auto& ins = InputManager::GetInstance();
	using tag = InputManager::TAGS;

	const auto& CAMERA = scnMng_.GetCameraPtr();
	const auto ACC = float(accel_ * scnMng_.GetDeltaTime());
	const auto ACCELERATION = isSprinting_ ? 24.0f : 24.0f;
	const auto FRICTION = 10.0f;
	const auto MAX_SPD = isSprinting_ ? 1.5f : 1.0f;

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
		Quaternion quaMat = AngleAxis(Vector3(0, 1, 0), CAMERA.GetAngles().y);
		Vector3 moveSpeed = (quaMat * moveDir).Normalized() * ACCELERATION * dt;
		velocity_.x += moveSpeed.x;
		velocity_.z += moveSpeed.z;
	}

	isSprinting_ = ins.CheckNowMap(INPUT_NUM, tag::SPRINT) ? true : false;

	if (velocity_.XZ().LengthSquare() > MAX_SPD * MAX_SPD)
	{
		float mult = MAX_SPD / velocity_.XZ().Length();

		velocity_.x *= mult;
		velocity_.z *= mult;
	}

	if (ins.CheckDownMap(INPUT_NUM, tag::JUMP))
	{
		Jump();
	}

	// マップから落下した場合
	if (transform_.position.y <= -1000.0f ||
		transform_.position.y >= 1000.0f)
		transform_.position.y = 0.0f;
}

void Player::TurnBody()
{
	// 正面をカメラの向きに同期
	transform_.rotation.y = LookRotation(scnMng_.GetCameraPtr().GetTargetPosition() - transform_.position).ToEuler().y;

	Quaternion qRot = Quaternion().Euler(
		-scnMng_.GetCameraPtr().GetAngles().x * 0.28f,
		0,
		-scnMng_.GetCameraPtr().GetAngles().x * 0.2f);

	// ここにフレーム本来の座標、回転、拡大率が入る
	MATRIX mat = MGetIdent();

	// 背骨(腰)
	MV1ResetFrameUserLocalMatrix(transform_.handleId, 2);
	mat = MV1GetFrameLocalMatrix(transform_.handleId, 2);
	MV1SetFrameUserLocalMatrix(transform_.handleId, 2, MMult(mat, Mat4x4ToMAT(qRot.ToMatrix())));

	// 背骨(腰上)
	MV1ResetFrameUserLocalMatrix(transform_.handleId, 3);
	mat = MV1GetFrameLocalMatrix(transform_.handleId, 3);
	MV1SetFrameUserLocalMatrix(transform_.handleId, 3, MMult(mat, Mat4x4ToMAT(qRot.ToMatrix())));

	// 背骨(胸)
	MV1ResetFrameUserLocalMatrix(transform_.handleId, 4);
	mat = MV1GetFrameLocalMatrix(transform_.handleId, 4);
	MV1SetFrameUserLocalMatrix(transform_.handleId, 4, MMult(mat, Mat4x4ToMAT(qRot.ToMatrix())));

	transform_.Update();
}
