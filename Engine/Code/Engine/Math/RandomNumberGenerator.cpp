#include "RandomNumberGenerator.hpp"
#include <cstdlib>

int RandomNumberGenerator::RollRandomIntLessThan(int maxNotInclusive)
{
	return rand() % maxNotInclusive;
}

int RandomNumberGenerator::RollRandomIntInRange(int minInclusive, int maxInclusive)
{
	return rand() % (maxInclusive - minInclusive + 1) + minInclusive;
}

float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) + 1.0f);
}

float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive)
{
	return minInclusive + RandomNumberGenerator::RollRandomFloatZeroToOne() * (maxInclusive - minInclusive);
}
