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
void Game::changeTurn()
{
	white_turn_ = !white_turn_;
	std::swap(cur_player_, another_player_);
	checkPiecesForBeating();
}
void Game::clearInfoForClickedPiece()
{
	hightlighted_cells_.clear();
	possible_beat_moves_.clear();
	possible_moves_.clear();
}
void Game::processMouseClick(const BoardIndex click_position)
{
	if (piece_clicked_)
		movePiece(click_position);
	else
	{
		pieces_iterator piece_on_position = click_position.checkForPieces(*cur_player_);
		if (piece_on_position != cur_player_->end())
		{

			if (must_beat_)
			{
				pieces_iterator piece_which_beat = std::find(
					pieces_that_can_beat_.begin(),
					pieces_that_can_beat_.end(),
					*piece_on_position);
				if (piece_which_beat != pieces_that_can_beat_.end())
				{
					piece_clicked_ = true;
					possible_beat_moves_ = piece_on_position->possibleBeatMoves(*cur_player_, *another_player_);
					piece_firstly_clicked_ = &(*piece_on_position);
					hightlighted_cells_.clear();
					hightlighted_cells_.push_back(piece_on_position->getPosition());
					for (auto x : possible_beat_moves_)
						hightlighted_cells_.push_back(x.first);
				}
			}
			else
			{
				piece_clicked_ = true;
				piece_firstly_clicked_ = &(*piece_on_position);
				possible_moves_ = piece_on_position->possibleMoves(*cur_player_, *another_player_);
				hightlighted_cells_.push_back(piece_on_position->getPosition());
				hightlighted_cells_.insert(hightlighted_cells_.end(), possible_moves_.begin(), possible_moves_.end());
			}
		}
	}
}
void Game::buildPossibleMoves(CheckersPiece& clicked_piece)
{
	piece_firstly_clicked_ = &clicked_piece;
	possible_beat_moves_ = clicked_piece.possibleBeatMoves(*cur_player_, *another_player_);
	if (!possible_beat_moves_.empty())
	{
		for (auto x : possible_beat_moves_)
			hightlighted_cells_.push_back(x.first);
	}
	else
	{
		possible_moves_ = clicked_piece.possibleMoves(*cur_player_, *another_player_);
		for (auto x : possible_moves_)
			hightlighted_cells_.push_back(x);
	}
}
void Game::movePiece(const BoardIndex & click_position)
{
	bool move_done = false;
	pieces_iterator clicked_piece_player_iter = click_position.checkForPieces(*cur_player_);
	if (clicked_piece_player_iter != cur_player_->end() && *clicked_piece_player_iter == *piece_firstly_clicked_)
	{
		clearInfoForClickedPiece();
		piece_clicked_ = false;
	}
	if (!possible_beat_moves_.empty())
	{
		int i = 0;
		while (i < possible_beat_moves_.size() && possible_beat_moves_[i].first != click_position)
			++i;
		if (i < possible_beat_moves_.size())
		{
			piece_firstly_clicked_->setPosition(click_position);
			another_player_->erase(possible_beat_moves_[i].second);
			possible_beat_moves_ = piece_firstly_clicked_->possibleBeatMoves(*cur_player_, *another_player_);
			if (!possible_beat_moves_.empty())
			{
				hightlighted_cells_.clear();
				for (auto x : possible_beat_moves_)
					hightlighted_cells_.push_back(x.first);
			}
			else
				move_done = true;
		}
	}
	else
	{
		if (std::find(possible_moves_.begin(), possible_moves_.end(), click_position) != possible_moves_.end())
		{
			piece_firstly_clicked_->setPosition(click_position);
			move_done = true;
		}
	}
	if (move_done)
	{
		clearInfoForClickedPiece();
		changeTurn();
		piece_clicked_ = false;
	}
}
void Game::checkPiecesForBeating()
{
	pieces_that_can_beat_.clear();
	for (auto x : *cur_player_)
	{
		if (!(x.possibleBeatMoves(*cur_player_, *another_player_).empty()))
		{
			pieces_that_can_beat_.push_back(x);
			hightlighted_cells_.push_back(x.getPosition());
		}
	}
	if (!pieces_that_can_beat_.empty())
		must_beat_ = true;
	else
		must_beat_ = false;
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
				processMouseClick(click_position);
				/*if (!piece_clicked_)
				{
					pieces_iterator iter = click_position.checkForPieces(*cur_player_);
					if (iter != cur_player_->end())
					{
						piece_clicked_ = true;
						hightlighted_cells_.push_back(click_position);
						buildPossibleMoves(*iter);
					}
				}
				else
					movePiece(click_position);*/

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
