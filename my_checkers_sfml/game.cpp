// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Game.h"
#include <ctime>
// useful operation on vectors and lists
template <typename T>
inline void appendVector(vector<T>& dest, const vector<T>& source)
{
	dest.insert(dest.end(), source.begin(), source.end());
}
template <typename T>
inline void appendVector(vector<T>& dest, const list<T>& source)
{
	dest.insert(dest.end(), source.begin(), source.end());
}
inline void appendVector(vector<BoardIndex>& dest, const CheckersPieceWithState* source)
{
	for (int i = 0; i<12; i++)
		if (source[i].not_beaten)
			dest.push_back(source[i].getPosition());
}
inline void appendVector(vector<BoardIndex>& dest, const list<CheckersPiece>& source)
{
	for (auto x : source)
	{
		dest.push_back(x.getPosition());
	}
}
inline void appendVector(vector<BoardIndex>& dest, const vector<move_with_piece>& source)
{
	for (auto x : source)
	{
		dest.push_back(x.first);
	}
}

template <typename T>
inline typename vector<T>::iterator findInVector(vector<T>& source, T key)
{
	return std::find(source.begin(), source.end(), key);
}

inline typename vector<move_with_piece>::iterator findInVector(vector<move_with_piece>& source, BoardIndex key)
{
	vector<move_with_piece>::iterator iter = source.begin();
	while (iter != source.end() && iter->first != key)
		++iter;
	return iter;
}

template <typename T>
int shiftToRight(T* arr, int arr_size, int position, int shift_size = 1)
{
	for (int i = position + shift_size; i < arr_size; i++)
		arr[i - shift_size] = arr[i];
	return arr_size - shift_size;
}
void Game::playersInit()
{
	int iter = 0;
	for (int j = 1; j <= 3; ++j)
	{
		char first = (j % 2 != 0 ? 'a' : 'b');
		for (char i = first; i <= 'h'; i += 2)
		{
			white_player_[iter] = CheckersPiece(BoardIndex(i, j), CheckersPiece::WHITE);
			/*white_player_[iter].setPosition({ i,j });
			white_player_[iter].setColor(CheckersPiece::WHITE);*/
			board_.getPiece(BoardIndex(i, j)) = CheckersType::WHITE_PIECE;
			iter++;
		}
	}
	iter = 0;
	for (int j = 8; j >= 6; j--)
	{
		char first = (j % 2 != 0 ? 'a' : 'b');
		for (char i = first; i <= 'h'; i += 2)
		{
			black_player_[iter] = CheckersPiece( BoardIndex(i, j), CheckersPiece::BLACK );
			/*black_player_[iter].setPosition({ i,j });
			black_player_[iter].setColor(CheckersPiece::BLACK);*/
			board_.getPiece(BoardIndex(i, j)) = CheckersType::BLACK_PIECE;
			iter++;
		}
	}
}


void Game::changeTurn()
{
	white_turn_ = !white_turn_;
	std::swap(cur_player_, another_player_);
	checkPiecesForBeating();
	if ((is_black_ai_ && !white_turn_) || (is_white_ai_ && white_turn_))
		hightlighted_cells_.clear();
	// drawPieces();
}
void Game::clearInfoForClickedPiece()
{
	is_piece_clicked_ = false;
	piece_firstly_clicked_ = -1;
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
		int piece_on_position = click_position.checkForPieces(cur_player_);
		if (piece_on_position != -1)
		{
			last_moves_of_cur_player_.push_back(click_position);
			if (must_beat_)
			{
				// check whether piece_on_position can beat
				auto piece_which_beat_iter = std::find(
					pieces_that_can_beat_.begin(),
					pieces_that_can_beat_.end(),
					cur_player_[piece_on_position].getPosition());
				if (piece_which_beat_iter != pieces_that_can_beat_.end())
				{
					// if clicked on right piece - fill info about it
					is_piece_clicked_ = true;
					possible_beat_moves_ = cur_player_[piece_on_position].possibleBeatMoves(white_player_, another_player_, board_);
					piece_firstly_clicked_ = piece_on_position;
					// clear hightlighted_cells_ (which stores pieces_that_can_beat_ previously)
					// and add new info
					hightlighted_cells_.clear();
					if (possible_beat_moves_.size() == 1)
						moveClickedPiece(possible_beat_moves_.front().first);
					else
					{
						hightlighted_cells_.push_back(cur_player_[piece_on_position].getPosition());
						appendVector(hightlighted_cells_, possible_beat_moves_);
					}
				}
			}
			else
			{
				is_piece_clicked_ = true;
				piece_firstly_clicked_ = piece_on_position;
				possible_moves_ = cur_player_[piece_on_position].possibleMoves(cur_player_, another_player_, board_);
				if (possible_moves_.size() == 1)
					moveClickedPiece(possible_moves_.front());
				else
				{
					hightlighted_cells_.push_back(cur_player_[piece_on_position].getPosition());
					appendVector(hightlighted_cells_, possible_moves_);
				}
			}
		}
	}
}

