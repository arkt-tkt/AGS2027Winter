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

	DrawFormatString(200, 200, 0xffffffu, "Velocity: %.3f\n(X: %.3f, Z: %.3f)",
		velocity_.XZ().Length(), velocity_.x, velocity_.z);

	DrawFormatString(200, 240, 0xffffffu, "Rotation: %.3f, %.3f, %.3f",
		transform_.rotation.x, transform_.rotation.y, transform_.rotation.z);
#endif

	CharacterBase3D::Draw();

	wpn_->Draw();
}

void Player::DrawUI()
{
	DrawFormatString(20, 700, 0xffffffu, "HP %3d", MathUtil::Round(hp_));

	wpn_->DrawUI();
}

void Player::InitParam()
{
	isAlive_ = true;
	
	invincible_ = 3.0f;
}

void Player::InitModel()
{
	auto i = static_cast<int>(ResourceManager::SRC::MODEL_PLAYER);

	transform_.modelId = resMng_.LoadModelDuplicate(
		ResourceManager::SRC(static_cast<ResourceManager::SRC>(i + PLAYER_NUM)));
	transform_.scale = PLAYER_SCALE;
	transform_.position = PLAYER_POSITION_INIT;
	transform_.localPosition = PLAYER_POSITION_LOCAL;
	transform_.localRotation.y = DX_PI_F;

	colliders_.emplace_back(
		std::make_shared<Collider3D>(
			Collider3D::TYPE::COLLIDER,
			Vector3(0, 24, 0),
			Vector3(0, 60, 0),
			8.0f));

	colliders_.emplace_back(
		std::make_shared<Collider3D>(
			Collider3D::TYPE::PLAYER,
			transform_.modelId));

	lineCollider_ = {
		Vector3(0, -4, 0),
		Vector3(0, 68, 0),
	};

	MV1SetMaterialEmiColor(transform_.modelId, 0, COLOR_F(0.2f, 0.2f, 0.2f, 1.0f));
}

void Player::InitAnim()
{
	animation_ = std::make_unique<Animation>(transform_.modelId);

	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_IDLE, 0, 30.0f);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_MOVE, 0, 30.0f);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_RELOAD, 0, 30.0f);

	animation_->Play(ResourceManager::SRC::ANIME_PL_RF_IDLE, 0);
}

void Player::InitOther()
{
	wpn_ = std::make_unique<WeaponBase>();
	wpn_->Init();
	wpn_->SetParentTransform(transform_);
}

void Player::Move()
{
	if (!isAlive_) return;

	auto dt = scnMng_.GetDeltaTime();

	// 別名
	using tag = InputManager::TAGS;

	// ローカル定数
	const auto& INS = InputManager::GetInstance();
	const auto& CAMERA = scnMng_.GetCameraPtr();

	// 移動方向
	Vector3 moveDir;
	if (INS.CheckNowMap(INPUT_NUM, tag::MOVE_UP)) moveDir.z++; // 上移動の処理
	if (INS.CheckNowMap(INPUT_NUM, tag::MOVE_DOWN)) moveDir.z--; // 下移動の処理
	if (INS.CheckNowMap(INPUT_NUM, tag::MOVE_LEFT)) moveDir.x--; // 左移動の処理
	if (INS.CheckNowMap(INPUT_NUM, tag::MOVE_RIGHT)) moveDir.x++; // 右移動の処理


	isSprinting_ = INS.CheckNowMap(INPUT_NUM, tag::SPRINT) && moveDir.XZ().LengthSquare() ?
		true : false;
	const auto SPRINT_MULT = isSprinting_ ? sprintMult_ : 1.0f;
	const auto MAX_SPD = maxSpeed_ * SPRINT_MULT * dt;
	Vector3 moveSpeed;
	float dotRate = 0.0f;

	if (moveDir.XZ().LengthSquare() > 0.0f)
	{
		// 移動方向をカメラの向きに合わせて回転
		Quaternion quaMat = AngleAxis(Vector3(0, 1, 0), CAMERA.GetAngles().y);
		moveSpeed = (quaMat * moveDir).Normalized() * accel_ * dt;

		dotRate = Dot(moveSpeed.XZ(), velocity_.XZ()) + 1.0f * 0.5f;
	}

	// 緩やかに減速、多少滑る
	float decaySpeed =  MathUtil::Lerp(std::exp(-deccel_ * dt), 1.0f, dotRate);
	velocity_.x *= decaySpeed;
	velocity_.z *= decaySpeed;

	// 移動方向をカメラの向きに同期
	if (moveDir.XZ().LengthSquare() > 0.0f)
	{
		velocity_.x += moveSpeed.x;
		velocity_.z += moveSpeed.z;
	}

	// 最大速度
	if (velocity_.XZ().LengthSquare() * SPRINT_MULT > MAX_SPD * MAX_SPD)
	{
		float mult = MAX_SPD / velocity_.XZ().Length();

		velocity_.x *= mult;
		velocity_.z *= mult;
	}

	// ジャンプ
	if (INS.CheckDownMap(INPUT_NUM, tag::JUMP))
	{
		Jump();
	}

	// マップから落下した場合
	if (transform_.position.y <= -1000.0f ||
		transform_.position.y >= 1000.0f)
		transform_.position.y = 0.0f;
}

