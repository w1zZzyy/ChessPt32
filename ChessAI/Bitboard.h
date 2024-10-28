#pragma once

#include "BitWork.h"
#include <functional>
#include <iostream>

enum PlayerType
{
	WHITE = 0,
	BLACK = 1,
	BOTH = 2
};

enum PieceType
{
	KING = 0b000,
	PAWN = 0b001,
	QUEEN = 0b010,
	KNIGHT = 0b011,
	BISHOP = 0b100,
	ROOK = 0b101,
	NONE = 0b110,
	ALL = 0b111
};

//		  NORTH
// 			^
//			|
//			|
// WEST <--- ---> EAST
//			|
//			|
//			v
//		  SOUTH

enum class Ray
{
	NORTH,
	EAST,
	SOUTH,
	WEST,
	NORTH_WEST,
	NORTH_EAST,
	SOUTH_EAST,
	SOUTH_WEST
};

// медленный подсчет атак 
namespace Slow
{
	template<Ray ray>
	U64 RayAttack(U64 piece, U64 NotBlocked) noexcept;
	template<PieceType pieceT>
	U64 SlideAttacks(U64 piece, U64 blocked) noexcept;

	template<Ray ray>
	inline U64 RayAttack(U64 piece, U64 NotBlocked) noexcept
	{
		U64 _attacks_ = 0;

		while (piece)
		{
			if constexpr (ray == Ray::EAST || ray == Ray::NORTH_EAST || ray == Ray::SOUTH_EAST)
				piece &= Edges::NotFileH;
			if constexpr (ray == Ray::WEST || ray == Ray::NORTH_WEST || ray == Ray::SOUTH_WEST)
				piece &= Edges::NotFileA;
			if constexpr (ray == Ray::NORTH)
				piece <<= 8;
			if constexpr (ray == Ray::EAST)
				piece >>= 1;
			if constexpr (ray == Ray::SOUTH)
				piece >>= 8;
			if constexpr (ray == Ray::WEST)
				piece <<= 1;
			if constexpr (ray == Ray::NORTH_WEST)
				piece <<= 9;
			if constexpr (ray == Ray::NORTH_EAST)
				piece <<= 7;
			if constexpr (ray == Ray::SOUTH_EAST)
				piece >>= 9;
			if constexpr (ray == Ray::SOUTH_WEST)
				piece >>= 7;


			_attacks_ |= piece;
			piece &= NotBlocked;
		}

		return _attacks_;
	}
	template<PieceType pieceT>
	inline U64 SlideAttacks(U64 piece, U64 blocked) noexcept
	{
		U64 free = ~blocked;
		if constexpr (pieceT == ROOK)
			return	
			RayAttack<Ray::EAST>(piece, free) |
			RayAttack<Ray::WEST>(piece, free) |
			RayAttack<Ray::SOUTH>(piece, free) |
			RayAttack<Ray::NORTH>(piece, free);
		if constexpr (pieceT == BISHOP)
			return	
			RayAttack<Ray::NORTH_EAST>(piece, free) |
			RayAttack<Ray::NORTH_WEST>(piece, free) |
			RayAttack<Ray::SOUTH_EAST>(piece, free) |
			RayAttack<Ray::SOUTH_WEST>(piece, free);
	}
}

enum class CASTLE_TYPE
{
	Short,
	Long
};
// маски для рокировок
namespace CASTLE
{
	constexpr short KING_mask = 0b1000;
	constexpr short QUEEN_mask = 0b0100;
	constexpr short king_mask = 0b0010;
	constexpr short queen_mask = 0b0001;

	constexpr U64 KING_blockers = 1ULL << 1 | 1ULL << 2;
	constexpr U64 QUEEN_blockers = 1ULL << 4 | 1ULL << 5 | 1ULL << 6;
	constexpr U64 king_blockers = 1ULL << 57 | 1ULL << 58;
	constexpr U64 queen_blockers = 1ULL << 60 | 1ULL << 61 | 1ULL << 62;

	constexpr short KING_DISABLE = 0b0111;
	constexpr short QUEEN_DISABLE = 0b1011;
	constexpr short king_DISABLE = 0b1101;
	constexpr short queen_DISABLE = 0b1110;
}

constexpr int RookBits[64] =
{
	12, 11, 11, 11, 11, 11, 11, 12,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	12, 11, 11, 11, 11, 11, 11, 12
};

constexpr int BishopBits[64] =
{
	6, 5, 5, 5, 5, 5, 5, 6,
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 5, 5, 5, 5, 5, 5, 6
};

template<PieceType pieceT>
class MagicAttacks // предназначен только для ладьи и слона
{
private:

	U64 attacks[64]; // все атаки фигуры, когда на доске она стоит одна
	U64* magic[64]; // все нападения этой фигуры для текущей позиции

public:

	MagicAttacks(); 
	~MagicAttacks();
	U64 getAttacks(UL index, U64 blockers) const noexcept;
};



template<PieceType pieceT>
inline MagicAttacks<pieceT>::MagicAttacks()
{
	static_assert(pieceT == ROOK || pieceT == BISHOP);

	for (size_t i = 0; i < 64; i++)
		magic[i] = (pieceT == ROOK) ? new U64[1ULL << RookBits[i]] : new U64[1ULL << BishopBits[i]];

	for (size_t i = 0; i < 64; i++)
	{
		U64 piece = 1ULL << i;

		U64 edges = 0;
		if (i >= 8)
			edges |= Edges::Rank1;
		if (i < 56)
			edges |= Edges::Rank8;
		if ((i + 1) % 8 != 0)
			edges |= Edges::Files[0];
		if (i % 8 != 0)
			edges |= Edges::Files[7];

		attacks[i] = Slow::SlideAttacks<pieceT>(piece, 0ULL) & ~edges;

		int bits = (pieceT == ROOK) ? 1ULL << RookBits[i] : 1ULL << BishopBits[i];
		for (int block_sqrs = 0; block_sqrs < bits; block_sqrs++)
		{
			U64 blockers = _pdep_u64(block_sqrs, attacks[i]);
			int key = _pext_u64(blockers, attacks[i]);
			magic[i][key] = Slow::SlideAttacks<pieceT>(piece, blockers);
		}
	}
}

template<PieceType pieceT>
inline MagicAttacks<pieceT>::~MagicAttacks()
{
	for (int i = 0; i < 64; i++)
		delete[] magic[i];
}

template<PieceType pieceT>
inline U64 MagicAttacks<pieceT>::getAttacks(UL index, U64 blockers) const noexcept
{
	blockers &= attacks[index];
	int key = _pext_u64(blockers, attacks[index]);
	return magic[index][key];
}



class Bitboard
{
private:

	U64 Pieces[2][6]{};
	U64 All[3]{};

public:

	// конструкторы
	Bitboard() = default;
	Bitboard(const Bitboard& b);

	// операторы
	inline const	U64& operator () (PlayerType i, PieceType j) const	noexcept { return Pieces[i][j]; }
	inline const	U64& operator () (PlayerType i)				 const	noexcept { return All[i]; }
	inline			U64& operator () (PlayerType i, PieceType j)		noexcept { return Pieces[i][j]; }
	inline			U64& operator () (PlayerType i)						noexcept { return All[i]; }
	Bitboard& operator = (const Bitboard& b) noexcept;

};
