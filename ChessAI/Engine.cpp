#include "Engine.h"
#include "Zobrist.h"

#include <atomic>


std::atomic<int> counter = 0;

float Engine::alpha_beta_MAX(Evaluate& position, int depth, float alpha, float beta) noexcept // ход белых
{
	if (depth == 0)
	{
		counter++;
		return position.eval();
	}

	float eval_best = -INFINITY;

	const Position& pos = position.get_pos();
	const Zobrist& key = pos.getKey();

	{
		auto entry = TPT.find(key);
		if (entry && entry->depth >= depth)
		{
			switch (entry->type)
			{
			case EXACT: 
				return entry->eval;
			case LOWER_BOUND: 
				alpha = std::max(alpha, entry->eval);
				break;
			case UPPER_BOUND:
				beta = std::min(beta, entry->eval);
				break;
			default:
				break;
			}

			if (alpha >= beta)
				return entry->eval;
		}
	}

	MoveList Moves = pos.GenerateMoves();
	if (Moves.empty())
	{
		counter++;
		return pos.checkmate() ? -MATE : 0;
	}
	Moves.sort();

	for (int i = 0; i < Moves.getSize(); i++)
	{
		Evaluate temp(position);
		temp.make_move(Moves[i]);

		float eval = alpha_beta_MIN(temp, depth - 1, alpha, beta);
		if (eval > eval_best)
		{
			eval_best = eval;
			alpha = std::max(alpha, eval);
		}

		if (alpha >= beta)
		{
			TPT.store(key, Entry(key, depth, eval_best, LOWER_BOUND));
			return eval_best;
		}
	}

	counter++;
	TPT.store(key, Entry(key, depth, eval_best, EXACT));
	return eval_best;
}
float Engine::alpha_beta_MIN(Evaluate& position, int depth, float alpha, float beta) noexcept // ход черных
{
	if (depth == 0)
	{
		counter++;
		return position.eval();
	}

	float eval_best = INFINITY;

	const Position& pos = position.get_pos();
	const Zobrist& key = pos.getKey();

	{
		auto entry = TPT.find(key);
		if (entry && entry->depth >= depth)
		{
			switch (entry->type)
			{
			case EXACT: 
				return entry->eval;
			case LOWER_BOUND:
				alpha = std::max(alpha, entry->eval);
				break;
			case UPPER_BOUND:
				beta = std::min(beta, entry->eval);
				break;
			default:
				break;
			}

			if (alpha >= beta)
				return entry->eval;
		}
	}

	MoveList Moves = pos.GenerateMoves();
	if (Moves.empty())
	{
		counter++;
		return pos.checkmate() ? MATE : 0;
	}
	Moves.sort();

	for (int i = 0; i < Moves.getSize(); i++)
	{
		Evaluate temp(position);
		temp.make_move(Moves[i]);

		float eval = alpha_beta_MAX(temp, depth - 1, alpha, beta);
		if (eval < eval_best)
		{
			eval_best = eval;
			beta = std::min(beta, eval_best);
		}

		if (alpha >= beta)
		{
			TPT.store(key, Entry(key, depth, eval_best, UPPER_BOUND));
			return eval_best;
		}
	}

	counter++;
	TPT.store(key, Entry(key, depth, eval_best, EXACT));
	return eval_best;
}

void Engine::Search()
{
	counter = 0;
	const PlayerType player = position_ptr->getPlayer();

	MoveList Moves = position_ptr->GenerateMoves();
	if (Moves.empty())
		return;

	float BestEval = (player == WHITE) ? -INFINITY : INFINITY;
	Move BestMove;

	struct Results
	{
		std::future<float> eval;
		Move move;
		Results() {}
	};

	int size = Moves.getSize();
	Results* results = new Results[size];

	for (int i = 0; i < size; ++i)
	{
		const Move& move = Moves[i];

		results[i].eval = workers.addTask(
			[player, &move, this]()
			{
				Evaluate position(*(this->position_ptr));
				position.make_move(move);
				return player == WHITE
					? alpha_beta_MIN(position, DEPTH - 1, -INFINITY, INFINITY)
					: alpha_beta_MAX(position, DEPTH - 1, -INFINITY, INFINITY);
			}
		);
		results[i].move = Moves[i];
	}

	for (int i = 0; i < size; i++)
	{
		float eval = results[i].eval.get();
		if (player == WHITE && eval > BestEval || player == BLACK && eval < BestEval)
		{
			BestEval = eval;
			BestMove = results[i].move;
		}
	}

	std::cout << BestEval << "\n";

	const Zobrist& key = position_ptr->getKey();
	TPT.store(key, Entry(key, DEPTH, BestEval, EXACT));

	position_ptr->make_move(BestMove);

	std::cout << counter << "\n";

	delete[] results;
}