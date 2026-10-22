#pragma once
#include "../../../Common/Geometry.h"
#include "../ActorBase3D.h"

class WeaponBase : public ActorBase3D
{
public:
	struct WeaponData
	{
		float damage = 40;
		int clipSize = 30;
		int reserve = 120;
		float reloadTime = 2.5f;
		float moveSpeedMult = 1.0f;
	};

	virtual ~WeaponBase() {}

	void Update() override;
	void LateUpdate() override;
	void Draw() override;

	virtual void Main();
	virtual void Sub();
	virtual void Alt();
	virtual void Reload();

	void SetParentTransform(Transform3D& parent) { parent_ = &parent; }
	float GetReloadProg() const;

protected:
	Transform3D* parent_;

	Vector3 muzzlePos_;

	WeaponData baseData_;
	WeaponData editData_;

	void InitParam() override {}
	void InitModel() override;
	void InitAnim() override {}

	void TurnVisual() override;

};