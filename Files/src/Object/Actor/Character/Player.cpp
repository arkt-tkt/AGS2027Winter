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

void Player::Draw()
{
	if (!isAlive_) return;

#ifdef _DEBUG
	DrawLine3D(
		Vec3ToVEC(transform_.position + lineCollider_.first),
		Vec3ToVEC(transform_.position + lineCollider_.second),
		0xff8000u);

	const int POS_X = 1200;

	DrawFormatString(POS_X, 400, 0xffffffu, "Velocity: %.3f\n(X: %.3f, Z: %.3f)",
		velocity_.XZ().Length(), velocity_.x, velocity_.z);

	DrawFormatString(POS_X, 480, 0xffffffu, "Rotation: %.3f, %.3f, %.3f",
		transform_.rotation.x, transform_.rotation.y, transform_.rotation.z);
#endif

	inv_->Draw();

	CharacterBase3D::Draw();
}

void Player::Release()
{
	inv_->Release();

	CharacterBase3D::Release();
}

void Player::InitParam()
{
	isAlive_ = true;
	
	invincible_ = 3.0f;

	hp_ = baseStats_.hp;
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

	lineCollider_ = { Vector3(0, -4, 0), Vector3(0, 68, 0) };

	MV1SetMaterialEmiColor(transform_.modelId, 0, COLOR_F(0.2f, 0.2f, 0.2f, 1.0f));
}

void Player::InitAnim()
{
	animation_ = std::make_unique<AnimationController>(transform_.modelId);

	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_IDLE, 0, 30.0f,
		AnimationController::BASE);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_WALK_F, 0, 40.0f,
		AnimationController::FULL_BODY);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_WALK_FL, 0, 30.0f,
		AnimationController::FULL_BODY);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_WALK_FR, 0, 30.0f,
		AnimationController::FULL_BODY);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_WALK_L, 0, 40.0f,
		AnimationController::FULL_BODY);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_WALK_R, 0, 40.0f,
		AnimationController::FULL_BODY);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_WALK_B, 0, 30.0f,
		AnimationController::FULL_BODY);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_WALK_BL, 0, 30.0f,
		AnimationController::FULL_BODY);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_WALK_BR, 0, 30.0f,
		AnimationController::FULL_BODY);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_DASH_FRONT, 0, 30.0f,
		AnimationController::FULL_BODY);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_JUMP, 0, 30.0f,
		AnimationController::FULL_BODY);
	animation_->AddFromOther(ResourceManager::SRC::ANIME_PL_RF_RELOAD, 0, 30.0f,
		AnimationController::UPPER_BODY);

	animation_->Set(ResourceManager::SRC::ANIME_PL_RF_IDLE, 0);
}

void Player::InitOther()
{
	inv_ = std::make_unique<Inventory>(this);
}

void Player::Move()
{
	if (!isAlive_) return;

	auto dt = scnMng_.GetDeltaTime();

	// 毎秒1HP回復
	if (!damageTimer_)
	{
		hp_ = std::clamp(hp_ + dt, 0.0f, baseStats_.hp);
	}

	// 別名
	using tag = InputManager::TAGS;

	// ローカル定数
	const auto& INS = InputManager::GetInstance();
	const auto& CAMERA = scnMng_.GetCameraPtr();

	// 移動方向
	moveDir_ = {};

	// 上移動の処理
	moveDir_.z += INS.CheckNowMap(INPUT_NUM, tag::MOVE_UP);
	// 下移動の処理
	moveDir_.z -= INS.CheckNowMap(INPUT_NUM, tag::MOVE_DOWN);
	// 左移動の処理
	moveDir_.x -= INS.CheckNowMap(INPUT_NUM, tag::MOVE_LEFT);
	// 右移動の処理
	moveDir_.x += INS.CheckNowMap(INPUT_NUM, tag::MOVE_RIGHT);

	// 長さを得る
	auto len = moveDir_.Length() / Gamepad::DINPUT_STICK_MAX;
	// 正規化
	moveDir_.Normalize();

	if (INS.CheckDownMap(INPUT_NUM, tag::SPRINT))
	{
		sprintSwitch_ = !sprintSwitch_;
	}

	if (sprintSwitch_ && moveDir_.XZ().LengthSquare())
	{
		sprintRate_ = (std::min)(sprintRate_ + SPRINT_STANCE_RATE * dt, 1.0f);
	}
	else
	{
		sprintRate_ = (std::max)(sprintRate_ - SPRINT_STANCE_RATE * dt, 0.0f);

		if (sprintRate_ <= 0.0f) sprintSwitch_ = false;
	}

	auto toTarget = scnMng_.GetCameraPtr().GetTargetPosition() - transform_.position;

	const auto DOT = Dot(
		Vector2(velocity_.x, velocity_.z).Normalized(),
		Vector2(toTarget.x, toTarget.z).Normalized());
	const auto DIR_MOVE_MULT = 1.0f - 0.1f + DOT * 0.1f;

	const auto SPRINT_MULT = 1.0f + sprintRate_ * (baseStats_.sprint - 1.0f);
	auto maxSpd_ = baseStats_.movement * SPRINT_MULT * dt * inv_->GetData().movement;

	maxSpd_ *= (godMode_ ? 1.75f : 1.0f);

	Vector3 moveSpeed;
	float dotRate = 0.0f;

	// 移動入力あり
	if (moveDir_.XZ().LengthSquare() > 0.0f)
	{
		// 移動方向をカメラの向きに合わせて回転
		Quaternion quaMat = AngleAxis(Vector3(0, 1, 0), CAMERA.GetAngles().y);
		moveSpeed = (quaMat * moveDir_).Normalized() * len * baseStats_.accel * dt;

		// 移動方向と速度の内積を計算して、減速率を調整
		dotRate = (Dot(moveSpeed.XZ(), velocity_.XZ()) + 1.0f) * 0.5f;
	}

	// 緩やかに減速、多少滑る
	float decaySpeed = MathUtil::Lerp(std::exp(-baseStats_.deccel * dt), 1.0f, dotRate);
	velocity_.x *= decaySpeed;
	velocity_.z *= decaySpeed;

	// 移動方向をカメラの向きに同期
	if (moveDir_.XZ().LengthSquare() > 0.0f)
	{
		velocity_.x += moveSpeed.x;
		velocity_.z += moveSpeed.z;
	}

	// 最大速度
	if (velocity_.XZ().LengthSquare() * SPRINT_MULT > std::powf(maxSpd_ * DIR_MOVE_MULT, 2.0f))
	{
		float mult = (std::min)(maxSpd_ * DIR_MOVE_MULT / velocity_.XZ().Length(), 1.0f);

		velocity_.x *= mult;
		velocity_.z *= mult;
	}

	// ジャンプ
	if (INS.CheckDownMap(INPUT_NUM, tag::JUMP))
	{
		Jump();
	}

	// 本移動
	transform_.position += velocity_;

	// マップから落下した場合
	if (!MathUtil::Within(transform_.position.y, -1000.0f, 1000.0f))
	{
		transform_.position.y = 0.0f;
	}
}

