#include "Game.h"
#include <algorithm>
#include <sstream>
void Game::drawPlayersPieces(const vector_pieces player, const sf::Texture& piece_texture,const sf::Texture& piece_king_texture)
{
	sf::Sprite piece;
	piece.setScale(sf::Vector2f(cell_size_ / (1.2*piece_texture.getSize().x), cell_size_ / (1.2* piece_texture.getSize().y)));
	for (auto players_piece : player)
	{
		if (players_piece.isKing())
			piece.setTexture(piece_king_texture);
		else
			piece.setTexture(piece_texture);
		sf::Vector2f piece_position = getRealPosition(players_piece.getPosition());
		piece_position.x += cell_size_ / 2 - piece.getGlobalBounds().height / 2;
		piece_position.y += cell_size_ / 2 - piece.getGlobalBounds().width / 2;
		piece.setPosition(piece_position);
		window.draw(piece);
	}
}
sf::Vector2f Game::getRealPosition(const BoardIndex & position) const
{
	return sf::Vector2f((position.column - 'a')*cell_size_+kLeftMargin,(8 - position.row)*cell_size_+kTopMargin);
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
			board_.getPiece(BoardIndex(i, j)) = static_cast<int>(CheckersType::WHITE_PIECE);
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
			board_.getPiece(BoardIndex(i, j)) = static_cast<int>(CheckersType::BLACK_PIECE);
			iter++;
		}
	}
}
void Game::changeTurn()
{
	white_turn_ = !white_turn_;
	std::swap(cur_player_, another_player_);
	checkPiecesForBeating();
	drawPieces();
}
void Game::clearInfoForClickedPiece()
{
	hightlighted_cells_.clear();
	possible_beat_moves_.clear();
	possible_moves_.clear();
}
void Game::processMouseClick(const BoardIndex click_position)
{
	if (is_piece_clicked_)
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
					is_piece_clicked_ = true;
					possible_beat_moves_ = piece_on_position->possibleBeatMoves(*cur_player_, *another_player_,board_);
					piece_firstly_clicked_ = &(*piece_on_position);
					hightlighted_cells_.clear();
					hightlighted_cells_.push_back(piece_on_position->getPosition());
					for (auto x : possible_beat_moves_)
						hightlighted_cells_.push_back(x.first);
				}
			}
			else
			{
				is_piece_clicked_ = true;
				piece_firstly_clicked_ = &(*piece_on_position);
				possible_moves_ = piece_on_position->possibleMoves(*cur_player_, *another_player_, board_);
				hightlighted_cells_.push_back(piece_on_position->getPosition());
				hightlighted_cells_.insert(hightlighted_cells_.end(), possible_moves_.begin(), possible_moves_.end());
			}
		}
	}
}
void Game::buildPossibleMoves(CheckersPiece& clicked_piece)
{
	piece_firstly_clicked_ = &clicked_piece;
	possible_beat_moves_ = clicked_piece.possibleBeatMoves(*cur_player_, *another_player_, board_);
	if (!possible_beat_moves_.empty())
	{
		for (auto x : possible_beat_moves_)
			hightlighted_cells_.push_back(x.first);
	}
	else
	{
		possible_moves_ = clicked_piece.possibleMoves(*cur_player_, *another_player_, board_);
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
		appendVector(hightlighted_cells_, pieces_that_can_beat_);
		is_piece_clicked_ = false;
	}
	if (!possible_beat_moves_.empty())
	{
		int i = 0;
		while (i < possible_beat_moves_.size() && possible_beat_moves_[i].first != click_position)
			++i;
		if (i < possible_beat_moves_.size())
		{
			board_.getPiece(piece_firstly_clicked_->getPosition()) = static_cast<int>(CheckersType::EMPTY);
			piece_firstly_clicked_->setPosition(click_position);
			board_.getPiece(possible_beat_moves_[i].second->getPosition()) = static_cast<int>(CheckersType::EMPTY);
			another_player_->erase(possible_beat_moves_[i].second);
			board_.getPiece(click_position) = piece_firstly_clicked_->getCheckersType();
			transformIntoKings();
			possible_beat_moves_ = piece_firstly_clicked_->possibleBeatMoves(*cur_player_, *another_player_,board_);
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
			board_.getPiece(piece_firstly_clicked_->getPosition()) = static_cast<int>(CheckersType::EMPTY);
			piece_firstly_clicked_->setPosition(click_position);
			board_.getPiece(click_position) = piece_firstly_clicked_->getCheckersType();
			transformIntoKings();
			move_done = true;
		}
	}
	if (move_done)
	{
		clearInfoForClickedPiece();
		checkForWin();
		changeTurn();
		is_piece_clicked_ = false;
	}
}
void Game::checkForWin()
{
	if (white_player_.empty())
		game_state_ = BLACK_WINS;
	else if (black_player_.empty())
		game_state_ = WHITE_WINS;
	else if (!checkPlayerHasMove(white_player_))
		game_state_ = BLACK_WINS;
	else if (!checkPlayerHasMove(black_player_))
		game_state_ = WHITE_WINS;
	else
		game_state_ = NOT_ENDED;
}
void Game::checkPiecesForBeating()
{
	pieces_that_can_beat_.clear();
	for (auto x : *cur_player_)
	{
		if (x.getPosition() == BoardIndex('b', 4))
			cout << x.getPosition() << endl;
		if (!(x.possibleBeatMoves(*cur_player_, *another_player_, board_).empty()))
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
void Game::transformIntoKings()
{
	for (auto &x : *cur_player_)
		if (x.transformIntoKingIfPossible())
			board_.getPiece(x.getPosition()) = x.getCheckersType();
}
BoardIndex Game::clickPositionInBoard(int x, int y)
{
	sf::Vector2f real_coordinates = window.mapPixelToCoords(sf::Vector2i(x, y));
	float row = (real_coordinates.y-kTopMargin) / cell_size_, column = (real_coordinates.x-kLeftMargin) / cell_size_;
	return BoardIndex(int(column) + 'a', 8 - int(row));
}
void Game::Run()
{
	playersInit();
	checkPiecesForBeating();
	bool game_ended = false;
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::Resized)
			{
				window.setSize(sf::Vector2u(event.size.width, event.size.width));
				//window.setView(sf::View(sf::FloatRect(0,0,event.size.width,event.size.width)));
			}
			if (event.type == sf::Event::KeyReleased)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::S:
					saveToFile("position.check");
					break;
				case sf::Keyboard::R:
					game_ended = false;
					loadFromFile("position.check");
					break;
				case sf::Keyboard::F:
					for (int i = 0; i < 8; i++)
					{
						for (int j = 0; j < 8; j++)
							cout << board_.checkers_map[i][j] << ' ';
						cout << endl;
					}
					break;
				}


			}
			if (!game_ended)
			{
				if (event.type == sf::Event::MouseButtonReleased)
				{
					BoardIndex click_position = clickPositionInBoard(event.mouseButton.x, event.mouseButton.y);
					std::cout << click_position << endl;
					processMouseClick(click_position);
					
				}
			}
		}
		window.clear(sf::Color(255, 228, 170, 255));
		if (!game_ended && game_state_ != NOT_ENDED)
		{
			cout << (game_state_ == WHITE_WINS ? "White wins!\n" : "Black wins\n") << endl;
			game_ended = true;
		}
		drawBoard();
		drawPieces();
		drawWinState();
		// end the current frame
		window.display();
	}
}
void Game::setWhiteTurn()
{
	white_turn_ = true;
	cur_player_ = &white_player_;
	another_player_ = &black_player_;
}
void Game::setBlackTurn()
{
	white_turn_ = false;
	cur_player_ = &black_player_;
	another_player_ = &white_player_;
}
void Game::loadFromFile(std::string file_name)
{
	std::ifstream in(file_name);
	clearAllStates();
	std::stringstream str_stream;
	std::string str;
	getline(in, str);
	str_stream << str;
	CheckersPiece piece_to_input;
	while (str_stream)
	{
		str_stream >> piece_to_input;
		white_player_.push_back(piece_to_input);
	}
	str_stream.clear();
	getline(in, str);
	str_stream << str;
	while (str_stream)
	{
		str_stream >> piece_to_input;
		black_player_.push_back(piece_to_input);
	}
	in >> white_turn_;
	if (white_turn_)
		setWhiteTurn();
	else
		setBlackTurn();
	in.close();
}
void Game::saveToFile(std::string file_name)
{
	std::ofstream out(file_name);
	for (auto x : white_player_)
		out << x << ' ';
	out << endl;
	for (auto x : black_player_)
		out << x << ' ';
	out << endl;
	out << white_turn_;
	out.close();
}
void Game::drawBoard()
{
	sf::RectangleShape cell;
	sf::Font text_font;
	text_font.loadFromFile("times.ttf");
	sf::Text board_coodinates("", text_font,25);
	board_coodinates.setFillColor(sf::Color::Black);
	/*sf::Vector2u window_size = window.getSize();
	unsigned int board_size = window_size.x - 2 * kTextMargin;
	cell_size_ = board_size / 8.0;*/
	cell.setSize(sf::Vector2f(cell_size_, cell_size_));
	for (int i = 0; i < 8; i++) //i - row
		for (int j = 0; j < 8; j++) //j - column
		{	
			cell.setPosition(getRealPosition(BoardIndex('a'+j,8-i)));	
			sf::Color cell_color;
			std::string coord;
			if ((i + j) % 2 != 0)
				cell_color = sf::Color(209, 139, 71, 255);
				//cell_color = sf::Color::Blue;
			else
				cell_color = sf::Color(255, 228, 170, 255);
				//cell_color = sf::Color::Yellow;
			cell.setFillColor(cell_color);
			window.draw(cell);
			if (j == 0)
			{
				//coord = 'a' + i;
				coord += std::to_string(8-i);
				board_coodinates.setString(coord);
				board_coodinates.setPosition((kLeftMargin - board_coodinates.getGlobalBounds().width)/2,
					cell_size_*(i+0.5)-board_coodinates.getCharacterSize()/2+kTopMargin);
				window.draw(board_coodinates);
			}
			if (i == 0)
			{
				coord = 'a' + j;
				board_coodinates.setString(coord);
				board_coodinates.setPosition(sf::Vector2f(cell_size_*(j + 0.5) - board_coodinates.getCharacterSize()/2+kLeftMargin, 
					0));
				window.draw(board_coodinates);
			}
		}
	const float kThickness = 5.0;
	cell.setSize(sf::Vector2f(cell_size_ - kThickness*2, cell_size_ - kThickness*2));
	for (auto x : hightlighted_cells_)
	{
		
		sf::Vector2f cell_position = getRealPosition(x);
		cell.setPosition(sf::Vector2f(cell_position.x+kThickness,cell_position.y+kThickness));
		cell.setFillColor(sf::Color(0, 0, 0, 0));
		cell.setOutlineColor(sf::Color::Yellow);
		cell.setOutlineThickness(kThickness);
		window.draw(cell);
	}
	cell.setSize(sf::Vector2f(board_size_, board_size_));
	cell.setPosition(getRealPosition({ 'a',8 }));
	cell.setFillColor(sf::Color(0, 0, 0, 0));
	cell.setOutlineColor(sf::Color::Black);
	cell.setOutlineThickness(1.0);
	window.draw(cell);
	//cell.setFillColor(sf::Color::Green);
	cell.setSize(sf::Vector2f(kRightMargin / 2, board_size_ / 2));
	cell.setOutlineThickness(1.0);
	if (white_turn_)
	{
		cell.setFillColor(sf::Color::White);
		cell.setPosition(sf::Vector2f(kLeftMargin + board_size_ + (kLeftMargin - cell.getSize().x) / 2, kTopMargin + board_size_ / 2));
	}
	else
	{
		cell.setFillColor(sf::Color::Black);
		cell.setPosition(sf::Vector2f(kLeftMargin + board_size_ + (kLeftMargin - cell.getSize().x) / 2, kTopMargin));
	}
	window.draw(cell);
}

