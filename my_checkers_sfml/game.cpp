// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Game.h"
#include <ctime>
void Game::drawPlayersPieces(const list_pieces& player, const sf::Texture& piece_texture, const sf::Texture& piece_king_texture)
{
	sf::Sprite piece;
	piece.setScale(sf::Vector2f(cell_size_ / (1.2*piece_texture.getSize().x), cell_size_ / (1.2* piece_texture.getSize().y)));
	for (auto players_piece : player)
	{
		if (players_piece.isKing())
			piece.setTexture(piece_king_texture);
		else
			piece.setTexture(piece_texture);
		piece.setColor(sf::Color::White);
		sf::Vector2f piece_position = getRealPosition(players_piece.getPosition());
		piece_position.x += cell_size_ / 2 - piece.getGlobalBounds().height / 2;
		piece_position.y += cell_size_ / 2 - piece.getGlobalBounds().width / 2;
		piece.setPosition(piece_position);
		window.draw(piece);
	}
}
sf::Vector2f Game::getRealPosition(const BoardIndex & position) const
{
	return sf::Vector2f(
		(position.column - 'a')*cell_size_+kLeftMargin,
		(8 - position.row)*cell_size_+kTopMargin);
}
void Game::playersInit()
{
	int iter = 0;
	for (int j = 1; j <= 3; ++j)
	{
		char first = (j % 2 != 0 ? 'a' : 'b');
		for (char i = first; i <= 'h'; i += 2)
		{
			white_player_.push_back(CheckersPiece(BoardIndex(i, j)));
			/*white_player_[iter].setPosition({ i,j });
			white_player_[iter].setColor(CheckersPiece::WHITE);*/
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
			black_player_.push_back(CheckersPiece( BoardIndex(i, j), CheckersPiece::BLACK ));
			/*black_player_[iter].setPosition({ i,j });
			black_player_[iter].setColor(CheckersPiece::BLACK);*/
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
	if (is_black_ai_ == !white_turn_)
		hightlighted_cells_.clear();
	// drawPieces();
}
void Game::clearInfoForClickedPiece()
{
	is_piece_clicked_ = false;
	piece_firstly_clicked_ = nullptr;
	last_moves_of_cur_player_.clear();
	hightlighted_cells_.clear();
	possible_beat_moves_.clear();
	possible_moves_.clear();
}
void Game::processMouseClick(const BoardIndex& click_position)
{
	if (is_piece_clicked_)
		moveClickedPiece(click_position);
	else
	{
		pieces_iterator piece_on_position = click_position.checkForPieces(*cur_player_);
		if (piece_on_position != cur_player_->end())
		{
			last_moves_of_cur_player_.push_back(click_position);
			if (must_beat_)
			{
				// check whether piece_on_position can beat
				pieces_iterator piece_which_beat_iter = std::find(
					pieces_that_can_beat_.begin(),
					pieces_that_can_beat_.end(),
					*piece_on_position);
				if (piece_which_beat_iter != pieces_that_can_beat_.end())
				{
					// if clicked on right piece - fill info about it
					is_piece_clicked_ = true;
					possible_beat_moves_ = piece_on_position->possibleBeatMoves(*cur_player_, *another_player_, board_);
					piece_firstly_clicked_ = &(*piece_on_position);
					// clear hightlighted_cells_ (which stores pieces_that_can beat previously
					// and add new info
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
				appendVector(hightlighted_cells_, possible_moves_);
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
void Game::moveClickedPiece(const BoardIndex & click_position)
{
	bool move_done = false;
	pieces_iterator clicked_piece_player_iter = click_position.checkForPieces(*cur_player_);

	if (!can_beat_many_times_ && clicked_piece_player_iter != cur_player_->end())
	{
		// if we clicked on piece, not on posible move
		// should clear info about current clicked piece and choose new
		// we don't won't do anything if our piece is in the middle of move
		// (can_beat_many_times_ == true)
		CheckersPiece *previously_clicked_piece = piece_firstly_clicked_;
		clearInfoForClickedPiece();
		appendVector(hightlighted_cells_, pieces_that_can_beat_); 
		// if we clicked on other piece - make move for it
		if (*clicked_piece_player_iter != *previously_clicked_piece)
			processMouseClick(click_position);
	}

	if (!possible_beat_moves_.empty())
	{
		// check wheter click position is in possible beat moves
		int i = 0;
		while (i < possible_beat_moves_.size() && possible_beat_moves_[i].first != click_position)
			++i;

		if (i < possible_beat_moves_.size())
		{
			last_moves_of_cur_player_.push_back(click_position);			
			board_.getPiece(piece_firstly_clicked_->getPosition()) = static_cast<int>(CheckersType::EMPTY);

			piece_firstly_clicked_->setPosition(click_position);
			
			board_.getPiece(possible_beat_moves_[i].second->getPosition()) = static_cast<int>(CheckersType::EMPTY);
			another_player_->erase(possible_beat_moves_[i].second);

			board_.getPiece(click_position) = piece_firstly_clicked_->getCheckersType();

			piece_firstly_clicked_->transformIntoKingIfPossible();

			possible_beat_moves_ = piece_firstly_clicked_->possibleBeatMoves(*cur_player_, *another_player_,board_);
			if (!possible_beat_moves_.empty())
			{
				can_beat_many_times_ = true;
				hightlighted_cells_.clear();
				for (auto x : possible_beat_moves_)
					hightlighted_cells_.push_back(x.first);
			}
			else
			{
				can_beat_many_times_ = false;
				move_done = true;
			}
		}
	}
	else
	{
		if (std::find(possible_moves_.begin(), possible_moves_.end(), click_position) != possible_moves_.end())
		{
			last_moves_of_cur_player_.push_back(click_position);

			board_.getPiece(piece_firstly_clicked_->getPosition()) = static_cast<int>(CheckersType::EMPTY);
			piece_firstly_clicked_->setPosition(click_position);

			board_.getPiece(click_position) = piece_firstly_clicked_->getCheckersType();
			
			piece_firstly_clicked_->transformIntoKingIfPossible();
			//transformIntoKings();
			move_done = true;
		}
	}
	if (move_done)
	{
		last_moves_to_show.clear();
		appendVector(last_moves_to_show, last_moves_of_cur_player_);
		clearInfoForClickedPiece();
		checkForWin();
		changeTurn();
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
void Game::makeMove(const move & move_to_make)
{
	auto piece_to_move = move_to_make.start_position.checkForPieces(*cur_player_);
	if (move_to_make.iter_piece_to_beat == -1)
		piece_to_move->setPosition(move_to_make.end_position);
	else
	{
		vector<move_with_piece> possible_beat_moves_for_piece = piece_to_move->possibleBeatMoves(*cur_player_, *another_player_, board_);
		auto piece_to_beat = findInVector(possible_beat_moves_for_piece, move_to_make.end_position);
		board_.emptyCell(piece_to_beat->second->getPosition());
		another_player_->erase(piece_to_beat->second);
		piece_to_move->setPosition(move_to_make.end_position);
	}
	piece_to_move->transformIntoKingIfPossible();
	board_.movePiece(move_to_make.start_position, move_to_make.end_position);
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
		
		int depth = 10;
		if (!game_ended && ((!white_turn_ && is_black_ai_) || (white_turn_ && is_white_ai_)))
		{
			if (!white_turn_ && is_black_ai_)
				black_ai_.update(white_player_, black_player_, board_);
			else
				white_ai_.update(white_player_, black_player_, board_);

			std::list<move> best_moves_for_ai;
			if (!white_turn_ && is_black_ai_)
				best_moves_for_ai = black_ai_.findBestMove(depth);
			else
				best_moves_for_ai = white_ai_.findBestMove(depth);

			last_moves_to_show.clear();
			for (auto cur_best_move = best_moves_for_ai.begin(); cur_best_move!=best_moves_for_ai.end();++cur_best_move)
			{
				
				last_moves_to_show.push_back(cur_best_move->start_position);
				makeMove(*cur_best_move);
				redrawPosition();
				sf::Clock clock;
				sf::Time timer = clock.getElapsedTime();
				while (clock.getElapsedTime() - timer < sf::milliseconds(500));
				if (std::next(cur_best_move, 1) == best_moves_for_ai.end())
					last_moves_to_show.push_back(cur_best_move->end_position);
			}
			checkForWin();
			changeTurn();
		}
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) || 
				event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::Resized)
			{
				//window.setSize(sf::Vector2u(event.size.width, event.size.height));
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
		
		if (!game_ended && game_state_ != NOT_ENDED)
		{
			cout << (game_state_ == WHITE_WINS ? "White wins!\n" : "Black wins\n") << endl;
			game_ended = true;
		}
		redrawPosition();
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
	float kThickness = 0.08*cell_size_;
	cell.setSize(sf::Vector2f(cell_size_ - kThickness*2, cell_size_ - kThickness*2));
	for (auto x : last_moves_to_show)
	{

		sf::Vector2f cell_position = getRealPosition(x);
		cell.setPosition(sf::Vector2f(cell_position.x + kThickness, cell_position.y + kThickness));
		cell.setFillColor(sf::Color(0, 0, 0, 0));
		cell.setOutlineColor(sf::Color(118, 255, 3, 255));
		cell.setOutlineThickness(kThickness);
		window.draw(cell);
	}

	for (auto x : hightlighted_cells_)
	{
		
		sf::Vector2f cell_position = getRealPosition(x);
		cell.setPosition(sf::Vector2f(cell_position.x+kThickness,cell_position.y+kThickness));
		cell.setFillColor(sf::Color(0, 0, 0, 0));
		cell.setOutlineColor(sf::Color(255, 193, 7, 255));
		cell.setOutlineThickness(kThickness);
		window.draw(cell);
	}
	

	// draw square around the corner of board_ to make it prettier
	cell.setSize(sf::Vector2f(board_size_, board_size_));
	cell.setPosition(getRealPosition(BoardIndex( 'a',8 )));
	cell.setFillColor(sf::Color(0, 0, 0, 0));
	cell.setOutlineColor(sf::Color::Black);
	cell.setOutlineThickness(1.0);
	window.draw(cell);

	//cell.setFillColor(sf::Color::Green);

	// draw current turn rectange (white color for white
	cell.setSize(sf::Vector2f(kTurnRectangleWidth, board_size_ / 2));
	cell.setOutlineThickness(1.0);
	if (white_turn_)
	{
		cell.setFillColor(sf::Color::White);
		cell.setPosition(sf::Vector2f(kLeftMargin + board_size_ + kTurnRectangleMargin, kTopMargin + board_size_ / 2));
	}
	else
	{
		cell.setFillColor(sf::Color::Black);
		cell.setPosition(sf::Vector2f(kLeftMargin + board_size_ + kTurnRectangleMargin, kTopMargin));
	}
	window.draw(cell);

	float left_beaten_position =
		kLeftMargin + board_size_ +
		2 * kTurnRectangleMargin + kTurnRectangleWidth;
	const int kNumberPiecesInARow = 3;
	float diametr_beaten_piece = (kRightMargin - 3 * kTurnRectangleMargin - kTurnRectangleWidth) / kNumberPiecesInARow;

	board_coodinates.setString("Beaten pieces:");
	board_coodinates.setPosition(sf::Vector2f(
		left_beaten_position + (kRightMargin - kTurnRectangleMargin*2- kTurnRectangleWidth - board_coodinates.getGlobalBounds().width)/2, 0));
	window.draw(board_coodinates);
	sf::Texture white_piece, black_piece;
	white_piece.loadFromFile("white_man.png");
	black_piece.loadFromFile("black_man.png");
	white_piece.setSmooth(true);
	black_piece.setSmooth(true);
	

	sf::RectangleShape not_beaten_piece(sf::Vector2f(diametr_beaten_piece,diametr_beaten_piece));
	sf::Sprite beaten_piece_black(black_piece), beaten_piece_white(white_piece);
	beaten_piece_black.setScale(
		diametr_beaten_piece / black_piece.getSize().x,
		diametr_beaten_piece / black_piece.getSize().y);
	beaten_piece_white.setScale(
		diametr_beaten_piece / white_piece.getSize().x,
		diametr_beaten_piece / white_piece.getSize().y);

	not_beaten_piece.setOutlineThickness(1.0);
	not_beaten_piece.setOutlineColor(sf::Color(0,0,0,60));
	not_beaten_piece.setFillColor(sf::Color(0, 0, 0, 0));

	int number_beaten_pieces_black = 12 - black_player_.size(),
		number_beaten_pieces_white = 12 - white_player_.size();

	for (int i = 0; i < 12; i++)
	{
		int row = i / kNumberPiecesInARow,
			column = i % kNumberPiecesInARow;

		if (i < number_beaten_pieces_black)
		{
			beaten_piece_black.setPosition(sf::Vector2f(
				left_beaten_position + column*diametr_beaten_piece,
				kTopMargin + row*diametr_beaten_piece));
			window.draw(beaten_piece_black);
		}
		else
		{
			not_beaten_piece.setPosition(sf::Vector2f(
				left_beaten_position + column*diametr_beaten_piece,
				kTopMargin + row*diametr_beaten_piece));
			window.draw(not_beaten_piece);
		}

		if (i < number_beaten_pieces_white)
		{
			beaten_piece_white.setPosition(sf::Vector2f(
				left_beaten_position + column*diametr_beaten_piece,
				kTopMargin + board_size_/2+row*diametr_beaten_piece));
			window.draw(beaten_piece_white);
		}
		else
		{
			not_beaten_piece.setPosition(sf::Vector2f(
				left_beaten_position + column*diametr_beaten_piece,
				kTopMargin + board_size_ / 2 + row*diametr_beaten_piece));
			window.draw(not_beaten_piece);
		}
	}
}

void Game::drawPieces()
{
	sf::Texture white_piece, black_piece,white_piece_king,black_piece_king;
	/*white_piece.loadFromFile("white_piece.png");
	white_piece_king.loadFromFile("white_piece_king.png");
	black_piece.loadFromFile("black_piece.png");
	black_piece_king.loadFromFile("black_piece_king.png");*/
	white_piece.loadFromFile("white_man.png");
	white_piece_king.loadFromFile("white_queen.png");
	black_piece.loadFromFile("black_man.png");
	black_piece_king.loadFromFile("black_queen.png");
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

void Game::redrawPosition()
{
	window.clear(sf::Color(255, 228, 170, 255));
	drawBoard();
	drawPieces();
	drawWinState();
	window.display();
}

bool Game::checkPlayerHasMove(const list_pieces & player)
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

Game::Game() :
	white_player_(),
	black_player_(),
	white_turn_(true),
	board_size_(700),
	cell_size_(87.5),
	hightlighted_cells_(),
	possible_beat_moves_(),
	possible_moves_(),
	is_piece_clicked_(false),
	piece_firstly_clicked_(),
	cur_player_(&white_player_),
	another_player_(&black_player_),
	pieces_that_can_beat_(),
	must_beat_(false),
	game_state_(NOT_ENDED),
	can_beat_many_times_(false)
{
	sf::VideoMode video_mode;

	// get display resolution
	video_mode = video_mode.getDesktopMode();

	//and use it to make game playable on different screen resolution
	unsigned int window_size = 0.8*std::min(video_mode.height, video_mode.width);// window_size = 900 * 0.8;;
	board_size_ = window_size;
	cell_size_ = board_size_ / 8;
	kTopMargin = kLeftMargin = kBottomMargin = 0.042*board_size_;
	kTurnRectangleWidth = 0.03*board_size_;
	kTurnRectangleMargin = 0.015 * board_size_;
	//kRightMargin = 2 * kTurnRectangleMargin + kTurnRectangleWidth;
	kRightMargin = 0.4 * board_size_;
	window.create(sf::VideoMode(window_size + kLeftMargin + kRightMargin, window_size + kTopMargin + kBottomMargin), "Checkers");
	//window.setView(sf::View(sf::FloatRect(0, 0, window_size + kLeftMargin + kRightMargin, window_size + kTopMargin + kBottomMargin)));

	//set window on the middle of a screen
	window.setPosition(sf::Vector2i((video_mode.width - window.getSize().x) / 2, 0));

	is_black_ai_ = false;
	is_white_ai_ = true;
	black_ai_ = Ai(white_player_, black_player_, board_, Ai::BLACK_PLAYER);
	white_ai_ = Ai(white_player_, black_player_, board_, Ai::WHITE_PLAYER);
}