void Player::AnimationSwitch()
{
	auto& ins = InputManager::GetInstance();
	using tag = InputManager::TAGS;

	if (inv_->GetReloadProg())
	{
		float animLen = inv_->GetData().reloadTime;
		animation_->PlayInSeconds(ResourceManager::SRC::ANIME_PL_RF_RELOAD, 0, animLen);
	}

	Vector3 dirs[] =
	{
		Vector3(0, 0, 1),
		Vector3(-1, 0, 1),
		Vector3(1, 0, 1),
		Vector3(-1, 0, 0),
		Vector3(1, 0, 0),
		Vector3(0, 0, -1),
		Vector3(-1, 0, -1),
		Vector3(1, 0, -1)
	};

	if (isLanding_)
	{
		animation_->Stop(ResourceManager::SRC::ANIME_PL_RF_JUMP, 0);

		auto src = int(ResourceManager::SRC::ANIME_PL_RF_WALK_F);

		for (auto i = 0; i <= 7; ++i)
		{
			const float DOT_RATE = 0.75f;
			float dot = (Dot(moveDir_, dirs[i].Normalized()) - DOT_RATE) / (1.0f - DOT_RATE);

			if (dot > 0.0f)
			{
				animation_->Play(ResourceManager::SRC(src + i), 0,
					true, dot, dot);
			}
			else
			{
				animation_->Stop(ResourceManager::SRC(src + i), 0);
			}
		}
	}
	else
	{
		animation_->Play(ResourceManager::SRC::ANIME_PL_RF_JUMP, 0, false, 1.0f, 1.0f, false);

		auto src = int(ResourceManager::SRC::ANIME_PL_RF_WALK_F);

		for (auto i = 0; i <= 7; ++i)
		{
			animation_->Stop(ResourceManager::SRC(src + i), 0);
		}
	}

	return;
}

void Player::LateMove()
{
	if (!isAlive_) return;

	inv_->Update();

	if (inv_->IsActed()) sprintSwitch_ = false;
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

	float visualAngleX = -scnMng_.GetCameraPtr().GetVisualAngles().x;

	Quaternion qRot = Quaternion().Euler(
		visualAngleX * 0.8f / frameCnt,
		0,
		visualAngleX * 0.64f / frameCnt);

	// ここにフレーム本来の座標、回転、拡大率が入る
	MATRIX mat = MGetIdent();

	for (const int& idx : ROTATABLE_FRAME_INDEX)
	{
		MV1ResetFrameUserLocalMatrix(transform_.modelId, idx);
		mat = MV1GetFrameLocalMatrix(transform_.modelId, idx);
		MV1SetFrameUserLocalMatrix(transform_.modelId, idx, MMult(mat, Mat4x4ToMAT(qRot.ToMatrix())));
	}

	transform_.Update();

	inv_->LateUpdate();
}
