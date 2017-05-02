#include "Game.h"
#include <algorithm>
void Game::drawPlayersPieces(const vector_pieces player, sf::Texture piece_texture)
{
	sf::Sprite piece;
	piece.setTexture(piece_texture);
	piece.setScale(sf::Vector2f(cell_size_ /	1.2/ piece_texture.getSize().x, cell_size_ /1.2/ piece_texture.getSize().y));
	for (auto white_piece : player)
	{
		piece.setPosition(getRealPosition(white_piece.getPosition()));
		window.draw(piece);
	}
}
sf::Vector2f Game::getRealPosition(const BoardIndex & position) const
{
	return sf::Vector2f((position.column - 'a')*cell_size_,(8 - position.row)*cell_size_);
}
void Game::playersInit()
{
	int iter = 0;
	for (int j = 1; j <= 3; ++j)
	{
		char first = (j % 2 != 0 ? 'a' : 'b');
		for (char i = first; i <= 'h'; i += 2)
		{
			white_player_[iter].setPosition({ i,j });
			white_player_[iter].setColor(CheckersPiece::WHITE);
			iter++;
		}
	}
	iter = 0;
	for (int j = 8; j >= 6; j--)
	{
		char first = (j % 2 != 0 ? 'a' : 'b');
		for (char i = first; i <= 'h'; i += 2)
		{
			black_player_[iter].setPosition({ i,j });
			black_player_[iter].setColor(CheckersPiece::BLACK);
			iter++;
		}
	}
}
void Game::movePiece(CheckersPiece& clicked_piece)
{
	possible_beat_moves = clicked_piece.possibleBeatMoves(white_player_,black_player_);
	if (!possible_beat_moves.empty())
	{
		for (auto x : possible_beat_moves)
			hightlighted_cells_.push_back(x.first);
	}
	else
	{
		vector<BoardIndex> possible_moves = clicked_piece.possibleMoves(white_player_, black_player_);
		for (auto x : possible_moves)
			hightlighted_cells_.push_back(x);
	}
}
BoardIndex Game::clickPositionInBoard(int x, int y)
{
	float row = y / cell_size_, column = x / cell_size_;
	return BoardIndex(int(column) + 'a', 8 - int(row));
}
void Game::Run()
{
	playersInit();
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		vector_pieces& cur_player = (white_turn_ ? white_player_ : black_player_);
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonReleased)
			{
				BoardIndex click_position = clickPositionInBoard(event.mouseButton.x, event.mouseButton.y);
				std::cout << click_position << endl;
				pieces_iterator iter = click_position.checkForPieces(cur_player);
				if (iter != cur_player.end())
				{
					hightlighted_cells_.push_back(click_position);
					movePiece(*iter);
				}

			}
		}

		// clear the window with black color
		window.clear(sf::Color::White);

		// draw everything here...
		// window.draw(...);
		drawBoard();
		drawPieces();
		// end the current frame
		window.display();
	}
}
void Game::drawBoard()
{
	sf::RectangleShape cell;
	sf::Font text_font;
	text_font.loadFromFile("times.ttf");

	sf::Text board_coodinates("", text_font);
	board_coodinates.setFillColor(sf::Color::Black);

	sf::Vector2u window_size = window.getSize();
	cell_size_ = (std::min(window_size.x, window_size.y)) / 8.0;

	cell.setSize(sf::Vector2f(cell_size_, cell_size_));
	for (int i = 0; i < 8; i++) //i - row
		for (int j = 0; j < 8; j++) //j - column
		{	
			cell.setPosition(sf::Vector2f(cell_size_*j, cell_size_*i));	
			sf::Color cell_color;
			std::string coord;
			if ((i + j )% 2 != 0)
				cell_color = sf::Color(209, 139, 71, 255);
			else
				cell_color = sf::Color(255, 228, 170, 255);
			cell.setFillColor(cell_color);
			window.draw(cell);
			if (j == 0)
			{
				//coord = 'a' + i;
				coord += std::to_string(8-i);
				board_coodinates.setString(coord);
				board_coodinates.setPosition(sf::Vector2f(0, cell_size_*(i+0.5)-board_coodinates.getCharacterSize()/2));
				window.draw(board_coodinates);
			}
			if (i == 0)
			{
				coord = 'a' + j;
				board_coodinates.setString(coord);
				board_coodinates.setPosition(sf::Vector2f(cell_size_*(j + 0.5) - board_coodinates.getCharacterSize()/2, 0));
				window.draw(board_coodinates);
			}

			board_coodinates.setString(coord);
		}
	const float kThickness = 5.0;
	cell.setSize(sf::Vector2f(cell_size_ - kThickness*2, cell_size_ - kThickness*2));
	for (auto x : hightlighted_cells_)
	{
		
		sf::Vector2f cell_position = getRealPosition(x);
		cell.setPosition(sf::Vector2f(cell_position.x+kThickness,cell_position.y+kThickness));
		cell.setFillColor(sf::Color(0, 0, 0, 0));
		cell.setOutlineColor(sf::Color::Yellow);
		cell.setOutlineThickness(5.0);
		window.draw(cell);
	}
}

void Game::drawPieces()
{
	sf::Texture white_piece, black_piece;
	white_piece.loadFromFile("white_piece.png");
	black_piece.loadFromFile("black_piece.png");
	white_piece.setSmooth(true);
	black_piece.setSmooth(true);
	drawPlayersPieces(white_player_, white_piece);
	drawPlayersPieces(black_player_, black_piece);
}