void Game::clickInEditor(const sf::Event & click_event)
{
	int mouse_x = click_event.mouseButton.x, mouse_y = click_event.mouseButton.y;
	sf::Vector2f mouse_real_coords = window_.mapPixelToCoords(sf::Vector2i(mouse_x, mouse_y));
	BoardIndex click_position = draw_app_.clickPositionInBoard(mouse_x,	mouse_y);
	if (click_event.mouseButton.button == sf::Mouse::Right)
	{
		if (click_position.isInBoard())
		{
			int white_position = click_position.checkForPieces(white_player_),
				black_position = click_position.checkForPieces(black_player_);
			board_.emptyCell(click_position);
			if (white_position != -1)
			{
				//white_player_[white_position].not_beaten = false;
				shiftToRight(white_player_, white_last_checker_, white_position);
				white_last_checker_--;
				white_player_[white_last_checker_].not_beaten = false;
			}
			else if (black_position != -1)
			{
				//black_player_[black_position].not_beaten = false;
				shiftToRight(black_player_, black_last_checker_,black_position);
				black_last_checker_--;
				black_player_[black_last_checker_].not_beaten = false;
			}
		}
	}
	else if (click_event.mouseButton.button == sf::Mouse::Left)
	{
		if (click_position.isInBoard())
		{
			if (!click_position.checkForPiecesBool(board_) && ((click_position.column - 'a' + click_position.row) % 2 != 0))
			{
				if (checkers_type_in_editor_ == CheckersType::WHITE_PIECE ||
					checkers_type_in_editor_ == CheckersType::WHITE_KING)
				{
					if (white_last_checker_ < 12)
					{
						board_.getPiece(click_position) = checkers_type_in_editor_;
						white_player_[white_last_checker_].setPosition(click_position);
						white_player_[white_last_checker_].fillInfoByType(checkers_type_in_editor_);
						white_player_[white_last_checker_].not_beaten = true;
						white_last_checker_++;
					}
					else
						cout << "Limit reached!\n";
				}
				else
				{
					if (black_last_checker_ < 12)
					{
						board_.getPiece(click_position) = checkers_type_in_editor_;
						black_player_[black_last_checker_].setPosition(click_position);
						black_player_[black_last_checker_].fillInfoByType(checkers_type_in_editor_);
						black_player_[black_last_checker_].not_beaten = true;
						black_last_checker_++;
					}
					else
						cout << "Limit reached!\n";
				}
			}
		}
		else
		{
			const pair<int,sf::FloatRect>* checkers_select_positions = draw_app_.getSelectCheckersPositions();
			int i = 0;
			while (i < 4 && !checkers_select_positions[i].second.contains(mouse_real_coords))
				i++;
			if (i < 4)
			{
				active_checker_in_editor_ = i;
				checkers_type_in_editor_ = checkers_select_positions[i].first;
			}
		}
	}
}


