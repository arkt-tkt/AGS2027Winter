#include <cmath>
#include <DxLib.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Common/DxLibUtil.h"
#include "../Manager/AudioManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "Common/Transform3D.h"
#include "Camera.h"

Camera::Camera()
{
	mode_ = MODE::FIXED;

	prevPos_ = pos_ = FIXED_CAMERA_LOCAL_POS;
	prevTargetPos_ = targetPos_ = FIXED_TARGET_POS;
	prevAngles_ = angles_ = FIXED_CAMERA_ANGLES;
	angles_ = INIT_ANGLES;

	mouseCamControl_ = !InputManager::GetInstance().GetMouseVisibility();

	followTarget_ = nullptr;
}

Camera::~Camera()
{
	ResetAwayColliders();
}

void Camera::Update()
{
	prevPos_ = pos_;
	prevTargetPos_ = targetPos_;
	prevAngles_ = angles_;

	auto x = isReverseX_ ? -FOLLOW_TP_LOCAL_POS_X : FOLLOW_TP_LOCAL_POS_X;
	posLocalX_ = MathUtil::Lerp(posLocalX_, x, FOLLOW_TP_LOCAL_POS_X_TRANS_RATE);

	mouseCamControl_ = !InputManager::GetInstance().GetMouseVisibility();

	if (!SceneManager::GetInstance().IsPause()) switch (mode_)
	{
	case MODE::FIXED:
		UpdateFixed(); break;
	case MODE::FOLLOW_TP:
		UpdateFollowTP(); break;
	case MODE::FOLLOW_FP:
		UpdateFollowFP(); break;
	}

	UpdateAngles();

	shakeTimer_ -= SceneManager::GetInstance().GetDeltaTime();
	if (shakeTimer_ < 0.0f) shakeTimer_ = 0.0f;

	if (mode_ == MODE::FOLLOW_TP)
	{
		CollisionStage();

		CollisionPlayer();
	}
}

void Camera::BeforeDraw()
{
	// 描画の際は必ず直前に呼び出しておく
	UpdateDxLibCamera();

	Vector3 finalPos = pos_;
	Vector3 finalTarget = targetPos_;

	// 画面の揺れを計算して反映させる
	ShakeScreen(finalPos, finalTarget);

	// 反動を考慮して上ベクトルを曲げる四元数
	auto nowAngles = _AngleClamp(angles_ + recoil_);
	Quaternion qRot = AngleAxis(Vector3(0, 0, 1), nowAngles.z) * AngleAxis(Vector3(1, 0, 0), nowAngles.x) * AngleAxis(Vector3(0, 1, 0), nowAngles.y);

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		Vec3ToVEC(finalPos), Vec3ToVEC(finalTarget), Vec3ToVEC(qRot * Vector3(0, 1, 0)));

	auto front = (finalTarget - finalPos).Normalized();

	AudioManager::GetInstance().Set3DListenerPos(
			finalPos, front, Vector3(0, 1, 0));

	// カメラの座標と回転を適用した後、Effekseerとの同期を行う
	Effekseer_Sync3DSetting();
}

#ifdef _DEBUG
void Camera::DebugDraw()
{
	using namespace MathUtil;

	const int POS_X = 1200;

	DrawFormatString(POS_X, 100, 0xffffff,
		"Camera: %.1f, %.1f, %.1f\nAngle : %.1f, %.1f, %.1f\nTarget: %.1f, %.1f, %.1f",
		pos_.x, pos_.y, pos_.z,
		RadToDeg(angles_.x), RadToDeg(angles_.y), RadToDeg(angles_.z),
		targetPos_.x, targetPos_.y, targetPos_.z);

	if (!followTarget_)
	{
		DrawFormatString(POS_X, 240, 0xffffff, "Follow: nil");
	}
	else
	{
		DrawFormatString(POS_X, 240, 0xffffff,
			"Follow: %.1f, %.1f, %.1f",
			followTarget_->position.x, followTarget_->position.y, followTarget_->position.z);
	}
}
#endif

void Camera::AddAwayCollider(std::weak_ptr<Collider3D> col)
{
	awayColliders_.push_back(col);
}

void Camera::AddAwayColliders(std::vector<std::shared_ptr<Collider3D>> cols)
{
	for (auto& col : cols)
	{
		awayColliders_.push_back(col);
	}
}

void Camera::ResetAwayColliders()
{
	awayColliders_.clear();
}

void Camera::SetFollowTransform(const Transform3D* t)
{
	if (!t)
	{
		followTarget_ = nullptr;
		ChangeCameraMode(MODE::FIXED);
		return;
	}

	followTarget_ = t;
	ChangeCameraMode(MODE::FOLLOW_FP);
}