void Player::AnimationSwitch()
{
	auto& ins = InputManager::GetInstance();
	using tag = InputManager::TAGS;
	
	transform_.localPosition = {};

	if (!animation_->IsPlaying(ResourceManager::SRC::ANIME_PL_RF_RELOAD, 0))
	{
		if (ins.CheckDownMap(INPUT_NUM, tag::RELOAD) || wpn_->GetReloadProg())
		{
			animation_->Play(ResourceManager::SRC::ANIME_PL_RF_RELOAD, 0, false);
		}
		else if (velocity_.XZ().LengthSquare() > 0.1f)
		{
			animation_->Play(ResourceManager::SRC::ANIME_PL_RF_MOVE, 0);
		}
		else
		{
			animation_->Play(ResourceManager::SRC::ANIME_PL_RF_IDLE, 0);
		}
	}

}

void Player::SubClass()
{
	wpn_->Update();

	const auto& INS = InputManager::GetInstance();

	if (INS.CheckDownMap(INPUT_NUM, InputManager::TAGS::RELOAD))
	{
		wpn_->Reload();
	}
	else if (INS.CheckNowMap(INPUT_NUM, InputManager::TAGS::ATTACK_SUB))
	{
		wpn_->Sub();
	}
	else if (INS.CheckNowMap(INPUT_NUM, InputManager::TAGS::ATTACK_MAIN))
	{
		wpn_->Main();
	}
}

void Player::TurnVisual()
{
	// キャラクターから注視点へのベクトル
	auto toTarget = scnMng_.GetCameraPtr().GetTargetPosition() - transform_.position;
	// ここでY軸要素を除去しないと回転が不均一になる
	toTarget.y = 0.0f;

	// 正面をカメラの向きに同期
	transform_.rotation.y = LookRotation(toTarget).ToEuler().y;

	// 徐々に体を折り曲げる
	int frameCnt = _countof(ROTATABLE_FRAME_INDEX);

	Quaternion qRot = Quaternion().Euler(
		-scnMng_.GetCameraPtr().GetAngles().x * 0.84f / frameCnt,
		0,
		-scnMng_.GetCameraPtr().GetAngles().x * 0.6f / frameCnt);

	// ここにフレーム本来の座標、回転、拡大率が入る
	MATRIX mat = MGetIdent();

	for (const int& idx : ROTATABLE_FRAME_INDEX)
	{
		MV1ResetFrameUserLocalMatrix(transform_.modelId, idx);
		mat = MV1GetFrameLocalMatrix(transform_.modelId, idx);
		MV1SetFrameUserLocalMatrix(transform_.modelId, idx, MMult(mat, Mat4x4ToMAT(qRot.ToMatrix())));
	}

	transform_.Update();

	wpn_->LateUpdate();
}