void Game::moveClickedPiece(const BoardIndex & click_position)
{
	move_done_ = false;
	int clicked_piece_player_iter = click_position.checkForPieces(cur_player_);
	CheckersPieceWithState &piece_to_move = cur_player_[piece_firstly_clicked_];
	if (!can_beat_many_times_ && clicked_piece_player_iter != -1)
	{
		// if we clicked on piece, not on posible move
		// should clear info about current clicked piece and choose new
		// we don't wan't do anything if our piece is in the middle of move
		// (can_beat_many_times_ == true)
		int previously_clicked_piece = piece_firstly_clicked_;
		clearInfoForClickedPiece();
		appendVector(hightlighted_cells_, pieces_that_can_beat_); 
		// if we clicked on other piece - make move for it
		if (clicked_piece_player_iter != previously_clicked_piece)
			processMouseClick(click_position);
		return;
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
			int player = (white_turn_ ? moveWithPlayer::WHITE_PLAYER : moveWithPlayer::BLACK_PLAYER);
			moveWithPlayer cur_move = moveWithPlayer(
				player, piece_to_move.getPosition(), click_position, 
				piece_firstly_clicked_, possible_beat_moves_[i].second);

			board_.movePiece(piece_to_move.getPosition(), click_position);
			draw_app_.animate(cur_player_[piece_firstly_clicked_].getPosition(), click_position, cur_player_[piece_firstly_clicked_].getCheckersType());

			piece_to_move.setPosition(click_position);
			
			board_.emptyCell(another_player_[possible_beat_moves_[i].second].getPosition());
			another_player_[possible_beat_moves_[i].second].not_beaten = false;


			if (piece_to_move.transformIntoKingIfPossible())
				cur_move.became_king = true;

			possible_beat_moves_ = piece_to_move.possibleBeatMoves(cur_player_, another_player_,board_);
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
				move_done_ = true;
			}
			all_moves_.push_back(cur_move);
		}
	}
	else
	{
		if (std::find(possible_moves_.begin(), possible_moves_.end(), click_position) != possible_moves_.end())
		{
			
			int player = (white_turn_ ? moveWithPlayer::WHITE_PLAYER : moveWithPlayer::BLACK_PLAYER);
			moveWithPlayer cur_move = moveWithPlayer(player, piece_to_move.getPosition(), click_position, piece_firstly_clicked_);
			last_moves_of_cur_player_.push_back(click_position);

			draw_app_.animate(cur_player_[piece_firstly_clicked_].getPosition(), click_position, cur_player_[piece_firstly_clicked_].getCheckersType());
			board_.movePiece(cur_player_[piece_firstly_clicked_].getPosition(), click_position);
			piece_to_move.setPosition(click_position);
			
			if (piece_to_move.transformIntoKingIfPossible())
				cur_move.became_king = true;
			all_moves_.push_back(cur_move);
			//transformIntoKings();
			move_done_ = true;
		}
	}
	if (move_done_)
	{
		showMoves();
		last_moves_to_show_.clear();
		appendVector(last_moves_to_show_, last_moves_of_cur_player_);
		clearInfoForClickedPiece();
		changeTurn();
		checkForWin();
	}
}
void Game::checkForWin()
{
	int white_size = sizeOfPieces(white_player_),
		black_size = sizeOfPieces(black_player_);
	if (white_size == 0)
		game_state_ = GameState::BLACK_WINS;
	else if (black_size == 0)
		game_state_ = GameState::WHITE_WINS;
	else if (white_turn_ && !checkPlayerHasMove(white_player_))
		game_state_ = GameState::BLACK_WINS;
	else if (!white_turn_ && !checkPlayerHasMove(black_player_))
		game_state_ = GameState::WHITE_WINS;
	else
		game_state_ = GameState::RUNNING;
}
void Game::makeMove(const move & move_to_make)
{
	//int piece_to_move = move_to_make.start_position.checkForPieces(cur_player_);
	if (move_to_make.iter_piece_to_beat != -1)
	{
		board_.emptyCell(another_player_[move_to_make.iter_piece_to_beat].getPosition());
		another_player_[move_to_make.iter_piece_to_beat].not_beaten = false;
	}
	cur_player_[move_to_make.iter_piece_to_move].setPosition(move_to_make.end_position);
	cur_player_[move_to_make.iter_piece_to_move].transformIntoKingIfPossible();
	board_.movePiece(move_to_make.start_position, move_to_make.end_position);
}
void Game::checkPiecesForBeating()
{
	pieces_that_can_beat_.clear();
	for (int i=0;i<12;i++)
	{
		if (cur_player_[i].not_beaten)
		{
			if (!(cur_player_[i].possibleBeatMoves(cur_player_, another_player_, board_).empty()))
			{
				pieces_that_can_beat_.push_back(cur_player_[i].getPosition());
				hightlighted_cells_.push_back(cur_player_[i].getPosition());
			}
		}
	}
	if (!pieces_that_can_beat_.empty())
		must_beat_ = true;
	else
		must_beat_ = false;
}
void Game::transformIntoKings()
{
	for (int i = 0; i < 12; i++)
		if (cur_player_[i].not_beaten)
			if (cur_player_[i].transformIntoKingIfPossible())
				board_.makeKing(cur_player_[i].getPosition());
}

