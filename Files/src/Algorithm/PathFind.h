#pragma once
#include <vector>
#include "../Common/Geometry.h"

// ダイクストラ法ベース




class PathFind
{
public:
	PathFind();

	Vector3 SearchPath(const Vector3& pos);

private:
	static constexpr float INF = 1.0e9f;
	static constexpr size_t NODE_COUNT = 48;

	// 各ノードの座標点
	std::vector<Vector3> nodes_;
	// 各ノード間の距離
	std::vector<std::vector<float>> dists_;

	void CalcNodeDist();

};