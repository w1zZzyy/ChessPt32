#include "Zobrist.h"

#include <random>

void ZOBRIST::init() noexcept
{
	std::mt19937_64 gen(std::random_device{}());
	std::uniform_int_distribution<unsigned long long> dist(0, ULLONG_MAX);

	for (PlayerType player = WHITE; player <= BLACK; ++player)
		for (PieceType piece = KING; piece <= ROOK; ++piece)
			for (size_t sqr = 0; sqr < 64; ++sqr)
				RandomValues[player][piece][sqr] = dist(gen);

}

void Zobrist::setKey(const Bitboard& position) noexcept
{
	auto gen_hash_help = [this](U64 pos, PieceType pieceT, PlayerType playerT)
	{
		while(pos)
		{
			UL index = BSF64(pos);
			key ^= ZOBRIST::RandomValues[playerT][pieceT][index];
		}
	};

	for (PlayerType player = WHITE; player <= BLACK; ++player)
		for (PieceType piece = KING; piece <= ROOK; ++piece)
			gen_hash_help(position(player, piece), piece, player);
}

void Zobrist::KeyUpdate(UL sqr, PlayerType player, PieceType piece) noexcept
{
	key ^= ZOBRIST::RandomValues[player][piece][sqr];
}