void Camera::ChangeCameraMode(MODE mode)
{
	mode_ = mode;
}

void Camera::SwitchFollowMode()
{
	if (mode_ == MODE::NONE || mode_ == MODE::FIXED) return;

	auto intMode = (int)mode_;

	if (++intMode == (int)MODE::END)
	{
		intMode = (int)MODE::FIXED + 1;
	}

	mode_ = (Camera::MODE)intMode;
}

bool Camera::IsInCameraBounds(Position3 pos)
{
	return MathUtil::Within(pos.x, 0.0f, Application::RESOLUTION.x) &&
		MathUtil::Within(pos.y, 0.0f, Application::RESOLUTION.y) &&
		MathUtil::Within(pos.z, 0.0f, 1.0f);
}

void Camera::StartShake(float intensity, float duration)
{
	shakeIntensity_ = intensity;
	shakeDuration_ = duration;
	shakeTimer_ = duration;
}

void Camera::UpdateDxLibCamera()
{
	// SetDrawScreen関数を使用した際にリセットされる各設定もここに記述する
	// ConvWorldPosToScreenPos関数などを使用する場合には、その前に必ずこの関数を呼び出すこと！


	/// カメラ
	// 遠近法カメラ(FOV)を設定する
	SetupCamera_Perspective(CAMERA_FOV);

	// カメラのNearとFarを設定する
	SetCameraNearFar(CAMERA_NEAR_RANGE, CAMERA_FAR_RANGE);


	/// 深度バッファ
	// Zバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(TRUE);

	// バックカリングを有効にする
	SetUseBackCulling(DX_CULLING_LEFT);


	/// ライト
	// 正面から斜め下に向かったライト
	ChangeLightTypeDir(Vec3ToVEC(LIGHT_DIRECTION));


	/// フォグ
	// フォグを有効化
	SetFogEnable(TRUE);

	// フォグの色を指定
	SetFogColor(FOG_COLOR[0], FOG_COLOR[1], FOG_COLOR[2]);

	// フォグの始点と終点を指定
	SetFogStartEnd(FOG_START_RANGE, FOG_END_RANGE);

}

void Camera::UpdateFixed()
{
	Quaternion qRot = AngleAxis(Vector3(0, 1, 0), angles_.y) * AngleAxis(Vector3(1, 0, 0), angles_.x);

	targetPos_ = FIXED_TARGET_POS;

	pos_ = (qRot * FIXED_CAMERA_LOCAL_POS);
}

void Camera::UpdateFollowTP()
{
	// フォロー先が無いやん！となったら一時固定カメラで
	if (followTarget_ == nullptr)
	{
		UpdateFixed();
		return;
	}

	auto nowAngles = _AngleClamp(angles_ + recoil_);
	Quaternion qRot = AngleAxis(Vector3(0, 0, 1), nowAngles.z) * AngleAxis(Vector3(1, 0, 0), nowAngles.x) * AngleAxis(Vector3(0, 1, 0), nowAngles.y);

	auto fPos = followTarget_->position;

	targetPos_ = fPos + qRot * Position3(posLocalX_, FOLLOW_TP_LOCAL_POS_Y, FOLLOW_TP_LOCAL_TARGET_POS_Z);

	pos_ = fPos + qRot * Position3(posLocalX_, FOLLOW_TP_LOCAL_POS_Y, FOLLOW_TP_LOCAL_CAMERA_POS_Z);
}

void Camera::UpdateFollowFP()
{
	// フォロー先が無いやん！となったら一時固定カメラで
	if (followTarget_ == nullptr)
	{
		UpdateFixed();
		return;
	}

	auto nowAngles = _AngleClamp(angles_ + recoil_);
	Quaternion qRot = AngleAxis(Vector3(0, 0, 1), nowAngles.z) * AngleAxis(Vector3(1, 0, 0), nowAngles.x) * AngleAxis(Vector3(0, 1, 0), nowAngles.y);

	auto fPos = followTarget_->position;
	auto fNeckPos = VECToVec3(MV1GetFramePosition(followTarget_->modelId, 5));

	targetPos_ = fNeckPos + qRot * Vector3(-3, 5, 500);

	pos_ = fNeckPos + qRot * Vector3(-3, 5, 8);
}

