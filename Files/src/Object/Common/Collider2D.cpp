#include "Collider2D.h"

Collider2D::Collider2D()
{
}

Collider2D::~Collider2D()
{
	colliders.clear();
}

void Collider2D::AddCollider(const Vector2& center, const Vector2& size)
{
	COLLIDER_DATA data;
	data.type = COLLIDER_TYPE::QUADRANGLE;
	data.center = center;
	data.size = size;

	colliders.push_back(data);
}

void Collider2D::AddCollider(const Vector2& center, float radius)
{
	COLLIDER_DATA data;
	data.type = COLLIDER_TYPE::CIRCLE;
	data.center = center;
	data.radius = radius;

	colliders.push_back(data);
}

std::vector<Collider2D::COLLIDER_DATA> Collider2D::GetColliders() const
{
	return colliders;
}
