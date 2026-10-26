#pragma once
#include <memory>
#include <random>

// (疑似) 乱数生成用のクラスです。Manager系クラス等でインスタンス化して使って下さい
class RNG
{
public:
	RNG();
	~RNG();
	
	// 旧来のrand関数
	int GetRandSimple();
	// unsigned int型のメルセンヌツイスター疑似乱数
	unsigned int GetRandUInt(unsigned int max = UINT_MAX);
	// float型のメルセンヌツイスター疑似乱数
	float GetRandReal(float min = 0.0, float max = 1.0);
	// 32ビットXORシフト疑似乱数
	uint32_t GetXORShift32();
	// 64ビットXORシフト疑似乱数
	uint64_t GetXORShift64();
	// 128ビットXORシフト疑似乱数
	uint32_t GetXORShift128();

private:
	std::unique_ptr<std::mt19937> mt_;
	
	uint32_t seedXOR32_;
	uint64_t seedXOR64_;

	struct uint128_t_rng
	{
		uint32_t vars[4];
	} seedXOR128_;

};