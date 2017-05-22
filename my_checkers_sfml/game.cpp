// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Game.h"
#include <ctime>

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

void Game::gameChooseWidgetInit()
{
	tgui::Button::Ptr ok_button = theme_->load("Button"), cancel_button = theme_->load("Button");
	tgui::Label::Ptr text_label_for_white = theme_->load("Label"),
		text_label_for_black = theme_->load("Label"),
		text_label_for_level = theme_->load("Label"),
		text_label_for_vs = theme_->load("Label");
	
	choose_window_ = theme_->load("ChildWindow");
	choose_window_->setResizable(true);
	choose_window_->setTitle("Choose game settings: ");
	choose_window_->setSize(window_.getSize().x / 2, window_.getSize().y / 4.5);
	choose_window_->setTitleButtons(tgui::ChildWindow::None);

	tgui::Layout2d center_coord = tgui::Layout2d(
		int((window_.getSize().x - choose_window_->getSize().x) / 2),
		int((window_.getSize().y - choose_window_->getSize().y) / 2));
	choose_window_->setPosition(center_coord);
	window_for_widgets_.add(choose_window_);

	ok_button->setText("OK");	
	cancel_button->setText("Cancel");
	text_label_for_white->setText("White player:");
	text_label_for_black->setText("Black player:");
	//text_label_for_vs->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	text_label_for_vs->setSize(20, text_label_for_vs->getSize().y);
	text_label_for_vs->setAutoSize(true);
	text_label_for_vs->setText("vs");
	text_label_for_level->setText("Ai level: ");
	text_label_for_level_state_->setText("1");
	tgui::Layout2d 
		choose_size = choose_window_->getSize(),
		ok_size = ok_button->getSize(), 
		cancel_size = cancel_button->getSize();
	float object_margin = 2*draw_app_.getObjectMargin();

	text_label_for_white->setPosition((choose_size.x / 2 - text_label_for_white->getSize().x) / 2, object_margin);
	text_label_for_black->setPosition(choose_size.x / 2 + (choose_size.x / 2 - text_label_for_black->getSize().x) / 2, object_margin);
	choose_window_->add(text_label_for_white);
	choose_window_->add(text_label_for_black);

	player_or_ai_choose_[0] = theme_->load("ComboBox");
	player_or_ai_choose_[1] = theme_->load("ComboBox");
	player_or_ai_choose_[0]->addItem("Player");
	player_or_ai_choose_[0]->addItem("Ai");
	player_or_ai_choose_[1]->addItem("Player");
	player_or_ai_choose_[1]->addItem("Ai");

	player_or_ai_choose_[0]->setPosition(
		(choose_size.x / 2 - player_or_ai_choose_[0]->getSize().x) / 2,
		2 * object_margin + text_label_for_white->getSize().y);
	player_or_ai_choose_[1]->setPosition(
		choose_size.x / 2 + (choose_size.x / 2 - player_or_ai_choose_[1]->getSize().x) / 2,
		2 * object_margin + text_label_for_black->getSize().y);
	player_or_ai_choose_[0]->setSelectedItemByIndex(0);
	player_or_ai_choose_[1]->setSelectedItemByIndex(0);
	choose_window_->add(player_or_ai_choose_[0]);
	choose_window_->add(player_or_ai_choose_[1]);
	
	text_label_for_level->setPosition(
		2 * object_margin, 
		player_or_ai_choose_[0]->getPosition().y + player_or_ai_choose_[0]->getSize().y + 2 * object_margin);
	choose_window_->add(text_label_for_level);
	
	level_slider_->setPosition(
		text_label_for_level->getPosition().x+text_label_for_level->getSize().x+object_margin,
		player_or_ai_choose_[0]->getPosition().y + player_or_ai_choose_[0]->getSize().y + 2 * object_margin);
	level_slider_->setMinimum(1);
	level_slider_->setMaximum(9);
	level_slider_->setValue(1);
	choose_window_->add(level_slider_);

	text_label_for_level_state_->setPosition(
		level_slider_->getPosition().x + level_slider_->getSize().x + object_margin,
		level_slider_->getPosition().y);
	choose_window_->add(text_label_for_level_state_);
	level_slider_->connect("ValueChanged", &Game::sliderDragged, this);

	text_label_for_vs->setPosition((choose_size.x - text_label_for_vs->getSize().x) / 2, player_or_ai_choose_[0]->getPosition().y);
	choose_window_->add(text_label_for_vs);
	ok_button->setPosition(
		(choose_size.x - ok_size.x - cancel_size.x - object_margin) / 2,
		choose_size.y - object_margin - ok_size.y);
	cancel_button->setPosition(
		(choose_size.x + ok_size.x + object_margin - cancel_size.x) / 2,
		choose_size.y - object_margin - cancel_size.y);

	ok_button->connect("pressed", &Game::okButtonClick,this);
	cancel_button->connect("pressed", &Game::cancelButtonClick,this);
	choose_window_->add(ok_button);
	choose_window_->add(cancel_button);
}

