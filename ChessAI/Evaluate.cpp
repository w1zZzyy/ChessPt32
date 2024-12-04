#include "Evaluate.h"

void Evaluate::init_material_diff()
{
	for (int i = 0; i < 6; i++)
	{
		PieceType pieceT = static_cast<PieceType>(i);
		MaterialDiff += PIECE_VALUES[i] * (BitsCount(position.getPiece(WHITE, pieceT)) - BitsCount(position.getPiece(BLACK, pieceT)));
	}
}

void Evaluate::init_position_diff()
{
	auto helper = [](U64 piece, const int* table) -> int
		{
			int pos = 0;
			while (piece)
			{
				UL index = BSF64(piece);
				pos += table[63 - index];
			}
			return pos;
		};

	for (PieceType pieceT = KING; pieceT <= ROOK; ++pieceT)
		PositioningDiff +=
		helper(position.getPiece(WHITE, pieceT), SquareTables[WHITE][pieceT])
		-
		helper(position.getPiece(BLACK, pieceT), SquareTables[BLACK][pieceT]);
}

Evaluate::Evaluate(const Position& pos) : position(pos)
{
	init_position_diff();
	init_material_diff();
}

Evaluate::Evaluate(const Evaluate& e) 
	: position(e.position), MaterialDiff(e.MaterialDiff), PositioningDiff(e.PositioningDiff)
{}

void Evaluate::make_move(const Move& move)
{
	PlayerType player = position.getPlayer();
	PieceType piece_moved = move.getPieceMoved();

	short from_index = move.getFrom(), to_index = move.getTo();
	int Who2Move = player == WHITE ? 1 : -1;

	switch (move.getType())
	{
	case MOVE_TYPE::CAPTURE:
	{
		PlayerType player2 = static_cast<PlayerType>(player ^ 1);
		PieceType piece_captured = move.getPieceCaptured();

		PositioningDiff += SquareTables[player2][piece_captured][63 - to_index] * Who2Move;
		MaterialDiff += PIECE_VALUES[piece_captured] * Who2Move;

		break;
	}
	case MOVE_TYPE::EN_PASSANT:
	{
		PlayerType player2 = static_cast<PlayerType>(player ^ 1);

		short kill = (player == WHITE)
			? to_index - 8
			: to_index + 8;

		PositioningDiff += SquareTables[player2][PAWN][63 - kill] * Who2Move;
		MaterialDiff += PIECE_VALUES[PAWN] * Who2Move;

		break;
	}
	case MOVE_TYPE::PROMOTION:
	{
		PieceType piece_promoted = move.getPiecePromotion();
		PositioningDiff += (SquareTables[player][piece_promoted][63 - to_index] - SquareTables[player][PAWN][63 - from_index]) * Who2Move;
		MaterialDiff += (PIECE_VALUES[piece_promoted] - PIECE_VALUES[PAWN]) * Who2Move;

		short piece_captured = move.getPieceCaptured();
		if (piece_captured)
		{
			PlayerType player2 = static_cast<PlayerType>(player ^ 1);
			PieceType piece_captured = move.getPieceCaptured();

			PositioningDiff += SquareTables[player2][piece_captured][63 - to_index] * Who2Move;
			MaterialDiff += PIECE_VALUES[piece_captured] * Who2Move;
		}

		return;
	}
	case MOVE_TYPE::CASTLE_KING:
	{
		PositioningDiff +=
			(SquareTables[player][ROOK][62 - to_index]
				-
				SquareTables[player][ROOK][64 - to_index])
			* Who2Move;
		break;
	}
	case MOVE_TYPE::CASTLE_QUEEN:
	{
		PositioningDiff +=
			(SquareTables[player][ROOK][64 - to_index]
				-
				SquareTables[player][ROOK][61 - to_index])
			* Who2Move;
		break;
	}

	default:
		break;
	}

	PositioningDiff += (SquareTables[player][piece_moved][63 - to_index] - SquareTables[player][piece_moved][63 - from_index]) * Who2Move;

	position.make_move(move);
}


float Evaluate::eval() const
{
	return	MaterialDiff + PositioningDiff * WEIGHT::POSITIONING;
}


