#include <DxLib.h>
#include "Object2D.h"

Object2D::Object2D()
{
	handleId = -1;
	//handleIds = nullptr;

	position = Vector2();
	scale = 1.0;
	rotation = 0.0;
}

Object2D::Object2D(int image_id)
{
	handleId = image_id;
	//handleIds = nullptr;

	position = Vector2();
	scale = 1.0;
	rotation = 0.0;
}

Object2D::Object2D(const std::vector<int>& image_ids)
{
	handleId = -1;
	handleIds = image_ids;

	position = Vector2();
	scale = 1.0;
	rotation = 0.0;
}
