#include "PathFind.h"

PathFind::PathFind()
{
	nodes_.reserve(NODE_COUNT);

	CalcNodeDist();
}

Vector3 PathFind::SearchPath(const Vector3& pos)
{
	return Vector3();
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
		for (int i = 0; i < n; i++)
		{
			// 自分自身に対する距離はゼロ化しておく
			dist[i][i] = 0;

			for (int j = i + 1; j < n; j++)
			{
				// 逆方向にも同じ距離を入力する
				dist[i][j] = dist[j][i] = 1;
			}
		}
	}

	dists_ = move(dist);
}
