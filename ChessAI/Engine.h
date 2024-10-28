#pragma once

#include "Position.h"
#include <utility>

namespace SquareTables
{
	constexpr int WhitePawn[64] =
	{
		0,	 0,  0,  0,  0,  0,  0,  0,
		50, 50, 50, 50, 50, 50, 50, 50,
		10, 10, 20, 30, 30, 20, 10, 10,
		 5,  5, 10, 25, 25, 10,  5,  5,
		 0,  0,  0, 25, 25,  0,  0,  0,
		 5, -5,-10,  0,  0,-10, -5,  5,
		 5, 10, 10,-20,-20, 10, 10,  5,
		 0,  0,  0,  0,  0,  0,  0,  0
	};
	constexpr int WhiteKnight[64] =
	{
		-50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50
	};
	constexpr int WhiteBishop[64] =
	{
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20
	};
	constexpr int WhiteRook[64] =
	{
		0,  0,  0,  0,  0,  0,  0,  0,
		5, 10, 10, 10, 10, 10, 10,  5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		0,  0,  0,  5,  5,  0,  0,  0
	};
	constexpr int WhiteQueen[64] =
	{
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		 -5,  0,  5,  5,  5,  5,  0, -5,
		  0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	};
	constexpr int WhiteKingMid[64] =
	{
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		 20, 20,  0,  0,  0,  0, 20, 20,
		 20, 30, 10,  0,  0, 10, 30, 20
	};
	constexpr int WhiteKingEnd[64] =
	{
		-50,-40,-30,-20,-20,-30,-40,-50,
		-30,-20,-10,  0,  0,-10,-20,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-30,  0,  0,  0,  0,-30,-30,
		-50,-30,-30,-30,-30,-30,-30,-50
	};



	constexpr int BlackPawn[64] =
	{
		0,  0,  0,  0,  0,  0,  0,  0,
		5, 10, 10,-20,-20, 10, 10,  5,
		5, -5,-10,  0,  0,-10, -5,  5,
		0,  0,  0, 25, 25,  0,  0,  0,
		5,  5, 10, 25, 25, 10,  5,  5,
		10, 10, 20, 30, 30, 20, 10, 10,
		50, 50, 50, 50, 50, 50, 50, 50,
		0,  0,  0,  0,  0,  0,  0,  0
	};
	constexpr int BlackKnight[64] =
	{
		-50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50
	};
	constexpr int BlackBishop[64] =
	{
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  5,  0,  0,  0,   0,  5,-10,
		-10,  10, 10, 10, 10,  10, 10,-10,
		-10,  5,  10, 10, 10,  10,  5,-10,
		-10,  0, 5, 10, 10, 5,  0,-10,
		-10,  0, 5, 10, 10, 5, 0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-20,-10,-10,-10,-10,-10,-10,-20
	};
	constexpr int BlackRook[64] =
	{
		 0,  0,  0,  5,  5,  0,  0,  0,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		 5, 10, 10, 10, 10, 10, 10,  5,
		 0,  0,  0,  0,  0,  0,  0,  0
	};
	constexpr int BlackQueen[64] =
	{
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		 -5,  0,  5,  5,  5,  5,  0, -5,
		  0,  0,  5,  5,  5,  5,  0, -5,
		-10,  0,  5,  5,  5,  5,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	};
	constexpr int BlackKingMid[64] =
	{
		20, 30, 10,  0,  0, 10, 30, 20,
		20, 20,  0,  0,  0,  0, 20, 20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		 -30,-40,-40,-50,-50,-40,-40,-30,
		 -30,-40,-40,-50,-50,-40,-40,-30,
	};
	constexpr int BlackKingEnd[64] =
	{
		-50,-30,-30,-30,-30,-30,-30,-50
		- 30,-30,  0,  0,  0,  0,-30,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-20,-10,  0,  0,-10,-20,-30,
		-50,-40,-30,-20,-20,-30,-40,-50,
	};

