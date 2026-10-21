#pragma once
#include "../ActorBase3D.h"

class Projectile : public ActorBase3D
{
public:
	struct Data
	{
		Position3 position;
		Vector3 velocity;
		float radian;
		float gravity;
	};

	void Update() override;
	void Draw() override;

	void SetData(Data dat) { dat_ = dat; }
	bool IsAlive() const { return hp_; }

private:
	Data dat_;
	int hp_;

	void InitOther() override;

};
