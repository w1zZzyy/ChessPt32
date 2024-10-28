#include "Position.h"
#include "Zobrist.h"
#include "ThreadPool.h"
#include <iostream>
#include <vector>





Position::Position(const std::string& FEN)
{
	{
		U64 mask = 0b1000000000000000000000000000000000000000000000000000000000000000;
		for (auto& c : FEN)
		{
			if (c >= '1' && c <= '8') { mask >>= (c - '0'); continue; }

			switch (c)
			{
			case 'K': pieces(WHITE) |= mask;		pieces(WHITE, KING) |= mask;		mask >>= 1; continue;
			case 'P': pieces(WHITE) |= mask;		pieces(WHITE, PAWN) |= mask;		mask >>= 1; continue;
			case 'Q': pieces(WHITE) |= mask;		pieces(WHITE, QUEEN) |= mask;		mask >>= 1; continue;
			case 'N': pieces(WHITE) |= mask;		pieces(WHITE, KNIGHT) |= mask;		mask >>= 1; continue;
			case 'B': pieces(WHITE) |= mask;		pieces(WHITE, BISHOP) |= mask;		mask >>= 1; continue;
			case 'R': pieces(WHITE) |= mask;		pieces(WHITE, ROOK) |= mask;		mask >>= 1; continue;
			case 'k': pieces(BLACK) |= mask;		pieces(BLACK, KING) |= mask;		mask >>= 1; continue;
			case 'p': pieces(BLACK) |= mask;		pieces(BLACK, PAWN) |= mask;		mask >>= 1; continue;
			case 'q': pieces(BLACK) |= mask;		pieces(BLACK, QUEEN) |= mask;		mask >>= 1; continue;
			case 'n': pieces(BLACK) |= mask;		pieces(BLACK, KNIGHT) |= mask;		mask >>= 1; continue;
			case 'b': pieces(BLACK) |= mask;		pieces(BLACK, BISHOP) |= mask;		mask >>= 1; continue;
			case 'r': pieces(BLACK) |= mask;		pieces(BLACK, ROOK) |= mask;		mask >>= 1; continue;
			case '/':																	continue;
			default:																	break;
			}

			break;
		}
		pieces(BOTH) = pieces(WHITE) | pieces(BLACK);
	}

	size_t index = FEN.find(" ") + 1;
	player = (FEN[index] == 'w') ? WHITE : BLACK;
	index += 2;

	while (index < FEN.size() && FEN[index] != ' ')
	{
		switch (FEN[index])
		{
		case 'K': castles |= 0b1000; break;
		case 'Q': castles |= 0b0100; break;
		case 'k': castles |= 0b0010; break;
		case 'q': castles |= 0b0001; break;
		default:
			break;
		}
		index++;
	}

	//zobrist = Zobrist::getHash(pieces, position[0] | position[1]);
}
Position::Position(const Position& pos)
{
	pieces = pos.pieces;
	player = pos.player;
	castles = pos.castles;
	en_passant = pos.en_passant;
}
Position& Position::operator=(const Position& pos) noexcept
{
	pieces = pos.pieces;
	player = pos.player;
	castles = pos.castles;
	en_passant = pos.en_passant;
	return *this;
}



