#pragma once

#include "Bitboard.h"

class PRE_CALC_ATTACKS
{
private:

	// хеш таблицы в которых уже рассчитаны всевозможные ходы для каждой позиции
	inline static MagicAttacks<ROOK> RookMagic;
	inline static MagicAttacks<BISHOP> BishopMagic;
	inline static U64 KingAttacks[64];
	inline static U64 KnightAttacks[64];
	inline static U64 PawnAttacks[2][64];

	// клетки между фигурами (например между a1 и a4 - a2, a3, a4)
	inline static U64 Between[64][64];

	// функции для рассчета хеш таблиц
	static void setKingHashTable() noexcept;
	static void setKnightHashTable() noexcept;
	static void setPawnHashTable() noexcept;
	static void setBetween() noexcept;

	static U64 getRookAttacks(UL from, U64 blockers) noexcept { return RookMagic.getAttacks(from, blockers); }
	static U64 getBishopAttacks(UL from, U64 blockers) noexcept { return BishopMagic.getAttacks(from, blockers); }
	static U64 getKingAttacks(UL from) noexcept { return KingAttacks[from]; }
	static U64 getKnightAttacks(UL from) noexcept { return KnightAttacks[from]; }
	template<PlayerType playerT>
	static U64 getPawnAttacks(UL from) noexcept { return PawnAttacks[playerT][from]; }

public:

	static void init() noexcept;
	static U64 getBetween(UL from, UL to) noexcept { return Between[from][to]; }

	// получаем атаки для всех фигур одинакого цвета и типа
	template<PieceType pieceT, PlayerType playerT = BOTH>
	static U64 getAttacks(U64 piece, U64 blockers = 0) noexcept;

	// получаем атаки для одной фигуры, стоящей на клетке from
	template<PieceType pieceT, PlayerType playerT = BOTH>
	static U64 getAttacks(UL from, U64 blockers = 0) noexcept;

};

template<PieceType pieceT, PlayerType playerT>
inline U64 PRE_CALC_ATTACKS::getAttacks(U64 piece, U64 blockers) noexcept
{
	static_assert(pieceT != NONE && pieceT != ALL);
	if constexpr (pieceT == PAWN)
	{
		static_assert(playerT != BOTH);
		if constexpr (playerT == WHITE)
			return (piece & Edges::NotFileA) << 9 | (piece & Edges::NotFileH) << 7;
		return (piece & Edges::NotFileA) >> 7 | (piece & Edges::NotFileH) >> 9;
	}

	U64 attacks = 0;

	while (piece)
	{
		UL from = BSF64(piece);
		attacks |= getAttacks<pieceT, playerT>(from, blockers);
	}

	return attacks;
}

template<PieceType pieceT, PlayerType playerT>
inline U64 PRE_CALC_ATTACKS::getAttacks(UL from, U64 blockers) noexcept
{
	static_assert(pieceT != NONE && pieceT != ALL);

	if constexpr (pieceT == KING)
		return PRE_CALC_ATTACKS::getKingAttacks(from);
	if constexpr (pieceT == PAWN)
	{
		static_assert(playerT != BOTH);
		return PRE_CALC_ATTACKS::getPawnAttacks<playerT>(from);
	}
	if constexpr (pieceT == QUEEN)
		return PRE_CALC_ATTACKS::getRookAttacks(from, blockers) | PRE_CALC_ATTACKS::getBishopAttacks(from, blockers);
	if constexpr (pieceT == KNIGHT)
		return PRE_CALC_ATTACKS::getKnightAttacks(from);
	if constexpr (pieceT == BISHOP)
		return PRE_CALC_ATTACKS::getBishopAttacks(from, blockers);
	if constexpr (pieceT == ROOK)
		return PRE_CALC_ATTACKS::getRookAttacks(from, blockers);

}