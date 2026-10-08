#pragma once
#include <memory>
#include <vector>
#include "../../Common/Geometry.h"
#include "Collider2D.h"

class Object2D
{
public:
	Object2D();
	Object2D(int image_id);
	Object2D(const std::vector<int>& image_ids);

	int handleId;
	std::vector<int> handleIds;

	Vector2 position;
	double scale;
	double rotation;

	//std::shared_ptr<Collider2D> collider;

};