	constexpr int Center[64] =
	{
		0, 0, 0,  0,   0,  0,  0, 0,
		0, 0, 0,  0,   0,  0,  0, 0,
		0, 0, 0,  5,   5,  0,  0, 0,
		0, 0, 25, 50,  50, 25, 0, 0,
		0, 0, 25, 50,  50, 25, 0, 0,
		0, 0, 0,  5,   5,  0,  0, 0,
		0, 0, 0,  0,   0,  0,  0, 0,
		0, 0, 0,  0,   0,  0,  0, 0
	};
}

class Engine
{
private:

	inline static constexpr float ALPHA							=				-10000.0f;
	inline static constexpr float BETA							=				 10000.0f;
	inline static constexpr int   DEPTH							=				 6;
	inline static constexpr float POSITIONING_FACTOR			=				 0.02f;
	inline static constexpr float DOUBLE_PAWN_FACTOR			=				-0.5f;
	inline static constexpr float ISOLATED_PAWNS_FACTOR			=				-0.5f;
	inline static constexpr float MOBILITY_FACTOR				=				 0.1f;
	inline static constexpr float CENTER_FACTOR					=				 0.01f;


	// оценка позиции
	static float	Evaluation(const Position& position) noexcept;
	static int		MaterialEvaluation(const Position& position) noexcept;
	static float	MobilityEvaluation(const Position& position) noexcept;
	template<PlayerType playerT, PieceType pieceT>
	static void		MobilityEvaluationHelp(const Position& position, int& mobility, int& center) noexcept;
	template<GameStage stage>
	static float	PositioningEvaluation(const Position& position) noexcept;
	static float	PawnStructEvaluation(const Position& position) noexcept;
	static float	PawnDouble(const Position& position, PlayerType player) noexcept;
	static float	PawnIsolated(const Position& position, PlayerType player) noexcept;


	// оценка позиции на n ходов вперед
	static float minimax(const Position& position, int depth, float alpha, float beta) noexcept;

public:

	static void Search(Position& position, int depth = DEPTH) noexcept;
	
};




template<PlayerType playerT, PieceType pieceT>
inline void Engine::MobilityEvaluationHelp(const Position& position, int& mobility, int& center) noexcept
{
	U64 piece = position.getPiece(playerT, pieceT);
	while (piece)
	{
		UL index = BSF64(piece);
		U64 attacks = PRE_CALC_ATTACKS::getAttacks<pieceT, playerT>(index, position.getBoard(BOTH));
		while (attacks)
		{
			mobility++;
			UL a_ind = BSF64(attacks);
			center += SquareTables::Center[a_ind];
		}
	}
}

template<GameStage stage>
inline float Engine::PositioningEvaluation(const Position& position) noexcept
{
	auto get_pos = [](U64 piece, const int* table) -> int
	{
		int pos = 0;
		while (piece)
		{
			UL index = BSF64(piece);
			pos += table[63 - index];
		}
		return pos;
	};

	float pos =
		get_pos(position.getPiece(WHITE, PAWN),		SquareTables::WhitePawn)		-		 get_pos(position.getPiece(BLACK, PAWN),	SquareTables::BlackPawn) +
		get_pos(position.getPiece(WHITE, QUEEN),	SquareTables::WhiteQueen)		-		 get_pos(position.getPiece(BLACK, QUEEN),	SquareTables::BlackQueen) +
		get_pos(position.getPiece(WHITE, KNIGHT),	SquareTables::WhiteKnight)		-		 get_pos(position.getPiece(BLACK, KNIGHT),	SquareTables::BlackKnight) +
		get_pos(position.getPiece(WHITE, BISHOP),	SquareTables::WhiteBishop)		-		 get_pos(position.getPiece(BLACK, BISHOP),	SquareTables::BlackBishop) +
		get_pos(position.getPiece(WHITE, ROOK),		SquareTables::WhiteRook)		-		 get_pos(position.getPiece(BLACK, ROOK),	SquareTables::BlackRook);

	if constexpr (stage == GameStage::endgame)
		pos += get_pos(position.getPiece(WHITE, KING), SquareTables::WhiteKingEnd) - get_pos(position.getPiece(BLACK, KING), SquareTables::BlackKingEnd);
	else
		pos += get_pos(position.getPiece(WHITE, KING), SquareTables::WhiteKingMid) - get_pos(position.getPiece(BLACK, KING), SquareTables::BlackKingMid);

	return pos * POSITIONING_FACTOR;
}
