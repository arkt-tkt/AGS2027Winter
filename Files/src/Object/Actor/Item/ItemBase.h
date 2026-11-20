#pragma once
#include <string>
#include "../../../Common/Geometry.h"
#include "../../../Manager/InputManager.h"
#include "../ActorBase3D.h"

class ItemBase : public ActorBase3D
{
public:
	enum class SHOT_TYPE
	{
		FULL_AUTO = -1,
		SEMI_AUTO = 1,
	};

	struct ItemData
	{

		std::string name = "STG/556-3 'Commando'";
		int price = 200;
		float movement = 1.0f;

		std::vector<int> firemode = { -1, 3, 1 };
		float damage = 36;
		int shots = 1;
		int clipsize = 30;
		int reserve = 150;
		float dispersion = 0.01f;
		float fireRate = 60.0f / 825.0f;
		float velocity = 5000.0f;
		float reload = 2.5f;


	};

	ItemBase(ItemData data) : baseData_(data) {}
	virtual ~ItemBase() {}

	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void DrawUI() override;

	virtual bool Main(int pad_num, InputManager::TAGS tag);
	virtual void Sub();
	virtual void Alt();
	virtual void Reload();
	virtual void CycleFiremode();

	void Refill();

	bool IsFiring() const { return fireCount_ < fireCountTarget_; }
	void SetParentTransform(Transform3D& parent) { parent_ = &parent; }
	float GetReloadProg() const;

protected:
	Transform3D* parent_;

	Vector3 muzzlePos_;

	ItemData baseData_;

	int ammoClip_;
	int ammoReserve_;
	float shotTimer_;
	float reloadProg_ = 0.0f;

	unsigned int firemode_ = 0u;
	int fireCountTarget_ = -1;
	int fireCount_ = -1;

	void InitParam() override;
	void InitModel() override;
	void InitAnim() override {}

	void TurnVisual() override;

};