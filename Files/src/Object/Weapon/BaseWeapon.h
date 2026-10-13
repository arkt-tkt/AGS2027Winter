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
	Object3D object3D_;
	Vector3 muzzlePos_;

	float damage_;

	int clipSize_;
	int maxClipSize_;

	float reloadTime_;

	float moveSpeedMult_;

};