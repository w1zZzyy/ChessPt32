#include "Bitboard.h"

Bitboard::Bitboard(const Bitboard& b)
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 6; j++)
			Pieces[i][j] = b.Pieces[i][j];		
	}

	for (int i = 0; i < 3; i++)
		All[i] = b.All[i];
}

Bitboard& Bitboard::operator=(const Bitboard& b) noexcept
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 6; j++)
			Pieces[i][j] = b.Pieces[i][j];
	}

	for (int i = 0; i < 3; i++)
		All[i] = b.All[i];

	return *this;
}

bool Bitboard::operator==(const Bitboard& b) const noexcept
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			if (Pieces[i][j] != b.Pieces[i][j])
				return false;
		}
	}

	for (int i = 0; i < 3; i++)
		if (All[i] != b.All[i])
			return false;

	return true;
}


PlayerType& operator++(PlayerType& player) noexcept
{
	player = static_cast<PlayerType>(player + 1);
	return player;
}

PieceType& operator++(PieceType& piece) noexcept
{
	piece = static_cast<PieceType>(piece + 1);
	return piece;
}
