#include "Zobrist.h"
#include <random>

void Zobrist::Setup() noexcept
{
	std::mt19937_64 gen(std::random_device{}());
	std::uniform_int_distribution<unsigned long long> dist(0, ULLONG_MAX);

	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 6; k++)
				RandomValues[i][j][k] = dist(gen);
		}
	}
}
U64 Zobrist::getHash(U64 pieces[2][6], U64 position) noexcept
{
	U64 hash = 0;
	while (position)
	{
		UL index = BSF64(position);
		U64 piece = 1ULL << index;

		for (int i = 0; i < 2; i++)
		{
			bool stop = false;
			for (int j = 0; j < 6; j++)
			{
				if (piece & pieces[i][j])
				{
					hash ^= RandomValues[index][i][j];
					stop = true;
					break;
				}
			}
			if (stop)
				break;
		}
	}

	return hash;
}
U64 Zobrist::getSqrHash(UL index, int player, int piece) noexcept
{
	return RandomValues[index][player][piece];
}