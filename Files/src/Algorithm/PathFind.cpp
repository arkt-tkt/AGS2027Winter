#include <DxLib.h>
#include "../Common/DxLibUtil.h"
#include "../Manager/ResourceManager.h"
#include "PathFind.h"

PathFind::PathFind()
{
	nodes_.reserve(NODE_COUNT);

	LoadData();
	CalcNodeDist();
}

#ifdef _DEBUG
void PathFind::DrawDebug()
{
	for (auto& node : nodes_)
	{
		DrawSphere3D(Vec3ToVEC(node.pos), 3, 8, 0x00ff00u, 0xffffffu, TRUE);

		for (auto& link : node.link)
		{
			DrawLine3D(Vec3ToVEC(node.pos), Vec3ToVEC(nodes_[link].pos), 0x00ff00u);
		}
	}
}
#endif

Vector3 PathFind::PathGet(const Vector3& pos)
{
	return Vector3();
}

void PathFind::LoadData()
{
	auto& rawData = ResourceManager::GetInstance().Reload(ResourceManager::SRC::JSON_PATHFIND).json_;

	auto& nodes = rawData.at("Nodes");

	std::string str = "Node_";

	for (size_t index = 0; index < nodes.size(); ++index)
	{
		auto& node = nodes.at((str + std::to_string(index)).c_str());

		Vector3 pos;
		pos.x = node.at("Position").at(0).get<float>();
		pos.y = node.at("Position").at(1).get<float>();
		pos.z = node.at("Position").at(2).get<float>();

		std::list<unsigned int> link =
			node.at("Link").get<std::list<unsigned int>>();

		Node ret = { pos, link };

		nodes_.push_back(ret);
	}
}

void PathFind::CalcNodeDist()
{
	size_t n = nodes_.size();

	using namespace std;

	// 各ノード間の接続を制御するため、ノードの数の2乗分だけサイズを確保する
	// ついでに初期化も済ませる
	vector<vector<float>> dist(n, vector<float>(n, INF));

	// 距離データ入力
	{
		// for文の場合、入力は以下の通りにするのが手っ取り早い
		for (unsigned int i = 0; i < n; i++)
		{
			// 自分自身に対する距離はゼロ化しておく
			dist[i][i] = 0;

			for (unsigned int j = i + 1; j < n; j++)
			{
				for (auto l : nodes_[i].link) if (l == j)
				{
					// 逆方向にも同じ距離を入力する
					dist[i][j] = dist[j][i] = (nodes_[j].pos - nodes_[i].pos).Length();
				}
			}
		}
	}

	dists_ = move(dist);
}