void Game::setWhiteTurn()
{
	white_turn_ = true;
	cur_player_ = white_player_;
	another_player_ = black_player_;
}
void Game::setBlackTurn()
{
	white_turn_ = false;
	cur_player_ = black_player_;
	another_player_ = white_player_;
}
void Game::loadFromFile(std::string file_name)
{
	std::ifstream in(file_name);
	clearAllStates();
	int k_white = 0, k_black = 0;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			in >> board_.checkers_map[i][j];

			switch (board_.checkers_map[i][j])
			{
				case CheckersType::WHITE_KING:
				{
					if (k_white >= 12)
					{
						cout << "Limit exceeded\n";
						break;
					}
					white_player_[k_white].not_beaten = true;
					white_player_[k_white].setColor(CheckersPiece::WHITE);
					white_player_[k_white].makeKing();
					white_player_[k_white].setPosition(intoBoardCoordinates(i, j));
					k_white++;
					break;
				}
				case CheckersType::WHITE_PIECE:
				{
					if (k_white >= 12)
					{
						cout << "Limit exceeded\n";
						break;
					}
					white_player_[k_white].not_beaten = true;
					white_player_[k_white].setColor(CheckersPiece::WHITE);
					white_player_[k_white].makePiece();
					white_player_[k_white].setPosition(intoBoardCoordinates(i, j));
					k_white++;
					break;
				}
				case CheckersType::BLACK_KING:
				{
					if (k_black >= 12)
					{
						cout << "Limit exceeded\n";
						break;
					}
					black_player_[k_black].not_beaten = true;
					black_player_[k_black].setColor(CheckersPiece::BLACK);
					black_player_[k_black].makeKing();
					black_player_[k_black].setPosition(intoBoardCoordinates(i, j));
					k_black++;
					break;
				}
				case CheckersType::BLACK_PIECE:
				{
					if (k_black >= 12)
					{
						cout << "Limit exceeded\n";
						break;
					}
					black_player_[k_black].not_beaten = true;
					black_player_[k_black].setColor(CheckersPiece::BLACK);
					black_player_[k_black].makePiece();
					black_player_[k_black].setPosition(intoBoardCoordinates(i, j));
					k_black++;
					break;
				}
				default:
					break;
			}
		}
	for (int i = k_white; i < 12; i++)
		white_player_[i].not_beaten = false;
	for (int i = k_black; i < 12; i++)
		black_player_[i].not_beaten = false;
	if (white_turn_)
		setWhiteTurn();
	else
		setBlackTurn();
	if ((is_black_ai_ && !white_turn_) || (is_white_ai_ && white_turn_))
		hightlighted_cells_.clear();
	in.close();
}
void Game::saveToFile(std::string file_name)
{
	std::ofstream out(file_name);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
			out << board_.checkers_map[i][j] << ' ';
		out << endl;
	}
	out << white_turn_;
	out.close();
}


int Game::isAiMove()
{
	if (!white_turn_ && is_black_ai_)
		return -1;
	else if (white_turn_ && is_white_ai_)
		return 1;
	else
		return 0;
}