void Position::setThreats() const noexcept
{
	threats.refresh();

	PlayerType player2 = static_cast<PlayerType>(player ^ 1);
	UL king_pos = BSF64_safe(pieces(player, KING));

	U64 rook	=	 PRE_CALC_ATTACKS::getAttacks<ROOK>(king_pos, pieces(player2));
	U64 bishop	=    PRE_CALC_ATTACKS::getAttacks<BISHOP>(king_pos, pieces(player2)); 

	{
		U64 knight = PRE_CALC_ATTACKS::getAttacks<KNIGHT>(king_pos) & (pieces(player2, KNIGHT));
		if (knight)
		{
			threats.checks_counter++;
			threats.sqrs_to_deffend |= knight;
		}
		U64 pawn = (player == WHITE)
			? PRE_CALC_ATTACKS::getAttacks<PAWN, WHITE>(king_pos) & pieces(player2, PAWN)
			: PRE_CALC_ATTACKS::getAttacks<PAWN, BLACK>(king_pos) & pieces(player2, PAWN);
		if (pawn)
		{
			threats.checks_counter++;
			threats.sqrs_to_deffend |= pawn;
		}
	}

	U64 pinners =
		bishop	&	 (pieces(player2, BISHOP)	 |	 pieces(player2, QUEEN))
										|
		rook	&	 (pieces(player2, ROOK)		 |	 pieces(player2, QUEEN));

	while (pinners)
	{
		UL pinner_index = BSF64(pinners);

		U64 between = PRE_CALC_ATTACKS::getBetween(king_pos, pinner_index); 
		U64 pinned = between & pieces(player);

		int pinned_counter = BitsCount(pinned);
		if (pinned_counter == 0)
		{
			threats.checks_counter++;
			threats.sqrs_to_deffend |= between;
		}
		else if (pinned_counter == 1)
		{
			UL pinned_index = BSF64_safe(pinned);
			threats.insert(pinned_index, between);
		}
	}


	// смотрим на атаки вражеских фигур, исключая позицию короля,
	// для того чтобы при шахе он не мог пойти по той же прямой, 
	// где стоит вражеская фигура
	U64 no_king_block = pieces(BOTH) ^ (1ULL << king_pos); 
	threats.opp_attacks = (player != WHITE)
		?
		PRE_CALC_ATTACKS::getAttacks<KING>(pieces(WHITE, KING)) |
		PRE_CALC_ATTACKS::getAttacks<PAWN, WHITE>(pieces(WHITE, PAWN)) |
		PRE_CALC_ATTACKS::getAttacks<KNIGHT>(pieces(WHITE, KNIGHT)) |
		PRE_CALC_ATTACKS::getAttacks<QUEEN>(pieces(WHITE, QUEEN), no_king_block) |
		PRE_CALC_ATTACKS::getAttacks<BISHOP>(pieces(WHITE, BISHOP), no_king_block) |
		PRE_CALC_ATTACKS::getAttacks<ROOK>(pieces(WHITE, ROOK), no_king_block)
		:
		PRE_CALC_ATTACKS::getAttacks<KING>(pieces(BLACK, KING)) |
		PRE_CALC_ATTACKS::getAttacks<PAWN, BLACK>(pieces(BLACK, PAWN)) |
		PRE_CALC_ATTACKS::getAttacks<KNIGHT>(pieces(BLACK, KNIGHT)) |
		PRE_CALC_ATTACKS::getAttacks<QUEEN>(pieces(BLACK, QUEEN), no_king_block) |
		PRE_CALC_ATTACKS::getAttacks<BISHOP>(pieces(BLACK, BISHOP), no_king_block) |
		PRE_CALC_ATTACKS::getAttacks<ROOK>(pieces(BLACK, ROOK), no_king_block);

}


void Position::setBasicMove(Move& move) const noexcept
{
	PlayerType player2 = static_cast<PlayerType>(player ^ 1);
	U64 to = 1ULL << move.getTo();

	if (to & pieces(player2))
	{
		move.setType(MOVE_TYPE::CAPTURE);
		for (int i = 1; i < 6; i++)
		{
			if (pieces(player2, static_cast<PieceType>(i)) & to)
			{
				move.setCapture(i);
				break;
			}
		}
		return;
	}
	move.setType(MOVE_TYPE::null);
}
MoveList Position::GenerateMoves() const noexcept
{
	setThreats();

	MoveList Moves;
	
	GenerateKingMoves(pieces(player, KING), &Moves);
	GeneratePieceMoves<QUEEN>(pieces(player, QUEEN), &Moves);
	GeneratePieceMoves<KNIGHT>(pieces(player, KNIGHT), &Moves);
	GeneratePieceMoves<BISHOP>(pieces(player, BISHOP), &Moves);
	GeneratePieceMoves<ROOK>(pieces(player, ROOK), &Moves);
	player == WHITE
		? GeneratePawnMoves<WHITE>(pieces(player, PAWN), &Moves)
		: GeneratePawnMoves<BLACK>(pieces(player, PAWN), &Moves);
	
	return Moves;

}
void Position::GenerateKingMoves(U64 piece, MoveList* Moves) const noexcept
{
	if (!piece)
		return;

	U64 rav = ~pieces(player) & ~threats.opp_attacks;

	if(player == WHITE)
	{
		if (Castle<CASTLE_TYPE::Short, WHITE>())
			Moves->emplace_back(Move(3, 1, MOVE_TYPE::CASTLE_KING, KING));
		if (Castle<CASTLE_TYPE::Long, WHITE>())
			Moves->emplace_back(Move(3, 5, MOVE_TYPE::CASTLE_QUEEN, KING));
	}
	else
	{
		if (Castle<CASTLE_TYPE::Short, BLACK>())
			Moves->emplace_back(Move(59, 57, MOVE_TYPE::CASTLE_KING, KING));
		if (Castle<CASTLE_TYPE::Long, BLACK>())
			Moves->emplace_back(Move(59, 61, MOVE_TYPE::CASTLE_QUEEN, KING));
	}
	
	UL from = BSF64_safe(piece);
	U64 moves = PRE_CALC_ATTACKS::getAttacks<KING>(from, pieces(BOTH)) & rav;
	while (moves)
	{
		UL to = BSF64(moves);
		Move move(from, to, KING);
		setBasicMove(move);
		Moves->push_back(move);
	}
}