void Game::drawPieces()
{
	sf::Texture white_piece, black_piece,white_piece_king,black_piece_king;
	white_piece.loadFromFile("white_piece.png");
	white_piece_king.loadFromFile("white_piece_king.png");
	black_piece.loadFromFile("black_piece.png");
	black_piece_king.loadFromFile("black_piece_king.png");
	white_piece.setSmooth(true);
	white_piece_king.setSmooth(true);
	black_piece.setSmooth(true);
	black_piece_king.setSmooth(true);
	drawPlayersPieces(white_player_, white_piece,white_piece_king);
	drawPlayersPieces(black_player_, black_piece,black_piece_king);
}

void Game::drawWinState()
{
	sf::Font text_font;
	text_font.loadFromFile("times.ttf");
	//float board_size_ = cell_size_ * 8;
	sf::Text win_text("", text_font, 100);
	win_text.setPosition(sf::Vector2f(
		kLeftMargin + (board_size_ / 2 - win_text.getGlobalBounds().width),
		kTopMargin + (board_size_ / 2 - win_text.getGlobalBounds().height)));
//	cout << win_text.getGlobalBounds().width << ' ' << win_text.getGlobalBounds().height << endl;
	switch (game_state_)
	{
	case WHITE_WINS:
		win_text.setString("White wins!");
		win_text.setOutlineThickness(1.0);
		win_text.setOutlineColor(sf::Color::Black);
		win_text.setPosition(sf::Vector2f(
			kLeftMargin + (board_size_ - win_text.getGlobalBounds().width)/2,
			kTopMargin + (board_size_ - win_text.getGlobalBounds().height)/2));
		window.draw(win_text);
		break;
	case BLACK_WINS:
		win_text.setFillColor(sf::Color::Black);
		win_text.setString("Black wins!");
		win_text.setPosition(sf::Vector2f(
			kLeftMargin + (board_size_ - win_text.getGlobalBounds().width)/2,
			kTopMargin + (board_size_ - win_text.getGlobalBounds().height)/2));
		window.draw(win_text);
		break;
	}
	
}

bool Game::checkPlayerHasMove(const vector_pieces & player)
{
	const_pieces_iterator iter = player.begin();
	bool cur_has_move = true;
	do
	{
		cur_has_move = true;
		if (iter->possibleBeatMoves(*cur_player_, *another_player_, board_).empty())
			if (iter->possibleMoves(*cur_player_, *another_player_, board_).empty())
				cur_has_move = false;
		++iter;
	} while (iter != player.end() && !cur_has_move);
	return cur_has_move;
}

void Game::clearAllStates()
{
	white_player_.clear();
	black_player_.clear();
	is_piece_clicked_ = false;
	setWhiteTurn();
	game_state_ = NOT_ENDED;
	piece_firstly_clicked_ = nullptr;
	must_beat_ = false;
	clearInfoForClickedPiece();
}
