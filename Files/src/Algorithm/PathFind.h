#pragma once
#include <list>
#include <unordered_map>
#include <vector>
#include "../Common/Geometry.h"

struct Node
{
	Vector3 pos;
	std::list<unsigned int> link;
};

// 優先度付きキューで管理するレコード
struct NodeRecord
{
	unsigned int id;
	float f_cost; // f(n) = g(n) + h(n)

	// 優先度付きキューを昇順（最小値を取り出す）にするための比較演算子
	bool operator>(const NodeRecord& other) const
	{
		return f_cost > other.f_cost;
	}
};

// ダイクストラ法ベース
class PathFind
{
public:
	PathFind();
	
#ifdef _DEBUG
	void DrawDebug();
#endif

	std::vector<int> PathGet(const Vector3& start_pos, const Vector3& goal_pos);

	Vector3 NodeToPos(unsigned int node_id)
	{
		return nodes_.at(node_id).pos;
	}

	unsigned int GetNearestNode(const Vector3& pos);

private:
	static constexpr size_t NODE_COUNT = 64ull;
	static constexpr float INF = 1.0e9f;

	// 各ノードの座標点
	std::unordered_map<int, Node> nodes_;
	// 各ノード間の距離
	std::vector<std::vector<float>> dists_;

	void LoadData();
	void CalcNodeDist();

};