#include "Engine.h"
#include "ThreadPool.h"
#include <utility>

float Engine::Evaluation(const Position& position) noexcept
{
	int q = BitsCount(position.getPiece(WHITE, QUEEN) | position.getPiece(BLACK, QUEEN));
	int r = BitsCount(position.getPiece(WHITE, ROOK) | position.getPiece(BLACK, ROOK));
	int k = BitsCount(position.getPiece(WHITE, KNIGHT) | position.getPiece(BLACK, KNIGHT));
	int b = BitsCount(position.getPiece(WHITE, BISHOP) | position.getPiece(BLACK, BISHOP));

	GameStage stage =
		(q == 0)
		? (r <= 2)
			? GameStage::endgame
			: GameStage::middlegame
		: (r + k + b == 0)
			? GameStage::endgame
			: GameStage::opening;

	int	  material		=	 MaterialEvaluation(position);
	//float mobility		=	 MobilityEvaluation(position);
	float pawn_struct	=	 PawnStructEvaluation(position);
	float positioning	=
		(stage != GameStage::endgame)
		? PositioningEvaluation<GameStage::opening>(position)
		: PositioningEvaluation<GameStage::endgame>(position);

	return material + /*mobility +*/ pawn_struct + positioning;

}

int Engine::MaterialEvaluation(const Position& position) noexcept
{
	int material = 0;
	for (int i = 0; i < 6; i++)
	{
		PieceType pieceT = static_cast<PieceType>(i);
		material += PIECE_VALUES[i] * (BitsCount(position.getPiece(WHITE, pieceT)) - BitsCount(position.getPiece(BLACK, pieceT)));
	}
	return material;
}

float Engine::MobilityEvaluation(const Position& position) noexcept
{
	// 
	// алгоритм для каждой сета фигур:
	// 1) считаем атаки одной фигуры -> U64
	// 2) считаем количество significant битов -> int
	// 3) считаем контроль центра -> int
	//

	int white_mobility = 0, white_center = 0;
	int black_mobility = 0, black_center = 0;

	MobilityEvaluationHelp<WHITE, KING>(position, white_mobility, white_center),
	MobilityEvaluationHelp<WHITE, PAWN>(position, white_mobility, white_center),
	MobilityEvaluationHelp<WHITE, QUEEN>(position, white_mobility, white_center),
	MobilityEvaluationHelp<WHITE, KNIGHT>(position, white_mobility, white_center),
	MobilityEvaluationHelp<WHITE, BISHOP>(position, white_mobility, white_center),
	MobilityEvaluationHelp<WHITE, ROOK>(position, white_mobility, white_center),
	MobilityEvaluationHelp<BLACK, KING>(position, black_mobility, black_center),
	MobilityEvaluationHelp<BLACK, PAWN>(position, black_mobility, black_center),
	MobilityEvaluationHelp<BLACK, QUEEN>(position, black_mobility, black_center),
	MobilityEvaluationHelp<BLACK, KNIGHT>(position, black_mobility, black_center),
	MobilityEvaluationHelp<BLACK, BISHOP>(position, black_mobility, black_center),
	MobilityEvaluationHelp<BLACK, ROOK>(position, black_mobility, black_center);

	return
		static_cast<float>(white_mobility - black_mobility) * MOBILITY_FACTOR
		+
		static_cast<float>(white_center - black_center) * CENTER_FACTOR;
}

float Engine::PawnStructEvaluation(const Position& position) noexcept
{
	return
		PawnDouble(position, WHITE) - PawnDouble(position, BLACK)
		+
		PawnIsolated(position, WHITE) - PawnIsolated(position, BLACK);
}

float Engine::PawnDouble(const Position& position, PlayerType player) noexcept
{
	int doubled = 0;
	U64 pawns = position.getPiece(player, PAWN);
	for (int i = 0; i < 8; i++)
	{
		U64 on_same_file = Edges::Files[i] & pawns;
		doubled += (BitsCount(on_same_file) - 1);
	}
	return static_cast<float>(doubled) * DOUBLE_PAWN_FACTOR;
}

float Engine::PawnIsolated(const Position& position, PlayerType player) noexcept
{
	int isolated = 0;
	U64 pawns = position.getPiece(player, PAWN);
	for (int i = 1; i < 7; i++)
	{
		U64 neighbours = (Edges::Files[i + 1] | Edges::Files[i - 1]) & pawns;
		if (neighbours == 0 && pawns & Edges::Files[i])
			isolated++;
	}

	if (pawns & Edges::Files[0] && !(Edges::Files[1] & pawns))
		isolated++;
	if (pawns & Edges::Files[7] && !(Edges::Files[6] & pawns))
		isolated++;

	return static_cast<float>(isolated) * ISOLATED_PAWNS_FACTOR;
}






float Engine::minimax(const Position& position, int depth, float alpha, float beta) noexcept
{
	if (depth == 0)
		return Evaluation(position);

	PlayerType player = position.getPlayer();
	float BestEval = player == WHITE ? ALPHA : BETA;

	MoveList Moves = position.GenerateMoves();
	if (Moves.empty())
		return 
		position.getOppAttacks() & position.getPiece(player, KING)
		? BestEval 
		: 0;

	for (int i = 0; i < Moves.getSize(); i++)
	{
		Position temp(position);
		float eval = minimax(temp.make_move(Moves[i]), depth - 1, alpha, beta);
		if (player == WHITE && eval > BestEval)
		{
			alpha = eval;
			BestEval = eval;
		}
		else if (player == BLACK && eval < BestEval)
		{
			beta = eval;
			BestEval = eval;
		}

		if (alpha >= beta)
			return BestEval;
	}

	return BestEval;
}
void Engine::Search(Position& position, int depth) noexcept
{
	if (depth == 0)
		return;

	PlayerType player	=	 position.getPlayer();
	float BestEval		=	 player == WHITE ? ALPHA - 1.0f : BETA + 1.0f;
	Move BestMove;

	MoveList Moves = position.GenerateMoves();
	if (Moves.empty())
		return;

	ThreadPool<float> threads;
	std::vector<std::pair<std::future<float>, Move>> results;

	for (int i = 0; i < Moves.getSize(); i++)
	{
		Position temp(position);
		results.emplace_back(threads.addTask(std::bind(&minimax, temp.make_move(Moves[i]), depth - 1, ALPHA, BETA)), Moves[i]);
	}

	threads.join();
	for (auto& res : results)
	{
		float eval = res.first.get();
		if (player == WHITE && eval > BestEval || player == BLACK && eval < BestEval)
		{
			BestEval = eval;
			BestMove = res.second;
		}
	}
	
	std::cout << BestEval << "\n";
	position.make_move(BestMove);
}