Position& Position::make_move(const Move& move) noexcept
{
	short index_from = move.getFrom(), index_to = move.getTo();
	U64 from = 1ULL << index_from, to = 1ULL << index_to;
	U64 combined = from | to;

	PieceType piece_moved = move.getPieceMoved();

	pieces(player) ^= combined;


	//zobrist ^= Zobrist::getSqrHash(index_from, player, piece_moved);
	MOVE_TYPE move_type = move.getType();

	switch (move_type)
	{
	case MOVE_TYPE::null:
	{
		NULLS++;
		pieces(player, piece_moved) ^= combined;
		//zobrist ^= Zobrist::getSqrHash(index_to, player, piece_moved);

		break;
	}

	case MOVE_TYPE::CAPTURE:
	{
		CAPTURES++;

		PlayerType player2 = static_cast<PlayerType>(player ^ 1);
		PieceType piece_captured = move.getPieceCaptured();

		pieces(player, piece_moved) ^= combined;
		pieces(player2, piece_captured) ^= to;
		pieces(player2) ^= to;

		/*zobrist ^= Zobrist::getSqrHash(index_to, player, piece_moved);
		zobrist ^= Zobrist::getSqrHash(index_to, player2, piece_captured);*/

		if (piece_captured == ROOK)
		{
			if (player == BLACK)
			{
				if (Edges::H1 & to)
					castles &= CASTLE::KING_DISABLE;
				else if (Edges::A1 & to)
					castles &= CASTLE::QUEEN_DISABLE;
			}
			else
			{
				if (Edges::H8 & to)
					castles &= CASTLE::king_DISABLE;
				else if (Edges::A8 & to)
					castles &= CASTLE::queen_DISABLE;
			}
		}

		break;
	}

	case MOVE_TYPE::DOUBLE_PUSH:
	{
		NULLS++;

		pieces(player, piece_moved) ^= combined;
		//zobrist ^= Zobrist::getSqrHash(index_to, player, piece_moved);

		en_passant = (player == WHITE) ? index_to - 8 : index_to + 8;
		player = static_cast<PlayerType>(player ^ 1);
		pieces(BOTH) = pieces(WHITE) | pieces(BLACK);

		return *this;
	}

	case MOVE_TYPE::EN_PASSANT:
	{
		CAPTURES++;
		PASSANTS++;

		PlayerType player2 = static_cast<PlayerType>(player ^ 1);
		/*zobrist ^= Zobrist::getSqrHash(index_to, player, piece_moved);
		zobrist ^= (player2 == 0) ? Zobrist::getSqrHash(index_to + 8, player2, 1) : Zobrist::getSqrHash(index_to - 8, player2, 1);*/

		U64 kill = (player == WHITE) ? 1ULL << (en_passant - 8) : 1ULL << (en_passant + 8);

		pieces(player, piece_moved) ^= combined;
		pieces(player2, PAWN) ^= kill;
		pieces(player2) ^= kill;

		break;
	}

	case MOVE_TYPE::PROMOTION:
	{
		PROMOTIONS++;

		PieceType piece_promoted = move.getPiecePromotion(), piece_captured = move.getPieceCaptured();

		pieces(player, piece_moved) ^= from;
		pieces(player, piece_promoted) |= to;

		//zobrist ^= Zobrist::getSqrHash(index_to, player, piece_promoted);

		if (piece_captured)
		{
			CAPTURES++;

			PlayerType player2 = static_cast<PlayerType>(player ^ 1);
			pieces(player2, piece_captured) ^= to;
			pieces(player2) ^= to;
			//zobrist ^= Zobrist::getSqrHash(index_to, player2, piece_captured);
		}

		break;
	}

	case MOVE_TYPE::CASTLE_KING:
	{
		CASTLES++;

		U64 rcomb = combined >> 1;

		pieces(player, KING) ^= combined;
		pieces(player, ROOK) ^= rcomb;
		pieces(player) ^= rcomb;

		/*zobrist ^= Zobrist::getSqrHash(index_to, player, piece_moved);
		zobrist ^= Zobrist::getSqrHash(index_to + 1, player, 5);
		zobrist ^= Zobrist::getSqrHash(index_to - 1, player, 5);*/

		castles &= (player == WHITE)
			? CASTLE::KING_DISABLE & CASTLE::QUEEN_DISABLE
			: CASTLE::king_DISABLE & CASTLE::queen_DISABLE;

		break;
	}

	case MOVE_TYPE::CASTLE_QUEEN:
	{
		CASTLES++;

		U64 rcomb = (from << 1) | (to << 2);

		pieces(player, KING) ^= combined;
		pieces(player, ROOK) ^= rcomb;
		pieces(player) ^= rcomb;

		/*zobrist ^= Zobrist::getSqrHash(index_to, player, piece_moved);
		zobrist ^= Zobrist::getSqrHash(index_to + 2, player, 5);
		zobrist ^= Zobrist::getSqrHash(index_to - 1, player, 5);*/

		castles &= (player == WHITE)
			? CASTLE::KING_DISABLE & CASTLE::QUEEN_DISABLE
			: CASTLE::king_DISABLE & CASTLE::queen_DISABLE;

		break;
	}

	default:
		break;
	}

	if(move_type == null || move_type == CAPTURE)
	{
		if (piece_moved == KING)
		{
			castles &= (player == WHITE)
				? CASTLE::KING_DISABLE & CASTLE::QUEEN_DISABLE
				: CASTLE::king_DISABLE & CASTLE::queen_DISABLE;
		}
		else if (piece_moved == ROOK)
		{
			if (player == WHITE)
			{
				if (Edges::H1 & from)
					castles &= CASTLE::KING_DISABLE;
				else if (Edges::A1 & from)
					castles &= CASTLE::QUEEN_DISABLE;
			}
			else
			{
				if (Edges::H8 & from)
					castles &= CASTLE::king_DISABLE;
				else if (Edges::A8 & from)
					castles &= CASTLE::queen_DISABLE;
			}
		}
	}

	en_passant = 64;
	player = static_cast<PlayerType>(player ^ 1);
	pieces(BOTH) = pieces(WHITE) | pieces(BLACK);


	return *this;
}


