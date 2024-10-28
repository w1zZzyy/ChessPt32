#include "PreCalculated.h"

void PRE_CALC_ATTACKS::setKingHashTable() noexcept
{
	for (size_t i = 0; i < 64; i++)
	{
		U64 attack = 0;
		U64 king = 1ULL << i;

		attack |= (king << 8);
		attack |= (king >> 8);
		attack |= ((king & Edges::NotFileA) << 1);
		attack |= ((king & Edges::NotFileH) >> 1);
		attack |= ((king & Edges::NotFileA) >> 7);
		attack |= ((king & Edges::NotFileH) << 7);
		attack |= ((king & Edges::NotFileA) << 9);
		attack |= ((king & Edges::NotFileH) >> 9);

		KingAttacks[i] = attack;
	}
}

void PRE_CALC_ATTACKS::setKnightHashTable() noexcept
{
	constexpr U64 NotFileAB = 0b0011111100111111001111110011111100111111001111110011111100111111;
	constexpr U64 NotFileGH = 0b1111110011111100111111001111110011111100111111001111110011111100;

	for (size_t i = 0; i < 64; i++)
	{
		U64 attack = 0;
		U64 knight = 1ULL << i;

		attack |= ((knight & NotFileAB) << 10);
		attack |= ((knight & Edges::NotFileA) << 17);
		attack |= ((knight & Edges::NotFileH) << 15);
		attack |= ((knight & NotFileGH) << 6);
		attack |= ((knight & NotFileGH) >> 10);
		attack |= ((knight & Edges::NotFileH) >> 17);
		attack |= ((knight & Edges::NotFileA) >> 15);
		attack |= ((knight & NotFileAB) >> 6);

		KnightAttacks[i] = attack;
	}
}

void PRE_CALC_ATTACKS::setPawnHashTable() noexcept
{
	for (size_t i = 0; i < 64; i++)
	{
		U64 pawn = 1ULL << i;
		PawnAttacks[WHITE][i] = (pawn & Edges::NotFileA) << 9 | (pawn & Edges::NotFileH) << 7;
		PawnAttacks[BLACK][i] = (pawn & Edges::NotFileA) >> 7 | (pawn & Edges::NotFileH) >> 9;
	}
}

void PRE_CALC_ATTACKS::setBetween() noexcept
{
	auto helper = [](int from, int to)->U64
		{
			if (from == to)
				return 0ULL;

			int fromY = from / 8, fromX = from % 8;
			int toY = to / 8, toX = to % 8;

			// linear
			if (fromY == toY)
				return (fromX > toX)
				? Slow::RayAttack<Ray::EAST>(1ULL << from, ~(1ULL << to))
				: Slow::RayAttack<Ray::WEST>(1ULL << from, ~(1ULL << to));
			if (fromX == toX)
				return (fromY > toY)
				? Slow::RayAttack<Ray::SOUTH>(1ULL << from, ~(1ULL << to))
				: Slow::RayAttack<Ray::NORTH>(1ULL << from, ~(1ULL << to));

			// diagonal
			if (abs(fromX - toX) != abs(fromY - toY))
				return 0ULL;

			if (fromX > toX)
				return (fromY > toY)
				? Slow::RayAttack<Ray::SOUTH_EAST>(1ULL << from, ~(1ULL << to))
				: Slow::RayAttack<Ray::NORTH_EAST>(1ULL << from, ~(1ULL << to));
			if (fromX < toX)
				return (fromY > toY)
				? Slow::RayAttack<Ray::SOUTH_WEST>(1ULL << from, ~(1ULL << to))
				: Slow::RayAttack<Ray::NORTH_WEST>(1ULL << from, ~(1ULL << to));

			return 0ULL;
		};

	for (int sqr1 = 0; sqr1 < 64; sqr1++)
	{
		for (int sqr2 = 0; sqr2 < 64; sqr2++)
			Between[sqr1][sqr2] = helper(sqr1, sqr2);
	}
}

void PRE_CALC_ATTACKS::init() noexcept
{
	setBetween();
	setKingHashTable();
	setKnightHashTable();
	setPawnHashTable();
}