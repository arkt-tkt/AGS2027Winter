#include <cmath>
#include <fstream>
#include <string>
#include <algorithm>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Common/DxLibUtil.h"
#include "../Common/MathUtil.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "Player.h"
#include "Camera.h"

Camera::Camera()
{
	Init();
}

bool Camera::Init()
{
	// カメラのNearとFarを設定する
	SetCameraNearFar(CAMERA_NEAR_RANGE, CAMERA_FAR_RANGE);

	mode_ = MODE::FIXED;

	prevPos_ = pos_ = FIXED_CAMERA_LOCAL_POS;
	prevTargetPos_ = targetPos_ = FIXED_TARGET_POS;
	prevAngles_ = angles_ = FIXED_CAMERA_ANGLES;

	prevPos_ = pos_;

	followTarget_ = nullptr;

	return true;
}

void Camera::Update()
{
	prevPos_ = pos_;
	prevTargetPos_ = targetPos_;
	prevAngles_ = angles_;

#ifdef _DEBUG
	// デバッグ用のカメラ回転処理
	UpdateDebug();
#endif

	if (!SceneManager::GetInstance().IsPause()) switch (mode_)
	{
	case MODE::FIXED:
		UpdateFixed(); break;
	case MODE::FOLLOW:
		UpdateFollow(); break;
	}

	UpdateAngles();

	shakeTimer_ -= SceneManager::GetInstance().GetDeltaTime();
	if (shakeTimer_ < 0.0f) shakeTimer_ = 0.0f;
}

void Camera::BeforeDraw()
{
	// 描画の際は必ず直前に呼び出しておく
	SetupDxLibCamera();

	Vector3 finalPos = pos_;
	Vector3 finalTarget = targetPos_;

	// 画面の揺れを計算して反映させる
	ShakeScreen(finalPos, finalTarget);

	// カメラの回転行列を作成
	Matrix4x4 mat = RotationMatrixXYZ(angles_);

	// カメラの上方向を計算
	//Vector3 up = mat * Vector3(0.0f, 1.0f, 0.0f);

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		Vec3ToVEC(finalPos), Vec3ToVEC(finalTarget), VGet(0, 1, 0));

	// カメラの座標と回転を適用した後、Effekseerとの同期を行う
	Effekseer_Sync3DSetting();
}

#ifdef _DEBUG
void Camera::DebugDraw()
{
	using namespace MathUtil;

	DrawFormatString(900, 100, 0xffffff,
		"Camera: %.1f, %.1f, %.1f\nAngle : %.1f, %.1f, %.1f\nTarget: %.1f, %.1f, %.1f",
		pos_.x, pos_.y, pos_.z,
		RadToDeg(angles_.x), RadToDeg(angles_.y), RadToDeg(angles_.z),
		targetPos_.x, targetPos_.y, targetPos_.z);

	if (!followTarget_) return;
	DrawFormatString(900, 160, 0xffffff,
		"Follow: %.1f, %.1f, %.1f",
		followTarget_->position.x, followTarget_->position.y, followTarget_->position.z);
}
#endif

void Camera::SetFollowTarget(const Object3D* t)
{
	if (!t)
	{
		followTarget_ = nullptr;
		ChangeCameraMode(MODE::FIXED);
		return;
	}

	followTarget_ = t;
	ChangeCameraMode(MODE::FOLLOW);
}

void Camera::ChangeCameraMode(MODE mode) { mode_ = mode; }

const Vector3& Camera::GetPosition() const { return pos_; }

const Vector3& Camera::GetTargetPosition() const { return targetPos_; }

const Vector3& Camera::GetAngles() const { return angles_; }

Camera::MODE Camera::GetCameraMode() const { return mode_; }

void Camera::StartShake(float intensity, float duration)
{
	shakeIntensity_ = intensity;
	shakeDuration_ = duration;
	shakeTimer_ = duration;
}

void Camera::UpdateFixed()
{
	/*
	VECTOR radAngles = VDegToRad(angles_);
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(radAngles.x));
	mat = MMult(mat, MGetRotY(radAngles.y));

	VECTOR targetLocalRotPos = VTransform(FIXED_TARGET_POS, mat);
	VECTOR nextTargetPos = VAdd(VECTOR(), targetLocalRotPos);
	targetPos_ = VLerp(prevTargetPos_, nextTargetPos, 1.0f);

	VECTOR cameraLocalRotPos = VTransform(FIXED_CAMERA_LOCAL_POS, mat);
	VECTOR newPos = VAdd(FIXED_TARGET_POS, cameraLocalRotPos);
	pos_ = VLerp(prevPos_, newPos, 1.0f);
	*/

	Matrix4x4 mat = RotationMatrixXYZ(angles_);

	Vector3 targetLocalRotPos = mat * FIXED_TARGET_POS;
	Vector3 nextTargetPos = Vector3() + targetLocalRotPos;
	targetPos_ = Lerp(prevTargetPos_, nextTargetPos, 1.0f);

	Vector3 cameraLocalRotPos = mat * FIXED_CAMERA_LOCAL_POS;
	Vector3 newPos = FIXED_TARGET_POS + cameraLocalRotPos;
	pos_ = Lerp(prevPos_, newPos, 1.0f);
}