int Position::CountPoses(int depth) const noexcept
{
	int counter = 0;
	ThreadPool<int> threads;
	std::vector<std::future<int>> results;

	MoveList Moves = GenerateMoves();
	for (int i = 0; i < Moves.getSize(); i++)
	{
		Position temp(*this);
		results.push_back(threads.addTask(std::bind(&CountPoses1, depth - 1, temp.make_move(Moves[i]))));
	}
	
	threads.join();
	for (auto& res : results)
		counter += res.get();
	
	return counter;
}
char Position::getPieceFEN(UL index) const noexcept
{
	U64 piece = 1ULL << index;
	if (piece & pieces(WHITE))
	{
		if (piece & pieces(WHITE, KING)) return 'K';
		if (piece & pieces(WHITE, PAWN)) return 'P';
		if (piece & pieces(WHITE, QUEEN)) return 'Q';
		if (piece & pieces(WHITE, KNIGHT)) return 'N';
		if (piece & pieces(WHITE, BISHOP)) return 'B';
		if (piece & pieces(WHITE, ROOK)) return 'R';
	}
	else
	{
		if (piece & pieces(BLACK, KING)) return 'k';
		if (piece & pieces(BLACK, PAWN)) return 'p';
		if (piece & pieces(BLACK, QUEEN)) return 'q';
		if (piece & pieces(BLACK, KNIGHT)) return 'n';
		if (piece & pieces(BLACK, BISHOP)) return 'b';
		if (piece & pieces(BLACK, ROOK)) return 'r';
	}
	return ' ';
}

void Position::print() const noexcept
{
	for (int i = 63; i >= 0; i--)
	{
		std::cout << getPieceFEN(i) << " ";
		if (i % 8 == 0)
			std::cout << "\n";
	}
}

MoveList Position::getPieceMoves(UL index) const noexcept
{
	setThreats();

	U64 piece = 1ULL << index;

	MoveList moves;

	if (piece & pieces(player, KING))
		GenerateKingMoves(piece, &moves);
	else if (piece & pieces(player, PAWN))
		player == WHITE ? GeneratePawnMoves<WHITE>(piece, &moves) : GeneratePawnMoves<BLACK>(piece, &moves);
	else if (piece & pieces(player, QUEEN))
		GeneratePieceMoves<QUEEN>(piece, &moves);
	else if (piece & pieces(player, KNIGHT))
		GeneratePieceMoves<KNIGHT>(piece, &moves);
	else if (piece & pieces(player, BISHOP))
		GeneratePieceMoves<BISHOP>(piece, &moves);
	else if (piece & pieces(player, ROOK))
		GeneratePieceMoves<ROOK>(piece, &moves);

	return moves;
}