sf::Time Game::moveAi(list<move>& best_moves_for_ai)
{
	ai_.updatePositions(white_player_, black_player_, board_);
	sf::Clock clock;
	sf::Time timer = clock.getElapsedTime();
	timer = clock.getElapsedTime() - timer;
	if (!white_turn_ && is_black_ai_)
		best_moves_for_ai = ai_.findBestMove(ai_depth_, moveWithPlayer::BLACK_PLAYER);
	else
		best_moves_for_ai = ai_.findBestMove(ai_depth_, moveWithPlayer::WHITE_PLAYER);
	for (auto x : best_moves_for_ai)
		cout << x.start_position << ' ' << x.end_position<<' ';
	cout << endl;
	ai_done_ = true;
	return timer;
}

void Game::applyAndShowMoves(const list<move>& moves_to_apply, sf::Time already_elapsed_time, bool change_turn_after)
{
	sf::Time timer = already_elapsed_time;
	sf::Clock clock;
	last_moves_to_show_.clear();
	int time_to_wait = 200;
	if (timer.asMilliseconds() <= time_to_wait)
	{
		sf::Time wait_time = sf::milliseconds(time_to_wait) - timer;
		timer = clock.getElapsedTime();
		while (clock.getElapsedTime() - timer < wait_time);
	}
	for (auto cur_move = moves_to_apply.begin(); cur_move != moves_to_apply.end(); ++cur_move)
	{
		draw_app_.animate(cur_move->start_position, cur_move->end_position,
			cur_player_[cur_move->iter_piece_to_move].getCheckersType());
		last_moves_to_show_.push_back(cur_move->start_position);
		makeMove(*cur_move);
		redrawPosition();
		clock.restart();
		if (moves_to_apply.size() != 1)
			while (clock.getElapsedTime() - timer < sf::milliseconds(time_to_wait));
		if (std::next(cur_move, 1) == moves_to_apply.end())
			last_moves_to_show_.push_back(cur_move->end_position);
		int player = (white_turn_ ? moveWithPlayer::WHITE_PLAYER : moveWithPlayer::BLACK_PLAYER);
		all_moves_.push_back(moveWithPlayer(player, *cur_move));
	}
	showMoves();
	if (change_turn_after)
	{
		changeTurn();
		checkForWin();
	}
}

void Game::redrawPosition()
{
	window_.clear(sf::Color(255, 228, 170, 255));
	int white_size = sizeOfPieces(white_player_), black_size = sizeOfPieces(black_player_);
	draw_app_.drawStaticElements();
	draw_app_.drawPieces();
	draw_app_.drawWinState();
	draw_app_.AiThinkingAnimation();
	if (game_state_ == GameState::NOT_RUNNING ||
		game_state_ == GameState::PAUSED)
	{
		sf::Texture trasparent_image;
		trasparent_image.loadFromFile("resources/images/transparent.png");
		sf::Sprite spr(trasparent_image);
		window_.draw(spr);
	}
	
	window_for_widgets_.draw();
	window_.display();
}

void Game::draw()
{
	draw_app_.drawStaticElements();
	draw_app_.drawPieces();
	draw_app_.drawWinState();
	window_for_widgets_.draw();

}

bool Game::checkPlayerHasMove(const CheckersPieceWithState* player)
{
	bool cur_has_move = true;
	int i = 0;
	do
	{
		cur_has_move = true;
		if (!player[i].not_beaten)
			cur_has_move = false;
		else if (player[i].possibleBeatMoves(cur_player_, another_player_, board_).empty() &&
			player[i].possibleMoves(cur_player_, another_player_, board_).empty())
			cur_has_move = false;
		++i;
	} while (i < 12 && !cur_has_move);
	return cur_has_move;
}

void Game::showMoves()
{
	int i = 0;
	cout << "--------------------------------------------\nMoves:\n";
	if (all_moves_.size() == 1)
	{
		cout << i + 1 << ". "<<all_moves_.front()<<endl;
		cout << "---------------------------------------\n";
		return;
	}
	for (auto iter = all_moves_.begin(); iter != all_moves_.end(); ++iter)
	{
		if (iter->player == moveWithPlayer::BLACK_PLAYER)
			i++;
		else
			cout << i + 1 << ". ";
		auto start_iter = iter;
		cout << *iter;
		while (std::next(iter) != all_moves_.end() && iter->player == std::next(iter)->player)
		{
			++iter;
			iter->showBeatPartOfMove();
		}
		if (iter->player == moveWithPlayer::BLACK_PLAYER || std::next(iter) == all_moves_.end())
			cout << endl;
		else
			cout << ' ';
	}
	cout << "---------------------------------------\n";
}

