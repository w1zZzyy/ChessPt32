#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <mutex>
#include <unordered_set>
#include <memory>
#include "Move.h"
#include "Position.h"

class Graphics
{
private:

	sf::RenderWindow window;
	Position* position;
	const PlayerType player; 



	const std::string imgs[2][6] =
	{
		{"PiecesImgs/WhiteKing.png", "PiecesImgs/WhiteQueen.png", "PiecesImgs/WhitePawn.png", "PiecesImgs/WhiteKnight.png","PiecesImgs/WhiteBishop.png","PiecesImgs/WhiteRook.png"},
		{"PiecesImgs/BlackKing.png", "PiecesImgs/BlackQueen.png", "PiecesImgs/BlackPawn.png", "PiecesImgs/BlackKnight.png","PiecesImgs/BlackBishop.png","PiecesImgs/BlackRook.png"}
	};
	sf::Texture textures[2][6];



	sf::Vector2f CellSize;
	sf::Color WhiteCell		=	 sf::Color(211, 211, 211);
	sf::Color BlackCell		=	 sf::Color(60, 110, 160);
	sf::Color PressedCell	=	 sf::Color(0, 170, 0, 100);
	sf::Color MovesCell		=	 sf::Color(100, 0, 0, 200);

	

	struct Coordinates
	{
		short index;
		sf::Vector2f cord;

		bool operator == (const Coordinates& c) const noexcept
		{
			return index == c.index;
		}
	};
	struct hash_cords
	{
		size_t operator () (const Coordinates& c) const noexcept
		{
			return std::hash<UL>{}(c.index);
		}
	};
	std::unordered_set<Coordinates, hash_cords> cords;



	struct Figure
	{
		sf::Texture* texture;
		Coordinates cords;
		MoveList Moves;
	};
	std::vector<std::shared_ptr<Figure>> WhitePieces, BlackPieces;
	std::shared_ptr<Figure> PressedPiece = nullptr;




	void draw();
	void update_pieces();
	void mouse_pressed();

public:

	Graphics(Position& board, PlayerType player, int winX = 950, int winY = 1000);
	void run();

};