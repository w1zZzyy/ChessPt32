#include "Graphics.h"
#include <mutex>

extern std::mutex updateMTX;

Graphics::Graphics(Position& board, PlayerType player, int winX, int winY) : player(player)
{
	window.create(sf::VideoMode(winX, winY), "ChessAI");
	position = &board;


	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			textures[i][j].loadFromFile(imgs[i][j]);
			textures[i][j].setSmooth(true);
		}
	}

	CellSize.x = static_cast<float>(winX) / 8;
	CellSize.y = static_cast<float>(winY) / 8;

	{
		short index = 63;
		for (float y = 0; y < 8 * CellSize.y; y += CellSize.y)
		{
			for (float x = 0; x < 8 * CellSize.x; x += CellSize.x)
			{
				cords.insert({ index, {x, y} });
				index--;
			}
		}
	}
}

void Graphics::run()
{
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{

			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::MouseButtonPressed:
				if(position->getPlayer() == player || player == BOTH)
					mouse_pressed();
				break;

			default:
				break;
			}
		}

		{
			std::lock_guard<std::mutex>m(updateMTX);
			update_pieces();
		}

		window.clear();
		draw();
		window.display();
	}
}


void Graphics::draw()
{
	{
		sf::RectangleShape cell;
		cell.setSize(CellSize);
		for (auto& sqr : cords)
		{
			if ((sqr.index / 8) % 2 == 0)
			{
				(sqr.index % 2 == 0) ? cell.setFillColor(WhiteCell) : cell.setFillColor(BlackCell);
			}
			else
			{
				(sqr.index % 2 != 0) ? cell.setFillColor(WhiteCell) : cell.setFillColor(BlackCell);
			}
			cell.setPosition(sqr.cord);
			window.draw(cell);
		}
	}

	{
		if(PressedPiece)
		{
			sf::RectangleShape cell;

			cell.setSize(CellSize);
			cell.setFillColor(PressedCell);
			cell.setPosition(PressedPiece->cords.cord);
			
			window.draw(cell);

			cell.setFillColor(MovesCell);
			for (auto& move : PressedPiece->Moves)
			{
				auto it = cords.find(Coordinates(move.getTo()));
				cell.setPosition(it->cord);
				window.draw(cell);
			}
		}
	}


	{
		sf::RectangleShape piece;
		piece.setSize(CellSize);

		for (auto& white : WhitePieces)
		{
			piece.setTexture(white->texture);
			piece.setPosition(white->cords.cord);
			window.draw(piece);
		}
		for (auto& black : BlackPieces)
		{
			piece.setTexture(black->texture);
			piece.setPosition(black->cords.cord);
			window.draw(piece);
		}
	}

}


void Graphics::update_pieces()
{
	WhitePieces.clear();
	BlackPieces.clear();

	for (auto& sqr : cords)
	{
		char piece = position->getPieceFEN(sqr.index);
		switch (piece)
		{
			case 'K': WhitePieces.push_back(std::make_shared<Figure>(&textures[0][0], sqr)); break;
			case 'Q': WhitePieces.push_back(std::make_shared<Figure>(&textures[0][1], sqr)); break;
			case 'P': WhitePieces.push_back(std::make_shared<Figure>(&textures[0][2], sqr)); break;
			case 'N': WhitePieces.push_back(std::make_shared<Figure>(&textures[0][3], sqr)); break;
			case 'B': WhitePieces.push_back(std::make_shared<Figure>(&textures[0][4], sqr)); break;
			case 'R': WhitePieces.push_back(std::make_shared<Figure>(&textures[0][5], sqr)); break;
			case 'k': BlackPieces.push_back(std::make_shared<Figure>(&textures[1][0], sqr)); break;
			case 'q': BlackPieces.push_back(std::make_shared<Figure>(&textures[1][1], sqr)); break;
			case 'p': BlackPieces.push_back(std::make_shared<Figure>(&textures[1][2], sqr)); break;
			case 'n': BlackPieces.push_back(std::make_shared<Figure>(&textures[1][3], sqr)); break;
			case 'b': BlackPieces.push_back(std::make_shared<Figure>(&textures[1][4], sqr)); break;
			case 'r': BlackPieces.push_back(std::make_shared<Figure>(&textures[1][5], sqr)); break;
			default:																		 break;
		}
	}
}


void Graphics::mouse_pressed()
{
	sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
	sf::RectangleShape cell;
	cell.setSize(CellSize);

	if (PressedPiece)
	{
		for (auto& cord : cords)
		{
			cell.setPosition(cord.cord);
			if (cell.getGlobalBounds().contains(mousePos))
			{
				//MoveList Moves = PressedPiece->Moves;
				for (auto& move : PressedPiece->Moves)
				{
					if (cord.index == move.getTo())
					{
						position->make_move(move);
						PressedPiece = nullptr;
						return;
					}
				}
			}
		}
		PressedPiece = nullptr;
	}

	{
		if(player != -1)
		{
			std::vector<std::shared_ptr<Figure>>& pieces = (player == 0) ? WhitePieces : BlackPieces;
			for (auto& piece : pieces)
			{
				cell.setPosition(piece->cords.cord);
				if (cell.getGlobalBounds().contains(mousePos))
				{
					PressedPiece = piece;
					PressedPiece->Moves = position->getPieceMoves(PressedPiece->cords.index);
					break;
				}
			}
		}
		else
		{
			for (auto& piece : WhitePieces)
			{
				cell.setPosition(piece->cords.cord);
				if (cell.getGlobalBounds().contains(mousePos))
				{
					PressedPiece = piece;
					PressedPiece->Moves = position->getPieceMoves(PressedPiece->cords.index);
					break;
				}
			}
			for (auto& piece : BlackPieces)
			{
				cell.setPosition(piece->cords.cord);
				if (cell.getGlobalBounds().contains(mousePos))
				{
					PressedPiece = piece;
					PressedPiece->Moves = position->getPieceMoves(PressedPiece->cords.index);
					break;
				}
			}
		}
	}
}