void Game::clearAllStates()
{
	std::fill(white_player_, white_player_ + 12, CheckersPieceWithState(CheckersPiece(), false));
	std::fill(black_player_, black_player_ + 12, CheckersPieceWithState(CheckersPiece(), false));
	all_moves_.clear();
	is_piece_clicked_ = false;
	setWhiteTurn();
	game_state_ = GameState::RUNNING;
	piece_firstly_clicked_ = -1;
	must_beat_ = false;
	last_moves_to_show_.clear();
	board_.clear();
	game_ended_ = false;
	clearInfoForClickedPiece();
}

Game::Game(): draw_app_(*this), widgets_app_(*this)
{
	draw_app_.setSizesAccordingToScreenResolution();
	window_.setIcon(gimp_image.width, gimp_image.height, gimp_image.pixel_data);
	window_for_widgets_.setWindow(window_);
	widgets_app_.initWidgets();
	cur_player_ = white_player_;
	another_player_ = black_player_;
	is_black_ai_ = false;
	is_white_ai_ = false;
}

void Game::Run()
{
	using std::thread;
	thread ai_thread;
	game_ended_ = false;
	
	list<move> best_moves_for_ai;
	sf::Time time_lapsed_for_ai;
	while (window_.isOpen())
	{

		if (game_mode_ == CHECKERS_GAME && game_state_ == GameState::RUNNING && isAiMove())
		{
			if (!ai_start_thinking_)
			{
				ai_start_thinking_ = true;
				ai_done_ = false;
				ai_thread = thread(&Game::moveAi,this,std::ref(best_moves_for_ai));
				ai_thread.detach();
			}
			if (ai_done_)
			{
				ai_start_thinking_ = false;
				applyAndShowMoves(best_moves_for_ai, time_lapsed_for_ai);
			}
			
		}
		sf::Event event;
		while (window_.pollEvent(event))
		{
			window_for_widgets_.handleEvent(event);

			if ((event.type == sf::Event::KeyPressed && 
				(event.key.code == sf::Keyboard::Escape || (event.key.control && event.key.code == sf::Keyboard::W))) ||
				event.type == sf::Event::Closed)
				window_.close();

			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return &&
				widgets_app_.getChooseWindow()->isVisible())
				widgets_app_.okButtonClick();

			if (event.type == sf::Event::Resized)
			{
				float aspect_ratio = draw_app_.getAspectRatio();
				cout << event.size.width << " " << event.size.width / aspect_ratio << endl;
				if (event.size.width / aspect_ratio < 0.85*draw_app_.getDisplayHeight())
					window_.setSize(sf::Vector2u(event.size.width, event.size.width / aspect_ratio));
				else
					window_.setSize(sf::Vector2u(0.85*draw_app_.getDisplayHeight()*aspect_ratio, 0.85*draw_app_.getDisplayHeight()));
			}
			
			if (game_state_ != GameState::RUNNING || ai_start_thinking_)
				continue;

			if (event.type == sf::Event::KeyReleased)
			{
				switch (event.key.code)
				{
					case sf::Keyboard::S:
						saveToFile("position.check");
						break;
					case sf::Keyboard::R:
						game_ended_ = false;
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

			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (game_mode_ == CHECKERS_GAME)
				{
					BoardIndex click_position = draw_app_.clickPositionInBoard(event.mouseButton.x, event.mouseButton.y);
					processMouseClick(click_position);
				}
				else
					clickInEditor(event);
			}
		}

		if (!game_ended_ && (game_state_ == GameState::BLACK_WINS || game_state_ == GameState::WHITE_WINS))
		{
			cout << (game_state_ == GameState::WHITE_WINS ? "White wins!\n" : "Black wins\n") << endl;
			game_ended_ = true;
		}
		redrawPosition();
	}
}