#include "RNG.h"

RNG::RNG()
{
	std::random_device rd;

	mt_ = std::make_unique<std::mt19937>(rd());

	std::uniform_int_distribution<uint32_t> dist(0U, UINT_MAX);
	std::uniform_int_distribution<uint32_t> dist32(1U, UINT_MAX);
	std::uniform_int_distribution<uint64_t> dist64(1U, ULLONG_MAX);

	srand(dist(*mt_));

	seedXOR32_ = dist32(*mt_);
	seedXOR64_ = dist64(*mt_);

	for (size_t i = 0; i < 4; ++i)
	{
		seedXOR128_.vars[i] = dist32(*mt_);
	}
}

RNG::~RNG()
{
}

int RNG::GetRandSimple()
{
	return rand();
}

unsigned int RNG::GetRandUInt(unsigned int max)
{
	std::uniform_int_distribution<unsigned int> dist(0U, max);
	return dist(*mt_);
}

float RNG::GetRandReal(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(*mt_);
}

uint32_t RNG::GetXORShift32()
{
	uint32_t x = seedXOR32_;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return seedXOR32_ = x;
}

uint64_t RNG::GetXORShift64()
{
	uint64_t x = seedXOR64_;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	return seedXOR64_ = x;
}

uint32_t RNG::GetXORShift128()
{
	uint32_t t = seedXOR128_.vars[3];

	uint32_t s = seedXOR128_.vars[0];
	seedXOR128_.vars[3] = seedXOR128_.vars[2];
	seedXOR128_.vars[2] = seedXOR128_.vars[1];
	seedXOR128_.vars[1] = s;

	t ^= t << 11;
	t ^= t >> 8;
	return seedXOR128_.vars[0] = t ^ s ^ (s >> 19);
}
