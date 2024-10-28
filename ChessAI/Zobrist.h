#pragma once
#include "BitWork.h"

class Zobrist
{
private:

	inline static U64 RandomValues[64][2][6];

public:

	static void Setup() noexcept;
	static U64 getHash(U64 pieces[2][6], U64 position) noexcept;
	static U64 getSqrHash(UL index, int player, int piece) noexcept;

};