void Camera::UpdateFollow()
{
	// フォロー先が無いやん！となったら一時固定カメラで
	if (followTarget_ == nullptr)
	{
		UpdateFixed();
		return;
	}

	/*
	auto tgtPos = Vec3ToVEC(followTarget_->position);

	VECTOR radAngles = VDegToRad(angles_);
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(radAngles.x));
	mat = MMult(mat, MGetRotY(radAngles.y));

	VECTOR targetLocalRotPos = VTransform(FIXED_TARGET_POS, mat);
	VECTOR nextTargetPos = VAdd(tgtPos, targetLocalRotPos);
	targetPos_ = VLerp(prevTargetPos_, nextTargetPos, 1.0f);

	VECTOR cameraLocalRotPos = VTransform(FOLLOW_CAMERA_LOCAL_POS, mat);
	VECTOR newPos = VAdd(tgtPos, cameraLocalRotPos);
	pos_ = VLerp(prevPos_, newPos, 1.0f);
	*/

	Quaternion quaRotY = AngleAxis(Vector3(0, 1, 0), angles_.y);

	Quaternion quaRot = quaRotY * AngleAxis(Vector3(1, 0, 0), angles_.x);

	auto tgtPos = followTarget_->position;

	targetPos_ = tgtPos + (quaRot * FOLLOW_TARGET_LOCAL_POS);

	pos_ = tgtPos + (quaRot * FOLLOW_CAMERA_LOCAL_POS);
}

void Camera::UpdateAngles()
{
	auto& ins = InputManager::GetInstance();
	constexpr float add = 0.05f;
	const float dt = SceneManager::GetInstance().GetDeltaTime();

	auto vec = ins.GetMouseMoveLength();

	angles_.x += add * dt * vec.y;
	angles_.y += add * dt * vec.x;

	angles_.x = MathUtil::Clamp(angles_.x, UP_ANGLE_LIMIT, DOWN_ANGLE_LIMIT);
	angles_.y = MathUtil::RadIn2PI(angles_.y);
}

#ifdef _DEBUG
void Camera::UpdateDebug()
{
	auto& ins = InputManager::GetInstance();
	using tag = InputManager::TAGS;

	constexpr float add = DX_PI_F / 180.0f;
	//constexpr float distAdd = 1.0f;

	if (ins.CheckNowKey(KEY_INPUT_O))
	{
		angles_.x -= add;
	}
	if (ins.CheckNowKey(KEY_INPUT_L))
	{
		angles_.x += add;
	}
	if (ins.CheckNowKey(KEY_INPUT_K))
	{
		angles_.y -= add;
	}
	if (ins.CheckNowKey(KEY_INPUT_SEMICOLON))
	{
		angles_.y += add;
	}

	/*
	if (ins.CheckNowKey(KEY_INPUT_ADD))
		camLocalPos_.z += distAdd; // 近づく
	if (ins.CheckNowKey(KEY_INPUT_SUBTRACT))
		camLocalPos_.z -= distAdd; // 離れる
	*/

	// 既定値にリセット
	if (ins.CheckNowKey(KEY_INPUT_NUMPAD0))
	{
		angles_ = FIXED_CAMERA_ANGLES;
	}

	angles_.x = MathUtil::Clamp(angles_.x, UP_ANGLE_LIMIT, DOWN_ANGLE_LIMIT);
	angles_.y = MathUtil::RadIn2PI(angles_.y);
}
#endif

void Camera::SetupDxLibCamera()
{
	// SetDrawScreen関数を使用した際にリセットされる各設定もここに記述する
	// ConvWorldPosToScreenPos関数などを使用する場合には、その前に必ずこの関数を呼び出すこと！

	// カメラ・深度バッファ
	{
		// 遠近法カメラ(FOV)を設定する
		SetupCamera_Perspective(CAMERA_FOV);

		// Zバッファを有効にする
		SetUseZBuffer3D(TRUE);

		// Zバッファへの書き込みを有効にする
		SetWriteZBuffer3D(TRUE);

		// バックカリングを有効にする
		SetUseBackCulling(DX_CULLING_LEFT);
	}

	// ライト
	{
		// 正面から斜め下に向かったライト
		ChangeLightTypeDir(Vec3ToVEC(LIGHT_DIRECTION));
	}

	// フォグ
	{
		// フォグを有効化
		SetFogEnable(TRUE);

		// フォグの色を指定
		SetFogColor(FOG_COLOR[0], FOG_COLOR[1], FOG_COLOR[2]);

		// フォグの始点と終点を指定
		SetFogStartEnd(FOG_START_RANGE, FOG_END_RANGE);
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
