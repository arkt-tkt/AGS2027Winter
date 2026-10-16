#pragma once
#include "ActorBase3D.h"

class Stage : public ActorBase3D
{
public:
	Stage();
	virtual ~Stage();

	void Update() override;
	void Draw() override;

private:
	Transform3D skyDome_;

	// ƒ‚ƒfƒ‹‰Šú‰»
	void InitModel() override;
	// ƒAƒjƒ‰Šú‰»
	void InitAnim() override;

};