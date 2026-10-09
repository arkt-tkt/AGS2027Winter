#pragma once
#include <vector>
#include "../../Common/Geometry.h"

class Collider2D
{
public:
	enum class COLLIDER_TYPE
	{
		NONE,

		QUADRANGLE,
		CIRCLE,
	
		END
	};

	enum class HIT_TYPE
	{
		NONE,

		STAGE,
		MODEL_PLAYER,
		ENEMY,
		ENEMY_ALT,
		PLAYER_BULLET,
		ENEMY_BULLET,

		END
	};

	struct COLLIDER_DATA
	{

		COLLIDER_TYPE type;
		// コリジョンの相対的中心座標（オブジェクトの中心座標を0, 0とする）
		Vector2 center;
		// コリジョンのサイズ（左右・上下に均等に拡張される）
		Vector2 size;

		float radius;
	};

	Collider2D();
	virtual ~Collider2D();

	void AddCollider(const Vector2& center, const Vector2& size);
	void AddCollider(const Vector2& center, float radius);

	std::vector<COLLIDER_DATA> GetColliders() const;

private:
	std::vector<COLLIDER_DATA> colliders;

};
