#pragma once

#include "Bitboard.h"
#include "Move.h"
#include "Threats.h"
#include "PreCalculated.h"
#include "Zobrist.h"

#include <string>
#include <mutex>
#include <memory>


//extern std::atomic<int> CAPTURES, PASSANTS, PROMOTIONS, CASTLES, NULLS;

class Position
{
private:

	Bitboard pieces;
	PlayerType player;
	short castles;
	short en_passant;
	Zobrist zobrist;

	mutable Threats threats; // шахи, атаки вражеских фигур, связки

	void setThreats() const noexcept;
	void setBasicMove(Move& move) const noexcept; // либо взятие, либо шах

	template<PieceType pieceT>
	void GeneratePieceMoves(U64 piece, MoveList* Moves) const noexcept;
	void GenerateKingMoves(U64 piece, MoveList* Moves) const noexcept;
	template<PlayerType playerT>
	void GeneratePawnMoves(U64 pawns, MoveList* Moves) const noexcept;

	// возвращает доску из связанных пешек и внутри функции генерирует для них ходы
	template<PlayerType playerT>
	U64 PinnedPawnsMoves(U64 pawns, MoveList* Moves) const noexcept;

	// ходы пешек
	template<PlayerType playerT>
	U64 PawnSinglePush(U64 piece, U64 free) const noexcept;
	template<PlayerType playerT>
	U64 PawnDoublePush(U64 piece, U64 free) const noexcept;
	template<PlayerType playerT>
	U64 PawnRightCapture(U64 piece) const noexcept;
	template<PlayerType playerT>
	U64 PawnLeftCapture(U64 piece) const noexcept;

	template<CASTLE_TYPE c, PlayerType playerT>
	bool Castle() const noexcept;

public:

	Position(const Position& pos);
	Position(const std::string& FEN);
	Position& operator = (const Position& pos) noexcept;

	MoveList GenerateMoves() const noexcept;
	Position& make_move(const Move& move) noexcept;

	int CountPoses(int depth) const noexcept;

	inline PlayerType getPlayer() const noexcept { return player; }
	inline U64 getPiece(PlayerType playerT, PieceType pieceT) const noexcept { return pieces(playerT, pieceT); }
	inline U64 getBoard(PlayerType playerT) const noexcept { return pieces(playerT); }
	inline bool checkmate() const noexcept { return (threats.opp_attacks & pieces(player, KING)) != 0; }
	inline const Zobrist& getKey() const noexcept { return zobrist; }

	char getPieceFEN(UL index) const noexcept;
	void print() const noexcept;

	// ЗАПУСКАТЬ ТОЛЬКО КОГДА ДВИЖОК НЕ ИЩЕТ ЛУЧШИЙ ХОД 
	// ЭТА ФУНКЦИЯ НУЖНА ТОЛЬКО ДЛЯ ГРАФИКИ
	MoveList getPieceMoves(UL index) const noexcept;
};




template<PieceType pieceT>
inline void Position::GeneratePieceMoves(U64 piece, MoveList* Moves) const noexcept
{
	// для короля и для пешек отдельно
	static_assert(pieceT != PAWN && pieceT != KING);

	U64 rav = 0; // range of acceptable values (ОДЗ)
	switch (threats.checks_counter)
	{
	case 0:
		rav = ~pieces(player);
		break;
	case 1:
		rav = threats.sqrs_to_deffend;
		break;
	case 2:
		return;
	default:
		break;
	}

	while (piece)
	{
		UL from = BSF64(piece);

		U64 acceptable = (threats.pin_table[from]) ? rav & threats.pin_table[from] : rav;
		U64 moves = PRE_CALC_ATTACKS::getAttacks<pieceT>(from, pieces(BOTH)) & acceptable;

		while (moves)
		{
			UL to = BSF64(moves);
			Move move(from, to, pieceT);
			setBasicMove(move);
			Moves->push_back(move);
		}
	}
}