void Game::okButtonClick()
{
	
	if (player_or_ai_choose_[0]->getSelectedItemIndex() == 0)
		is_white_ai_ = false;
	else
		is_white_ai_ = true;
	if (player_or_ai_choose_[1]->getSelectedItemIndex() == 0)
		is_black_ai_ = false;
	else
		is_black_ai_ = true;
	ai_depth_ = level_slider_->getValue();
	clearAllStates();
	playersInit();
	//choose_window_->hide();
	choose_window_->hideWithEffect(animation_type_,animations_duration_);
}

void Game::cancelButtonClick()
{
	if (game_state_ == static_cast<int>(GameState::PAUSED))
		game_state_ = static_cast<int>(GameState::RUNNING);
	choose_window_->hideWithEffect(animation_type_, animations_duration_);
}

void Game::menuClick(const vector<sf::String>& menu_items)
{
	int i = 0;
	sf::Time timer;
	timer = sf::seconds(0.5);
	if (menu_items[i] == "Game")
	{
		i++;
		if (menu_items[i] == "New game")
		{
			choose_window_->showWithEffect(animation_type_,animations_duration_);
			if (game_state_ == static_cast<int>(GameState::RUNNING))
				game_state_ = static_cast<int>(GameState::PAUSED);
			else
				game_state_ = static_cast<int>(GameState::NOT_RUNNING);
		}
		if (menu_items[i] == "Quit")
			window_.close();
	}
}

void Game::sliderDragged(int new_value)
{
	text_label_for_level_state_->setText(std::to_string(new_value));
}



