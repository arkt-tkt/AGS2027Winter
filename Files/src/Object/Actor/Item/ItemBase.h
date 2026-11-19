#pragma once
#include "../../../Common/Geometry.h"
#include "../../../Manager/InputManager.h"
#include "../ActorBase3D.h"

class ItemBase : public ActorBase3D
{
public:
	enum class SHOT_TYPE
	{
		SEMI_AUTO,
		FULL_AUTO
	};

	struct WeaponData
	{
		SHOT_TYPE type = SHOT_TYPE::FULL_AUTO;
		float damage = 36;
		int shots = 1;
		int clipSize = 30;
		int reserve = 150;
		float spread = 0.01f;
		float fireRate = 60.0f / 825.0f;
		float velocity = 5000.0f;
		float reloadTime = 2.5f;
		float moveSpeedMult = 1.0f;
	};

	virtual ~ItemBase() {}

	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void DrawUI();

	virtual void Main(int pad_num, InputManager::TAGS tag);
	virtual void Sub();
	virtual void Alt();
	virtual void Reload();

	void Refill();

	void SetParentTransform(Transform3D& parent) { parent_ = &parent; }
	float GetReloadProg() const;

protected:
	Transform3D* parent_;

	Vector3 muzzlePos_;

	WeaponData baseData_;
	WeaponData editData_;

	int ammoClip_;
	int ammoReserve_;
	float shotCT_;
	float reloadProg_;

	void InitParam() override;
	void InitModel() override;
	void InitAnim() override {}

	void TurnVisual() override;

};