template<PlayerType playerT>
inline void Position::GeneratePawnMoves(U64 pawns, MoveList* Moves) const noexcept
{
	static_assert(playerT != BOTH);

	if (threats.checks_counter == 2)
		return;

	pawns ^= PinnedPawnsMoves<playerT>(pawns, Moves);

	constexpr U64 last_rank = (playerT == WHITE) ? Edges::Rank8 : Edges::Rank1;
	constexpr PlayerType player2 = (playerT == WHITE) ? BLACK : WHITE;

	auto move_gen = [&Moves, &last_rank, this](U64 moves, int move_back, MOVE_TYPE type)// move_back нужен для того чтобы понять найти связь между to и from (to -> from)
	{
		while (moves)
		{
			UL to = BSF64(moves);
			UL from = to + move_back;
			U64 t64 = 1ULL << to;

			Move move(from, to, PAWN);

			if (type == MOVE_TYPE::CAPTURE)
			{
				for (int i = 1; i < 6; i++)
				{
					if (pieces(player2, static_cast<PieceType>(i)) & t64)
					{
						move.setCapture(i);
						break;
					}
				}
			}

			if (last_rank & t64)
			{
				move.setType(MOVE_TYPE::PROMOTION);
				for (int i = 2; i < 6; i++)
				{
					move.setProm(i);
					Moves->push_back(move);
				}
			}
			else
			{
				move.setType(type);
				Moves->push_back(move);
			}
		}
	};
	auto passant_gen = [&Moves, &player2, this](int passant, int from, int kill)
	{
			if (threats.checks_counter == 1)
			{
				if (1ULL << kill == threats.sqrs_to_deffend)
					Moves->emplace_back(Move(from, passant, MOVE_TYPE::EN_PASSANT, PAWN));
				return;
			}

			U64 blocks = pieces(BOTH) ^ (1ULL << from | 1ULL << kill);
			if (PRE_CALC_ATTACKS::getAttacks<ROOK>(pieces(playerT, KING), blocks) & (pieces(player2, ROOK) | pieces(player2, QUEEN)))
				return;

			Moves->emplace_back(Move(from, passant, MOVE_TYPE::EN_PASSANT, PAWN));
	};
	
	U64 free_sqrs = ~pieces(BOTH);
	U64 sqrs_to_deffend = threats.checks_counter == 0 ? Edges::FULL : threats.sqrs_to_deffend;

	U64 forward1 = PawnSinglePush<playerT>(pawns, free_sqrs) & sqrs_to_deffend; 
	U64 forward2 = PawnDoublePush<playerT>(pawns, free_sqrs) & sqrs_to_deffend; 
	U64 captureL = PawnLeftCapture<playerT>(pawns); 
	U64 captureR = PawnRightCapture<playerT>(pawns); 

	constexpr int move_back_f1 = (playerT == WHITE) ? -8 : 8;
	constexpr int move_back_f2 = (playerT == WHITE) ? -16 : 16;
	constexpr int move_back_cl = (playerT == WHITE) ? -9 : 7;
	constexpr int move_back_cr = (playerT == WHITE) ? -7 : 9;

	move_gen(forward1, move_back_f1, MOVE_TYPE::null);
	move_gen(forward2, move_back_f2, MOVE_TYPE::DOUBLE_PUSH);
	move_gen(captureL & pieces(player2) & sqrs_to_deffend, move_back_cl, MOVE_TYPE::CAPTURE);
	move_gen(captureR & pieces(player2) & sqrs_to_deffend, move_back_cr, MOVE_TYPE::CAPTURE);

	if (en_passant == -1)
		return;

	U64 pass = 1ULL << en_passant;
	if (captureL & pass)
		passant_gen(en_passant, en_passant + move_back_cl, en_passant + move_back_f1);
	if (captureR & pass)
		passant_gen(en_passant, en_passant + move_back_cr, en_passant + move_back_f1);
	
}

