#pragma once
#include <DxLib.h>
#include <vector>
#include "../../Common/Geometry.h"

class Collider3D
{
public:
	enum class TYPE
	{
		PLAYER,
		ENEMY,
		STAGE,
	};

	enum class SHAPE
	{
		SPHERE,
		LINE,
		CAPSULE,
		CAPSULE_ALT,
		MODEL,
	};

	struct COLLIDER_DATA
	{
		TYPE type;
		SHAPE shape;
		Position3 pos1;
		Position3 pos2;
		float r;
		int handleId;
	};

	// SPHERE
	Collider3D(TYPE type, Position3 local_pos, float r);
	// LINE
	Collider3D(TYPE type, Position3 local_pos1, Position3 local_pos2);
	// CAPSULE
	Collider3D(TYPE type, Position3 local_pos1, Position3 local_pos2, float r);
	// CAPSULE_ALT
	Collider3D(TYPE type, float r);
	// MODEL
	Collider3D(TYPE type, int handle_id);

	~Collider3D();

	void Update(Position3 world_pos, Vector3 world_rot);
	void UpdateAlt(Position3 world_pos1, Vector3 world_pos2);
	void Release();

#ifdef _DEBUG
	void DrawDebug() const;
#endif

	const COLLIDER_DATA& GetColliderData() const;

private:
#ifdef _DEBUG
	static constexpr int COLLIDER_DIV = 8;
	static constexpr unsigned int COLLIDER_COLOR = 0xff8000u;
#endif

	// 基礎データ
	COLLIDER_DATA baseData_;
	// 基礎データに座標や回転情報をブレンドしたもの
	COLLIDER_DATA data_;

	void SharedConstructor();

};