#include "Position.h"
#include "Engine.h"
#include "Graphics.h"
#include "Zobrist.h"

#include <iostream>
#include <chrono>

std::mutex updateMTX;

class TimeStart
{
private:

	std::chrono::time_point<std::chrono::steady_clock> start;

public:

	TimeStart() { start = std::chrono::high_resolution_clock::now(); }
	~TimeStart() 
	{ 
		auto end = std::chrono::high_resolution_clock::now(); 
		std::chrono::duration<double> d = end - start;
		std::cout << "Time = " << d.count() << "\n";
	}

};






static void MoveMaker(Position& pos, int depth)
{
	if (depth == 0)
		return;
	
	MoveList Moves = pos.GenerateMoves();
	for (int i = 0; i < Moves.getSize(); i++)
	{
		Position temp(pos);
		pos.make_move(Moves[i]);
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		MoveMaker(pos, depth - 1);
		pos = temp;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}



std::atomic<int> CAPTURES = 0, PASSANTS = 0, PROMOTIONS = 0, CASTLES = 0, NULLS = 0;
static void Stats(Position& position, int depth)
{
	int counter;
	{
		TimeStart t;
		counter = position.CountPoses(depth);
	}
	std::cout << counter;
	/*std::cout
		<< "\n::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n"
		<< "\t\t\tSTATS\n\n"
		<< "Positions     =     " << counter << "\n"
		<< "Captures      =     " << CAPTURES << "\n"
		<< "Passants      =     " << PASSANTS << "\n"
		<< "Castles       =     " << CASTLES << "\n"
		<< "Promotions    =     " << PROMOTIONS << "\n"
		<< "Common        =     " << NULLS
		<< "\n\n::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n";*/
}



static void Play(Position& position, PlayerType player)
{
	Engine engine(position, 8);
	while (true)
	{
		if (position.getPlayer() == player)
		{
			std::lock_guard<std::mutex> m(updateMTX);

			{
				TimeStart t;
				engine.Search();
			}

			if (position.getPlayer() == player)
				break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}



int main()
{
	ZOBRIST::init();
	PRE_CALC_ATTACKS::init();

	Position board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq");

	Graphics graphics(board, WHITE);
	std::thread t(&Play, std::ref(board), BLACK);
	graphics.run();
	t.join();
}