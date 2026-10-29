#pragma once
#include <DxLib.h>
#include <vector>
#include "../Common/Geometry.h"

class Collider3D;
class Transform3D;

class Camera
{
public:
	static constexpr Vector3 LIGHT_DIRECTION = { 0.0f, -1.0f, -0.2f };

	enum class MODE
	{
		NONE,
		FIXED,
		FOLLOW1,
		FOLLOW2,
		END
	};

	Camera();
	~Camera();

	bool Init();
	void Update();
	// カメラおよび3D機能のセットアップ(SetDrawScreen関数使用後は呼び出し必須)
	void BeforeDraw();

#ifdef _DEBUG
	void DebugDraw(); // デバッグ用
#endif

	void AddAwayCollider(std::weak_ptr<Collider3D> col);
	void AddAwayColliders(std::vector<std::shared_ptr<Collider3D>> cols);
	void ResetAwayColliders();

	void SetFollowTarget(const Transform3D* = nullptr);
	void ChangeCameraMode(MODE mode);
	void SwitchFollowMode();
	
	// カメラ領域内に座標点が存在するかどうかを判定
	bool IsInCameraBounds(Vector3 pos);

	const Vector3& GetPosition() const { return pos_; }
	const Vector3& GetTargetPosition() const { return targetPos_; }
	const Vector3& GetAngles() const { return angles_; }
	const MODE& GetCameraMode() const { return mode_; }

	void StartShake(float intensity, float duration);

private:
	static constexpr float CAMERA_NEAR_RANGE = 1.0f;
	static constexpr float CAMERA_FAR_RANGE = 3000.0f;

	static constexpr float FOG_START_RANGE = 1333.0f;
	static constexpr float FOG_END_RANGE = 2000.0f;
	static constexpr int FOG_COLOR[3] = { 0x08, 0x00, 0x10 };

	static constexpr float CAMERA_FOV = 75.0f * DX_PI_F / 180.0f;

	static constexpr Vector3 FIXED_TARGET_POS = { 0.0f, 0.0f, 0.0f };
	static constexpr Vector3 FIXED_CAMERA_LOCAL_POS = { 0.0f, 0.0f, -320.0f };
	static constexpr Vector3 FIXED_CAMERA_ANGLES = { 0.0f, 0.0f, 0.0f };

	static constexpr float FOLLOW1_LOCAL_POS_X = 35.0f;
	static constexpr float FOLLOW1_LOCAL_POS_Y = 60.0f;
	static constexpr float FOLLOW1_LOCAL_TARGET_POS_Z = 500.0f;
	static constexpr float FOLLOW1_LOCAL_CAMERA_POS_Z = -100.0f;
	static constexpr Vector3 FOLLOW1_TARGET_LOCAL_POS = {
		FOLLOW1_LOCAL_POS_X, FOLLOW1_LOCAL_POS_Y, FOLLOW1_LOCAL_TARGET_POS_Z };
	static constexpr Vector3 FOLLOW1_CAMERA_LOCAL_POS = {
		FOLLOW1_LOCAL_POS_X, FOLLOW1_LOCAL_POS_Y, FOLLOW1_LOCAL_CAMERA_POS_Z };

	static constexpr float MOUSE_SENSITIVITY_INIT = 0.1f;

	static constexpr float UP_ANGLE_LIMIT = -64.0f * DX_PI_F / 180.0f;
	static constexpr float DOWN_ANGLE_LIMIT = 64.0f * DX_PI_F / 180.0f;

	const Transform3D* followTarget_;

	std::vector<std::weak_ptr<Collider3D>> awayColliders_;

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

	float mouseSensitivity_ = MOUSE_SENSITIVITY_INIT;

	void UpdateDxLibCamera();

	void UpdateFixed();
	void UpdateFollow1();
	void UpdateFollow2();

	void UpdateAngles();

	void CollisionStage();
	void CollisionPlayer();

	void ShakeScreen(Vector3& cam_pos, Vector3& tgt_pos) const;

};
