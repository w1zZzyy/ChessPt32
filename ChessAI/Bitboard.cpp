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
