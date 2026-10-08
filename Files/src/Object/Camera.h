#pragma once
#include <DxLib.h>
#include "../Common/Geometry.h"

class Object3D;

class Camera
{
public:
	static constexpr Vector3 LIGHT_DIRECTION = { 0.0f, -1.0f, -0.2f };

	enum class MODE
	{
		NONE,
		FIXED,
		FOLLOW,
		END
	};

	Camera();
	bool Init();
	void Update();

	// カメラおよび3D機能のセットアップ(SetDrawScreen関数使用後は呼び出し必須)
	void BeforeDraw();

#ifdef _DEBUG
	void DebugDraw(); // デバッグ用
#endif

	void SetFollowTarget(const Object3D* = nullptr);
	void ChangeCameraMode(MODE mode);

	const Vector3& GetPosition() const;
	const Vector3& GetTargetPosition() const;
	const Vector3& GetAngles() const;
	MODE GetCameraMode() const;

	void StartShake(float intensity, float duration);

private:
	static constexpr float CAMERA_NEAR_RANGE = 0.01f;
	static constexpr float CAMERA_FAR_RANGE = 2500.0f;

	static constexpr float FOG_START_RANGE = CAMERA_FAR_RANGE * 0.5f;
	static constexpr float FOG_END_RANGE = CAMERA_FAR_RANGE;
	static constexpr int FOG_COLOR[3] = { 0x08, 0x00, 0x10 };

	static constexpr float CAMERA_FOV = 45.0f * DX_PI_F / 180.0f;

	static constexpr Vector3 FIXED_TARGET_POS = { 0.0f, 0.0f, 0.0f };
	static constexpr Vector3 FIXED_CAMERA_LOCAL_POS = { 0.0f, 0.0f, -320.0f };
	static constexpr Vector3 FIXED_CAMERA_ANGLES = { 0.0f, 0.0f, 0.0f };

	static constexpr Vector3 FOLLOW_TARGET_LOCAL_POS = { 30.0f, 40.0f, 1000.0f };
	static constexpr Vector3 FOLLOW_CAMERA_LOCAL_POS = { 30.0f, 70.0f, -150.0f };

	static constexpr float UP_ANGLE_LIMIT = -48.0f * DX_PI_F / 180.0f;
	static constexpr float DOWN_ANGLE_LIMIT = 64.0f * DX_PI_F / 180.0f;

	const Object3D* followTarget_;

	MODE mode_;

	float shakeTimer_ = 0.0f;
	float shakeDuration_ = 0.0f;
	float shakeIntensity_ = 0.0f;

	Vector3 pos_;
	Vector3 prevPos_;

	Vector3 targetPos_;
	Vector3 prevTargetPos_;
	
	// ラジアン度
	Vector3 angles_;
	// ラジアン度
	Vector3 prevAngles_;

	void UpdateFixed();
	void UpdateFollow();

	void UpdateAngles();

#ifdef _DEBUG
	// デバッグ用
	void UpdateDebug();
#endif

	void SetupDxLibCamera();
	void ShakeScreen(Vector3& cam_pos, Vector3& tgt_pos) const;

};
