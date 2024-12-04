#pragma once
#include <cstdint>
#include <intrin.h>
#include <bit>


using U64 = uint64_t;
using UL = unsigned long;

enum MOVE_TYPE
{
	null = 0b0000,
	CAPTURE = 0b0001,
	EN_PASSANT = 0b0010,
	DOUBLE_PUSH = 0b0011,
	CASTLE_KING = 0b0100,
	CASTLE_QUEEN = 0b0101,
	PROMOTION = 0b0110,
};
namespace Edges
{
	constexpr U64 NotFileA = 0b0111111101111111011111110111111101111111011111110111111101111111;
	constexpr U64 NotFileH = 0b1111111011111110111111101111111011111110111111101111111011111110;

	constexpr U64 Rank1 = 0x00000000000000FF; 
	constexpr U64 Rank2 = 0x000000000000FF00;
	constexpr U64 Rank7 = 0x00FF000000000000;
	constexpr U64 Rank8 = 0xFF00000000000000;

	constexpr U64 H1 = 0x0000000000000001;
	constexpr U64 A1 = 0x0000000000000080;
	constexpr U64 H8 = 0x0100000000000000; 
	constexpr U64 A8 = 0x8000000000000000;

	constexpr U64 Files[8] =
	{
		0x8080808080808080,
		0x4040404040404040,
		0x2020202020202020,
		0x1010101010101010,
		0x0808080808080808,
		0x0404040404040404,
		0x0202020202020202,
		0x0101010101010101, 
	};

	constexpr U64 FULL = ~0ULL;

};

enum class GameStage
{
	opening,
	middlegame,
	endgame
};



UL BSF64(U64& pos) noexcept;
UL BSF64_safe(const U64& pos) noexcept;

int BitsCount(U64 pos) noexcept;