void Game::widgetsInit()
{
	main_menu_ = theme_->load("MenuBar");
	main_menu_->setSize(tgui::Layout2d(float(window_.getSize().x), 25));
	main_menu_->addMenu("Game");
	main_menu_->addMenuItem("Game", "New game");
	main_menu_->addMenuItem("Game", "Staticstics");
	main_menu_->addMenuItem("Game","Quit");
	main_menu_->connect("MenuItemClicked",&Game::menuClick,this);
	unmove_button_ = theme_->load("Button");
	next_move_button_ = theme_->load("Button");
	unmove_button_->setText("Undo move");
	next_move_button_->setText("Next move");
	unmove_button_->hide();
	next_move_button_->hide();
	window_for_widgets_.add(main_menu_);
	window_for_widgets_.add(unmove_button_);
	window_for_widgets_.add(next_move_button_);
	draw_app_.setWidgetsPosition(main_menu_, unmove_button_, next_move_button_);
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
		last_moves_to_show_.clear();
		appendVector(last_moves_to_show_, last_moves_of_cur_player_);
		clearInfoForClickedPiece();
		checkForWin();
		changeTurn();
	}
}
void Game::checkForWin()
{
	if (white_player_.empty())
		game_state_ = static_cast<int>(GameState::BLACK_WINS);
	else if (black_player_.empty())
		game_state_ = static_cast<int>(GameState::WHITE_WINS);
	else if (!checkPlayerHasMove(white_player_))
		game_state_ = static_cast<int>(GameState::BLACK_WINS);
	else if (!checkPlayerHasMove(black_player_))
		game_state_ = static_cast<int>(GameState::WHITE_WINS);
	else
		game_state_ = static_cast<int>(GameState::RUNNING);
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

void Game::Run()
{
	playersInit();
	checkPiecesForBeating();
	game_ended_ = false;
	while (window_.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		
		if (game_state_ == static_cast<int>(GameState::RUNNING) &&
			!game_ended_ && ((!white_turn_ && is_black_ai_) || (white_turn_ && is_white_ai_)))
		{
			if (!white_turn_ && is_black_ai_)
				black_ai_.update(white_player_, black_player_, board_);
			else
				white_ai_.update(white_player_, black_player_, board_);
			sf::Clock clock;
			sf::Time timer = clock.getElapsedTime();
			std::list<move> best_moves_for_ai;
			timer = clock.getElapsedTime() - timer;
			if (!white_turn_ && is_black_ai_)
				best_moves_for_ai = black_ai_.findBestMove(ai_depth_);
			else
				best_moves_for_ai = white_ai_.findBestMove(ai_depth_);

			last_moves_to_show_.clear();
			
			if (timer.asMilliseconds() <= 500)
			{
				sf::Time wait_time = sf::milliseconds(500) - timer;
				timer = clock.getElapsedTime();
				while (clock.getElapsedTime() - timer < wait_time);
			}
			for (auto cur_best_move = best_moves_for_ai.begin(); cur_best_move!=best_moves_for_ai.end();++cur_best_move)
			{
				
				last_moves_to_show_.push_back(cur_best_move->start_position);
				makeMove(*cur_best_move);
				redrawPosition();
				clock.restart();
				if (best_moves_for_ai.size() != 1)
					while (clock.getElapsedTime() - timer < sf::milliseconds(500));
				if (std::next(cur_best_move, 1) == best_moves_for_ai.end())
					last_moves_to_show_.push_back(cur_best_move->end_position);
			}
			checkForWin();
			changeTurn();
		}
		sf::Event event;
		while (window_.pollEvent(event))
		{
			window_for_widgets_.handleEvent(event);
			// "close requested" event: we close the window
			if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) || 
				event.type == sf::Event::Closed)
				window_.close();
			if (event.type == sf::Event::Resized)
			{
				float aspect_ratio = draw_app_.getAspectRatio();
				window_.setSize(sf::Vector2u(event.size.width, event.size.width/aspect_ratio));
				//window_.setView(sf::View(sf::FloatRect(0,0,event.size.width,event.size.width)));
			}
			if (event.type == sf::Event::MouseWheelMoved)
				choose_window_->show();

			if (game_state_ != static_cast<int>(GameState::RUNNING))
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
			if (!game_ended_)
			{
				if (event.type == sf::Event::MouseButtonReleased)
				{
					BoardIndex click_position = draw_app_.clickPositionInBoard(event.mouseButton.x, event.mouseButton.y);
					//std::cout << click_position << endl;
					processMouseClick(click_position);

				}
			}
		}
		
		if (!game_ended_ && (game_state_ == static_cast<int>(GameState::BLACK_WINS) || game_state_ == static_cast<int>(GameState::WHITE_WINS)))
		{
			cout << (game_state_ == static_cast<int>(GameState::WHITE_WINS) ? "White wins!\n" : "Black wins\n") << endl;
			game_ended_ = true;
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


void Game::redrawPosition()
{
	window_.clear(sf::Color(255, 228, 170, 255));
	draw_app_.drawStaticElements(white_turn_, hightlighted_cells_, last_moves_to_show_, white_player_.size(), black_player_.size());
	int cur_color = (white_turn_ ? CheckersPiece::WHITE : CheckersPiece::BLACK);
	draw_app_.drawPieces(white_player_,black_player_,game_state_,cur_color);
	draw_app_.drawWinState(game_state_);
	window_for_widgets_.draw();
	window_.display();
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
	window_for_widgets_.removeAllWidgets();
	widgetsInit();
	draw_app_.setWidgetsPosition(main_menu_,unmove_button_,next_move_button_);
	gameChooseWidgetInit();
	white_player_.clear();
	black_player_.clear();
	is_piece_clicked_ = false;
	setWhiteTurn();
	game_state_ = static_cast<int>(GameState::RUNNING);
	piece_firstly_clicked_ = nullptr;
	must_beat_ = false;
	last_moves_to_show_.clear();
	board_.clear();
	game_ended_ = false;
	clearInfoForClickedPiece();
}

Game::Game(int game_mode, int ai_level) :
	white_player_(),
	black_player_(),
	white_turn_(true),
	hightlighted_cells_(),
	possible_beat_moves_(),
	possible_moves_(),
	is_piece_clicked_(false),
	piece_firstly_clicked_(),
	cur_player_(&white_player_),
	another_player_(&black_player_),
	pieces_that_can_beat_(),
	must_beat_(false),
	game_state_(static_cast<int>(GameState::NOT_RUNNING)),
	can_beat_many_times_(false),
	ai_depth_(ai_level),
	draw_app_(window_)
{
	draw_app_.setSizesAccordingToScreenResolution();
	window_for_widgets_.setWindow(window_);
	window_.setIcon(gimp_image.width, gimp_image.height, gimp_image.pixel_data);
	widgetsInit();
	gameChooseWidgetInit();
	is_black_ai_ = false;
	is_white_ai_ = false;
	animations_duration_ = sf::seconds(0.2);
	if (game_mode == WHITE_AI || game_mode == TWO_AI)
		is_white_ai_ = true;
	if (game_mode == BLACK_AI || game_mode == TWO_AI)
		is_black_ai_ = true;
	black_ai_ = Ai(white_player_, black_player_, board_, Ai::BLACK_PLAYER);
	white_ai_ = Ai(white_player_, black_player_, board_, Ai::WHITE_PLAYER);
}
