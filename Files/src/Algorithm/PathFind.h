#pragma once
#include <list>
#include <map>
#include <vector>
#include "../Common/Geometry.h"

struct Node
{
	Vector3 pos;
	std::list<unsigned int> link;
};

// ダイクストラ法ベース
class PathFind
{
public:
	PathFind();
	
#ifdef _DEBUG
	void DrawDebug();
#endif

	Vector3 PathGet(const Vector3& pos);

private:
	static constexpr Vector3 PATHFIND_NODE_LOC[] =
	{
		// 道路
		{ -700, 0, 1420 },
		{ -700, 0, 980 },
		{ -700, 0, 670 },
		{ -700, 0, 70 },
		{ -700, 0, -890 },
		{ -700, 0, -1765 },
		{ -700, 0, -2150 },

		// 庁舎前　※未完
		{ -700, 0, 1700 },
		{ -1100, 0, 1700 },
		{ 225, 0, 825 },
		{ 0, 0, 1350 },
		{ 0, 0, 1820 },
		{ 255, 0, 1820 },

		// 駐車場
		{ -300, 0, -50 },
		{ 120, 0, -50 },
		{ 450, 0, -100 },
		{ 570, 0, -235 },
		{ -300, 0, -890 },
		{ 120, 0, -890 },
		{ 255, 0, -890 },
		{ 570, 0, -800 },

		// 公園
		{ 255, 0, -1220 },
		{ -100, 0, -1500 },
		{ -465, 0, -1765 },
		{ 255, 0, -1765 },

		// 路地裏(X+)
		{ 850, 0, 1820 },
		{ 1150, 0, 1500 },
		{ 1150, 0, 740 },
		{ 1150, 0, 550 },
		{ 940, 0, 650 },
		{ 660, 0, 650 },
		{ 1150, 0, -550 },

		// ガソスタ
		{ -1050, 0, 90 },
		{ -1050, 0, 670 },
		{ -1280, 0, 100 },
		{ -1280, 0, 520 },
		{ -1280, 0, 670 },
		{ -1450, 0, 110 },
		{ -1600, 0, 420 },
		{ -1660, 0, 120 },
		{ -1750, 0, 420 },

		// 路地裏(X-)　※未完
		{ -1400, 0, 1700 },
		{ -1700, 0, -140 },
		{ -1920, 0, 1080 },
		{ -2150, 0, -140 },
		{ -2220, 0, 1700 },
		{ -2220, 0, 350 },
	};

	static constexpr size_t NODE_COUNT = _countof(PATHFIND_NODE_LOC);
	static constexpr float INF = 1.0e9f;

	// 各ノードの座標点
	std::vector<Node> nodes_;
	// 各ノード間の距離
	std::vector<std::vector<float>> dists_;

	void LoadData();
	void CalcNodeDist();

};