void Camera::UpdateAngles()
{
	auto& ins = InputManager::GetInstance();
	const float dt = SceneManager::GetInstance().GetDeltaTime();

	if (mouseCamControl_)
	{
		Vector2 mVec = ins.GetMouseMoveLength() * mouseSensitivity_ * dt;
		angles_.x += mVec.y;
		angles_.y += mVec.x;
	}

	const float stickSensitivity = 1.8f / float(Gamepad::DINPUT_STICK_MAX) * dt;
	Vector2 pVec(0, 0);
	pVec.y -= ins.CheckNowMap(1, InputManager::TAGS::VIEW_UP) * stickSensitivity;
	pVec.y += ins.CheckNowMap(1, InputManager::TAGS::VIEW_DOWN) * stickSensitivity;
	pVec.x -= ins.CheckNowMap(1, InputManager::TAGS::VIEW_LEFT) * stickSensitivity;
	pVec.x += ins.CheckNowMap(1, InputManager::TAGS::VIEW_RIGHT) * stickSensitivity;
	float pLen = pVec.Length();
	pVec.Normalize();
	pVec *= pLen;

	angles_.x += pVec.y;
	angles_.y += pVec.x;

	angles_.x = MathUtil::Clamp(angles_.x, UP_ANGLE_LIMIT, DOWN_ANGLE_LIMIT);
	angles_.y = MathUtil::RadIn2PI(angles_.y);

	recoil_ += Lerp(Vector3(), -recoil_, 4.0f * dt);
	auto kickDiff = Lerp(Vector3(), kickPower_, 20.0f * dt);
	recoil_ += kickDiff;
	kickPower_ -= kickDiff;
}

void Camera::CollisionStage()
{
	Quaternion qRot = AngleAxis(Vector3(1, 0, 0), angles_.x) * AngleAxis(Vector3(0, 1, 0), angles_.y);

	for (const auto& c : awayColliders_)
	{
		auto& dat = c.lock()->GetColliderData();

		if (dat.type != Collider3D::TYPE::STAGE || dat.modelId == -1) continue;

		auto hitPoly = c.lock()->GetNearestHitPolyLine(
			followTarget_->position + Vector3(0, FOLLOW_TP_LOCAL_POS_Y, 0),
			pos_ + qRot * Vector3(2, 1, 0)
		);

		if (!hitPoly.HitFlag)
		{
			continue;
		}

		auto pushBack =
			Vector3(-FOLLOW_TP_LOCAL_POS_X, 0, -FOLLOW_TP_LOCAL_CAMERA_POS_Z).Normalized() * 0.5f;

		auto norm = VECToVec3(hitPoly.Normal).Normalized() * 3.0f;

		Vector3 newPos = VECToVec3(hitPoly.HitPosition) + qRot * pushBack + norm;
		Vector3 targetPosAdjust = newPos - pos_;

		pos_ = newPos;
		targetPos_ += targetPosAdjust;
	}
}

void Camera::CollisionPlayer()
{
	for (const auto& c : awayColliders_)
	{
		auto& dat = c.lock()->GetColliderData();

		if (dat.type != Collider3D::TYPE::PLAYER || dat.modelId == -1) continue;

		if (mode_ != MODE::FOLLOW_TP)
		{
			MV1SetOpacityRate(dat.modelId, 1.0f);
			return;
		}

		auto hit = MV1CollCheck_Line(
			dat.modelId, -1,
			Vec3ToVEC(pos_),
			Vec3ToVEC(targetPos_)
		);

		if (hit.HitFlag)
		{
			MV1SetOpacityRate(dat.modelId, 0.25f);
		}
		else if (MV1GetOpacityRate(dat.modelId) < 1.0f)
		{
			MV1SetOpacityRate(dat.modelId, 1.0f);
		}
	}
}

void Camera::ShakeScreen(Vector3& cam_pos, Vector3& tgt_pos) const
{
	// スクリーンシェイクの適用 (減衰付きランダムオフセット)
	if (shakeTimer_ > 0.0f)
	{
		// 残り時間に応じた減衰率 (1.0 -> 0.0)
		float power = shakeTimer_ / shakeDuration_;
		float currentIntensity = shakeIntensity_ * power;

		// X, Y, Z にランダムな揺れを加算
		Vector3 offset = {
			(GetRand(100) / 50.0f - 1.0f) * currentIntensity,
			(GetRand(100) / 50.0f - 1.0f) * currentIntensity,
			(GetRand(100) / 50.0f - 1.0f) * currentIntensity
		};

		cam_pos += offset;
		tgt_pos += offset;
	}
}

Vector3 Camera::_AngleClamp(const Vector3& v) const
{
	Vector3 ret = v;
	ret.x = MathUtil::Clamp(v.x, UP_ANGLE_LIMIT, DOWN_ANGLE_LIMIT);
	ret.y = MathUtil::RadIn2PI(v.y);
	return ret;
}