template<PlayerType playerT>
inline U64 Position::PinnedPawnsMoves(U64 pawns, MoveList* Moves) const noexcept
{
	U64 pinned = pawns & threats.pinned;
	if (pinned == 0 || threats.checks_counter != 0)
		return pinned;

	U64 passant						=				 (en_passant != -1) ? 1ULL << en_passant : 0;
	PlayerType player2				=				 static_cast<PlayerType>(player ^ 1);
	U64 free						=				 ~pieces(BOTH);
	constexpr U64 last_rank			=				 (playerT == WHITE) ? Edges::Rank8 : Edges::Rank1;

	auto MoveTypeHelper = [&](U64 moves, UL from, MOVE_TYPE type)
	{
		while (moves)
		{
			UL to = BSF64(moves);
			U64 t64 = 1ULL << to;

			Move move(from, to, PAWN);

			if (type == MOVE_TYPE::CAPTURE)
			{
				if (pieces(player2, BISHOP) & t64)
					move.setCapture(BISHOP);
				else
					move.setCapture(QUEEN);
			}

			if (last_rank & t64)
			{
				move.setType(MOVE_TYPE::PROMOTION);
				for (int i = 2; i < 6; i++)
				{
					move.setProm(i);
					Moves->push_back(move);
				}
			}
			else
			{
				move.setType(type);
				Moves->push_back(move);
			}
		}
	};

	auto GeneratePinnedMoves = [&](U64 pinned_sqrs, UL from)
	{
		U64 pinned_pawn		 =		 1ULL << from;
		U64 captures		 =		 PRE_CALC_ATTACKS::getAttacks<PAWN, playerT>(from) & pinned_sqrs;
		U64 single_push		 =		 PawnSinglePush<playerT>(pinned_pawn, free) & pinned_sqrs;
		U64 double_push		 =		 PawnDoublePush<playerT>(pinned_pawn, free) & pinned_sqrs;

		MoveTypeHelper(captures & pieces(player2), from, CAPTURE);
		MoveTypeHelper(single_push, from, null);
		MoveTypeHelper(double_push, from, DOUBLE_PUSH);

		if(captures & passant)
			Moves->emplace_back(Move(from, passant, MOVE_TYPE::EN_PASSANT, PAWN));
	};

	U64 pinned_ret = pinned;
	{
		while (pinned)
		{
			UL index = BSF64(pinned);
			U64 pinned_sqrs = threats.pin_table[index];
			if (pinned_sqrs)
				GeneratePinnedMoves(pinned_sqrs, index);
		}
	}
	return pinned_ret;
}

template<PlayerType playerT>
inline U64 Position::PawnSinglePush(U64 piece, U64 free) const noexcept
{
	static_assert(playerT != BOTH);
	if constexpr (playerT == WHITE)
		return (piece << 8) & free;
	return (piece >> 8) & free;
}
template<PlayerType playerT>
inline U64 Position::PawnDoublePush(U64 piece, U64 free) const noexcept
{
	static_assert(playerT != BOTH);
	constexpr U64 rank = (playerT == WHITE) ? Edges::Rank2 : Edges::Rank7;
	U64 single = PawnSinglePush<playerT>(piece & rank, free);
	return PawnSinglePush<playerT>(single, free);
}
template<PlayerType playerT>
inline U64 Position::PawnRightCapture(U64 piece) const noexcept
{
	if constexpr (playerT == WHITE)
		return (piece & Edges::NotFileH) << 7;
	return (piece & Edges::NotFileH) >> 9;
}
template<PlayerType playerT>
inline U64 Position::PawnLeftCapture(U64 piece) const noexcept
{
	if constexpr (playerT == WHITE)
		return (piece & Edges::NotFileA) << 9;
	return (piece & Edges::NotFileA) >> 7;
}


template<CASTLE_TYPE c, PlayerType playerT>
inline bool Position::Castle() const noexcept
{
	constexpr U64 mask =
		(playerT == WHITE)
		? ((c == CASTLE_TYPE::Short) ? CASTLE::KING_mask : CASTLE::QUEEN_mask)
		: ((c == CASTLE_TYPE::Short) ? CASTLE::king_mask : CASTLE::queen_mask);
	constexpr U64 blockers =
		(playerT == WHITE)
		? ((c == CASTLE_TYPE::Short) ? CASTLE::KING_blockers : CASTLE::QUEEN_blockers)
		: ((c == CASTLE_TYPE::Short) ? CASTLE::king_blockers : CASTLE::queen_blockers);
	constexpr U64 castle_sqrs =
		(playerT == WHITE)
		? ((c == CASTLE_TYPE::Short) ? CASTLE::KING_blockers : CASTLE::QUEEN_blockers ^ (1ULL << 6))
		: ((c == CASTLE_TYPE::Short) ? CASTLE::king_blockers : CASTLE::queen_blockers ^ (1ULL << 62));

	if ((mask & castles) == 0)
		return false;
	if (blockers & pieces(BOTH))
		return false;
	if (threats.checks_counter != 0)
		return false;
	if (threats.opp_attacks & castle_sqrs)
		return false;

	return true;
}





static int CountPoses1(int depth, Position& position) noexcept
{
	if (depth == 0)
		return 1;

	int counter = 0;
	MoveList Moves = position.GenerateMoves();

	for (int i = 0; i < Moves.getSize(); i++)
	{
		Position temp(position);
		counter += CountPoses1(depth - 1, temp.make_move(Moves[i]));
	}

	return counter;
}