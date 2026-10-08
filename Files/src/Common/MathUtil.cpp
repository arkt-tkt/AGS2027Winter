#include "MathUtil.h"

unsigned int MathUtil::LerpColor(const unsigned int& a, const unsigned int& b, float t)
{
	unsigned int red, gre, blu;
	red = std::lerp(a & 0xff0000u, b & 0xff0000u, t);
	gre = std::lerp(a & 0xff00u, b & 0xff00u, t);
	blu = std::lerp(a & 0xffu, b & 0xffu, t);
	return (red & 0xff0000u) + (gre & 0xff00u) + (blu & 0xffu);
}

unsigned int MathUtil::LerpColorMap(const std::map<float, unsigned int>& map, float rate)
{
	std::pair<float, unsigned int> lPair(-1.0f, 0x0u), rPair(-1.0f, 0x0u);

	for (auto it = map.begin(); it != map.end(); )
	{
		if ((*it).first <= rate)
		{
			lPair = (*it);
			++it;
		}
		else if ((*it).first > lPair.first)
		{
			rPair = (*it);
			break;
		}
	}

	// lPair が無い場合
	if (lPair.first == -1.0f)
	{
		// map 内の要素数がゼロである可能性が濃いため、確認する事
		return 0xff00ffu;
	}

	// rPair が無い、または何故か lPair と同じ場合
	if (rPair.first == -1.0f || lPair.first == rPair.first)
	{
		// 前者は正常動作でも起こり得るため、中身が確実に存在する lPair.second を返す
		return lPair.second;
	}

	// ここから LerpColor 関数に渡すための変換処理
	rPair.first -= lPair.first;
	float dRate = (rate - lPair.first) / rPair.first;

	// 戻り値をそのまま返す
	return LerpColor(lPair.second, rPair.second, dRate);
}
