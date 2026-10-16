#pragma once
#include "../Common/Transform3D.h"
#include "../../Common/Geometry.h"

class BaseWeapon : public Transform3D
{
public:
	struct WeaponData
	{
		float damage;
		int clipSize;
		int reserve;
		float reloadTime;
		float moveSpeedMult;
	};

	BaseWeapon(WeaponData dat);

	void Update();
	void Draw();

	virtual void Main();
	virtual void Sub();
	virtual void Alt();
	virtual void Reload();

	float GetReloadProg() const;

protected:
	Vector3 muzzlePos_;

	WeaponData baseData_;
	WeaponData editData_;
};