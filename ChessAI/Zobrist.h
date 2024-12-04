#pragma once
#include "Bitboard.h"

namespace ZOBRIST
{
	static U64 RandomValues[2][6][64];
	void init() noexcept;
}


class Zobrist
{
private:

	U64 key;

public:

	inline Zobrist() : key(0) {}

	void setKey(const Bitboard& position) noexcept;
	inline const U64 getKey() const noexcept { return key; }
	void KeyUpdate(UL sqr, PlayerType player, PieceType piece) noexcept;

	inline Zobrist& operator  =	(const Zobrist& z) noexcept { key  = z.key; return *this; }
	inline Zobrist& operator &= (const Zobrist& z) noexcept { key &= z.key; return *this; }
	inline Zobrist& operator |= (const Zobrist& z) noexcept { key |= z.key; return *this; }
	inline Zobrist& operator ^= (const Zobrist& z) noexcept { key ^= z.key; return *this; }

	inline bool operator == (const Zobrist& z) const noexcept { return key == z.key; }
	inline bool operator != (const Zobrist& z) const noexcept { return key != z.key; }
};
