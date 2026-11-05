#include <DxLib.h>
#include <queue>
#include "../Common/DxLibUtil.h"
#include "../Common/MathUtil.h"
#include "../Manager/InputManager.h"
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
	if ((InputManager::GetInstance().CheckNowKey(KEY_INPUT_LCONTROL) ||
		InputManager::GetInstance().CheckNowKey(KEY_INPUT_RCONTROL)) &&
		InputManager::GetInstance().CheckNowKey(KEY_INPUT_Z))
	{
		LoadData();
		CalcNodeDist();
	}

	for (auto& node : nodes_)
	{
		DrawSphere3D(Vec3ToVEC(node.second.pos), 3, 8, 0x00ff00u, 0xffffffu, TRUE);

		for (auto& link : node.second.link)
		{
			DrawLine3D(Vec3ToVEC(node.second.pos), Vec3ToVEC(nodes_[link].pos), 0x00ff00u);
		}

		// オーバーレイ表示
		Vector3 pos = node.second.pos + Vector3(0, 5, 0);

		VECTOR scPos = ConvWorldPosToScreenPos(Vec3ToVEC(pos));

		if (!MathUtil::Within(scPos.z, 0.0f, 1.0f)) continue;

		DrawStringToCenterPos(MathUtil::Round(scPos.x), MathUtil::Round(scPos.y),
			std::to_string(node.first).c_str(), 0x00ff00u);
	}
}
#endif

std::vector<int> PathFind::PathGet(const Vector3& start_pos, const Vector3& goal_pos)
{
	std::priority_queue<NodeRecord, std::vector<NodeRecord>, std::greater<NodeRecord>> open_set;

	// 各ノードへの実コスト g(n)
	std::unordered_map<int, float> g_score;
	// 経路復元用の親ノードマップ
	std::unordered_map<int, int> came_from;

	// 初期化
	for (const auto& pair : nodes_)
	{
		g_score[pair.first] = std::numeric_limits<float>::infinity();
	}

	unsigned int start_id = GetNearestNode(start_pos);
	unsigned int goal_id = GetNearestNode(goal_pos);

	if (start_id == (unsigned int)(-1) ||
		goal_id == (unsigned int)(-1))
	{
		return {};
	}

	g_score[start_id] = 0.0f;
	open_set.push({ start_id, (nodes_.at(goal_id).pos - nodes_.at(start_id).pos).Length() });

	while (!open_set.empty())
	{
		NodeRecord current_record = open_set.top();
		open_set.pop();

		int current_id = current_record.id;

		// ゴールに到達した場合
		if (current_id == goal_id)
		{
			std::vector<int> path;
			int curr = goal_id;

			while (curr != start_id)
			{
				path.push_back(curr);
				curr = came_from[curr];
			}

			path.push_back(start_id);

			std::reverse(path.begin(), path.end());
			return path;
		}

		const auto& current_node = nodes_.at(current_id);

		// 隣接ノードの走査
		for (const auto& l : current_node.link)
		{
			unsigned int next_id = l;
			double edge_cost = dists_[current_id][next_id];
			double tentative_g = g_score[current_id] + edge_cost;

			// より短い経路を発見した場合
			if (tentative_g < g_score[next_id])
			{
				came_from[next_id] = current_id;
				g_score[next_id] = tentative_g;
				float f = tentative_g + (nodes_.at(next_id).pos - nodes_.at(goal_id).pos).Length();
				open_set.push({ next_id, f });
			}
		}
	}

	// 経路が見つからない場合
	return {};
}

unsigned int PathFind::GetNearestNode(const Vector3& pos)
{
	unsigned int ret = (unsigned int)(-1);
	float retLen = std::numeric_limits<float>::infinity();

	for (const auto& pair : nodes_)
	{
		// 距離の計算
		float dist = (pair.second.pos - pos).LengthSquare();
		if (dist < retLen)
		{
			ret = pair.first;
			retLen = dist;
		}
	}

	return ret;
}

void PathFind::LoadData()
{
	for (auto& node : nodes_)
	{
		node.second.link.clear();
	}
	nodes_.clear();

	auto& rawData = ResourceManager::GetInstance().Reload(ResourceManager::SRC::JSON_PATHFIND).json_;

	std::string str = "Node_";
	size_t index = 0;

	for (auto& nodes : rawData)
	{
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			auto& node = nodes.at((str + std::to_string(index)).c_str());

			Vector3 pos;
			pos.x = node.at("Position").at(0).get<float>();
			pos.y = node.at("Position").at(1).get<float>();
			pos.z = node.at("Position").at(2).get<float>();

			std::list<unsigned int> link =
				node.at("Link").get<std::list<unsigned int>>();

			nodes_[index].pos = pos;
			nodes_[index].link.insert(nodes_[index].link.end(), link.begin(), link.end());

			for (auto lt : link)
			{
				nodes_[lt].link.emplace_back(index);
			}

			++index;
		}
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
