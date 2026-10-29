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
#include "Common/Transform3D.h"
#include "Camera.h"

Camera::Camera()
{
	Init();
}

Camera::~Camera()
{
	ResetAwayColliders();
}

bool Camera::Init()
{
	mode_ = MODE::FIXED;

	prevPos_ = pos_ = FIXED_CAMERA_LOCAL_POS;
	prevTargetPos_ = targetPos_ = FIXED_TARGET_POS;
	prevAngles_ = angles_ = FIXED_CAMERA_ANGLES;

	followTarget_ = nullptr;

	return true;
}

void Camera::Update()
{
	prevPos_ = pos_;
	prevTargetPos_ = targetPos_;
	prevAngles_ = angles_;

	if (!SceneManager::GetInstance().IsPause()) switch (mode_)
	{
	case MODE::FIXED:
		UpdateFixed(); break;
	case MODE::FOLLOW1:
		UpdateFollow1(); break;
	case MODE::FOLLOW2:
		UpdateFollow2(); break;
	}

	UpdateAngles();

	shakeTimer_ -= SceneManager::GetInstance().GetDeltaTime();
	if (shakeTimer_ < 0.0f) shakeTimer_ = 0.0f;

	if (mode_ == MODE::FOLLOW1)
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

	if (!followTarget_)
	{
		DrawFormatString(900, 160, 0xffffff, "Follow: nil");
	}
	else
	{
		DrawFormatString(900, 160, 0xffffff,
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

void Camera::SetFollowTarget(const Transform3D* t)
{
	if (!t)
	{
		followTarget_ = nullptr;
		ChangeCameraMode(MODE::FIXED);
		return;
	}

	followTarget_ = t;
	ChangeCameraMode(MODE::FOLLOW2);
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

bool Camera::IsInCameraBounds(Vector3 pos)
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

	pos_ = (qRot * FOLLOW1_CAMERA_LOCAL_POS);
}

void Camera::UpdateFollow1()
{
	// フォロー先が無いやん！となったら一時固定カメラで
	if (followTarget_ == nullptr)
	{
		UpdateFixed();
		return;
	}

	Quaternion qRot = AngleAxis(Vector3(0, 1, 0), angles_.y) * AngleAxis(Vector3(1, 0, 0), angles_.x);

	auto fPos = followTarget_->position;

	targetPos_ = fPos + qRot * FOLLOW1_TARGET_LOCAL_POS;

	pos_ = fPos + (qRot * FOLLOW1_CAMERA_LOCAL_POS);
}

void Camera::UpdateFollow2()
{
	// フォロー先が無いやん！となったら一時固定カメラで
	if (followTarget_ == nullptr)
	{
		UpdateFixed();
		return;
	}

	Quaternion qRot = AngleAxis(Vector3(0, 1, 0), angles_.y) * AngleAxis(Vector3(1, 0, 0), angles_.x);

	auto fPos = followTarget_->position;
	auto fNeckPos = VECToVec3(MV1GetFramePosition(followTarget_->modelId, 5));

	targetPos_ = fNeckPos + qRot * Vector3(-3, 5, 500);

	pos_ = fNeckPos + qRot * Vector3(-3, 5, 8);
}

void Camera::UpdateAngles()
{
	auto& ins = InputManager::GetInstance();
	const float dt = SceneManager::GetInstance().GetDeltaTime();

	Vector2 mVec = ins.GetMouseMoveLength() * mouseSensitivity_ * dt;
	angles_.x += mVec.y;
	angles_.y += mVec.x;

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
}

void Camera::CollisionStage()
{
	Quaternion qRot = AngleAxis(Vector3(0, 1, 0), angles_.y) * AngleAxis(Vector3(1, 0, 0), angles_.x);

	for (const auto& c : awayColliders_)
	{
		auto& dat = c.lock()->GetColliderData();

		if (dat.type != Collider3D::TYPE::STAGE || dat.modelId == -1) continue;

		auto hitPoly = c.lock()->GetNearestHitPolyLine(
			followTarget_->position + Vector3(0, FOLLOW1_LOCAL_POS_Y, 0),
			pos_ + qRot * Vector3(2, 1, 0)
		);

		if (!hitPoly.HitFlag)
		{
			continue;
		}

		auto pushBack =
			Vector3(-FOLLOW1_LOCAL_POS_X, 0, -FOLLOW1_LOCAL_CAMERA_POS_Z).Normalized() * 0.5f;

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
