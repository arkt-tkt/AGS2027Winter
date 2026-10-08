#pragma once
#include "Common/Object3D.h"

class Stage
{
public:
	Stage();
	virtual ~Stage();

	void Init();
	void Update();
	void Draw();

	const Object3D& GetStageObject() const;

private:
	Object3D stage_;
	Object3D skyDome_;

};