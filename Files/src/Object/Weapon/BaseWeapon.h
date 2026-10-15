#pragma once
#include "../Common/Object3D.h"
#include "../../Common/Geometry.h"

class BaseWeapon : public Object3D
{
public:
	BaseWeapon();

	void Update();
	void Draw();

	virtual void Main();
	virtual void Sub();
	virtual void Alt();
	virtual void Reload();

protected:
	Vector3 muzzlePos_;

	float damage_;

	int clipSize_;
	int maxClipSize_;

	int reserve_;
	int maxReserve_;

	float reloadTime_;

	float moveSpeedMult_;

};