#include "gui.h"
#include "Game.h"
AppWidgets::AppWidgets(Game & game)
{
	checkers_game_ = &game;
	draw_app_ = &game.draw_app_;
	window_for_widgets_ = &game.window_for_widgets_;
}


void AppWidgets::okButtonClick()
{
	if (player_or_ai_choose_[0]->getSelectedItemIndex() == 0)
		checkers_game_->is_white_ai_ = false;
	else
		checkers_game_->is_white_ai_ = true;
	if (player_or_ai_choose_[1]->getSelectedItemIndex() == 0)
		checkers_game_->is_black_ai_ = false;
	else
		checkers_game_->is_black_ai_ = true;
	checkers_game_->ai_depth_ = level_slider_->getValue();
	if (player_or_ai_choose_[0]->getSelectedItemIndex() == 1)
		choose_window_->hide();
	else
		choose_window_->hideWithEffect(animation_type_, animations_duration_);
	checkers_game_->clearAllStates();
	checkers_game_->playersInit();
}

void AppWidgets::cancelButtonClick()
{
	if (checkers_game_->game_state_ == GameState::PAUSED)
		checkers_game_->game_state_ = GameState::RUNNING;
	if (player_or_ai_choose_[0]->getSelectedItemIndex() == 1)
		choose_window_->hide();
	else
		choose_window_->hideWithEffect(animation_type_, animations_duration_);
}

void AppWidgets::undoButtonCLick()
{
	if (checkers_game_->game_mode_ == checkers_game_->POSITION_EDITOR)
	{
		checkers_game_->ai_depth_ = 5;
		checkers_game_->game_mode_ = checkers_game_->CHECKERS_GAME;
		//choose_window_->showWithEffect(animation_type_, animations_duration_);
		return;
	}
	if (!checkers_game_->all_moves_.empty())
	{
		auto iter = checkers_game_->all_moves_.rbegin();
		bool last_move_ai = false;
		if ((iter->player == moveWithPlayer::WHITE_PLAYER && checkers_game_->is_white_ai_) ||
			(iter->player == moveWithPlayer::BLACK_PLAYER && checkers_game_->is_black_ai_))
		{
			last_move_ai = true;
			do
			{
				unmakeMove(*iter, checkers_game_->white_player_, checkers_game_->black_player_, checkers_game_->board_);
				++iter;
			} while ((iter != checkers_game_->all_moves_.rend() || std::prev(iter) == checkers_game_->all_moves_.rend()) &&
				std::prev(iter)->player == iter->player);
			checkers_game_->all_moves_.erase(iter.base(), checkers_game_->all_moves_.end());

		}
		if (!checkers_game_->all_moves_.empty())
		{
			iter = checkers_game_->all_moves_.rbegin();
			do
			{
				unmakeMove(*iter, checkers_game_->white_player_, checkers_game_->black_player_, checkers_game_->board_);
				++iter;
			} while ((iter != checkers_game_->all_moves_.rend() || std::prev(iter) == checkers_game_->all_moves_.rend()) &&
				std::prev(iter)->player == iter->player);
			if (!last_move_ai && checkers_game_->move_done_)
			{
				checkers_game_->last_moves_to_show_.clear();
				checkers_game_->hightlighted_cells_.clear();
				checkers_game_->changeTurn();
			}
			else
			{

				checkers_game_->clearInfoForClickedPiece();
				checkers_game_->checkPiecesForBeating();
			}
			checkers_game_->all_moves_.erase(iter.base(), checkers_game_->all_moves_.end());

		}
		/*all_moves_.erase(iter.base(), all_moves_.end());
		if (iter!=all_moves_.rend() &&
		(is_white_ai_ && iter->player == moveWithPlayer::WHITE_PLAYER) ||
		(is_black_ai_ && iter->player == moveWithPlayer::BLACK_PLAYER))
		{
		do
		{
		unmakeMove(*iter, white_player_, black_player_, board_);
		++iter;
		} while ((iter != all_moves_.rend() || std::prev(iter) == all_moves_.rend()) && std::prev(iter)->player == iter->player);
		}
		all_moves_.erase(iter.base(), all_moves_.end());*/


	}
}

void AppWidgets::comboBoxConnectWithSlider()
{
	if (player_or_ai_choose_[0]->getSelectedItemIndex() == 0 &&
		player_or_ai_choose_[1]->getSelectedItemIndex() == 0)
	{
		level_slider_->hide();
		text_label_for_level_state_->hide();
		text_label_for_level_->hide();
	}
	else
	{
		level_slider_->show();
		text_label_for_level_state_->show();
		text_label_for_level_->show();
	}
}

void AppWidgets::menuClick(const vector<sf::String>& menu_items)
{
	int i = 0;
	sf::Time timer;
	timer = sf::seconds(0.5);
	if (menu_items[i] == "Game")
	{
		checkers_game_->game_mode_ = checkers_game_->CHECKERS_GAME;
		i++;
		if (menu_items[i] == "New game with ai")
		{
			window_for_widgets_->removeAllWidgets();
			initWidgets();
			choose_window_->showWithEffect(animation_type_, animations_duration_);
			if (checkers_game_->game_state_ == GameState::RUNNING)
				checkers_game_->game_state_ = GameState::PAUSED;
			else
				checkers_game_->game_state_ = GameState::NOT_RUNNING;
		}
		if (menu_items[i] == "New game for two players")
		{
			choose_window_->hideWithEffect(animation_type_, animations_duration_);
			checkers_game_->is_white_ai_ = checkers_game_->is_black_ai_ = false;
			checkers_game_->clearAllStates();
			checkers_game_->playersInit();
		}
		if (menu_items[i] == "Quit")
			checkers_game_->window_.close();
	}
	if (menu_items[i] == "Create position")
	{
		checkers_game_->clearAllStates();
		checkers_game_->draw_app_.drawSelectCheckers();
		checkers_game_->game_mode_ = checkers_game_->POSITION_EDITOR;
		checkers_game_->checkers_type_in_editor_ = checkers_game_->draw_app_.getSelectCheckersPositions()[2].first;
		checkers_game_->white_last_checker_ = checkers_game_->black_last_checker_ = 0;
		checkers_game_->active_checker_in_editor_ = 2;
	}
}

void AppWidgets::sliderDragged(int new_value)
{
	text_label_for_level_state_->setText(std::to_string(new_value));
}

void AppWidgets::gameChooseWidgetInit()
{
	tgui::Button::Ptr ok_button = theme_->load("Button"), cancel_button = theme_->load("Button"),
		reverce_button = theme_->load("Button");
	tgui::Label::Ptr text_label_for_white = theme_->load("Label"),
		text_label_for_black = theme_->load("Label"),
		text_label_for_vs = theme_->load("Label");

	choose_window_ = theme_->load("ChildWindow");
	choose_window_->setResizable(true);
	choose_window_->setTitle("Choose game settings: ");
	choose_window_->setSize(checkers_game_->window_.getSize().x / 2, checkers_game_->window_.getSize().y / 4.5);
	choose_window_->setTitleButtons(tgui::ChildWindow::None);

	tgui::Layout2d center_coord = tgui::Layout2d(
		int((checkers_game_->window_.getSize().x - choose_window_->getSize().x) / 2),
		int((checkers_game_->window_.getSize().y - choose_window_->getSize().y) / 2));
	choose_window_->setPosition(center_coord);
	window_for_widgets_->add(choose_window_);

	ok_button->setText("OK");
	cancel_button->setText("Cancel");
	text_label_for_white->setText("White player:");
	text_label_for_black->setText("Black player:");
	//text_label_for_vs->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	text_label_for_vs->setSize(20, text_label_for_vs->getSize().y);
	text_label_for_vs->setAutoSize(true);
	text_label_for_vs->setText("vs");
	reverce_button->setText("->\n<-");

	text_label_for_level_->setText("Ai level: ");
	text_label_for_level_state_->setText("1");
	tgui::Layout2d
		choose_size = choose_window_->getSize(),
		ok_size = ok_button->getSize(),
		cancel_size = cancel_button->getSize();
	float object_margin = 2 * draw_app_->getObjectMargin();

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
	player_or_ai_choose_[1]->setSelectedItemByIndex(1);
	player_or_ai_choose_[0]->connect("ItemSelected", &AppWidgets::comboBoxConnectWithSlider, this);
	player_or_ai_choose_[1]->connect("ItemSelected", &AppWidgets::comboBoxConnectWithSlider, this);
	choose_window_->add(player_or_ai_choose_[0]);
	choose_window_->add(player_or_ai_choose_[1]);

	text_label_for_level_->setPosition(
		2 * object_margin,
		player_or_ai_choose_[0]->getPosition().y + player_or_ai_choose_[0]->getSize().y + 2 * object_margin);
	choose_window_->add(text_label_for_level_);

	level_slider_->setPosition(
		text_label_for_level_->getPosition().x + text_label_for_level_->getSize().x + object_margin,
		player_or_ai_choose_[0]->getPosition().y + player_or_ai_choose_[0]->getSize().y + 2 * object_margin);
	level_slider_->setMinimum(1);
	level_slider_->setMaximum(9);
	level_slider_->setValue(1);
	choose_window_->add(level_slider_);
	comboBoxConnectWithSlider();

	text_label_for_level_state_->setPosition(
		level_slider_->getPosition().x + level_slider_->getSize().x + object_margin,
		level_slider_->getPosition().y);
	choose_window_->add(text_label_for_level_state_);
	level_slider_->connect("ValueChanged", &AppWidgets::sliderDragged, this);

	text_label_for_vs->setPosition((choose_size.x - text_label_for_vs->getSize().x) / 2, player_or_ai_choose_[0]->getPosition().y);
	reverce_button->setPosition((choose_size.x - reverce_button->getSize().x) / 2, player_or_ai_choose_[0]->getPosition().y);
	choose_window_->add(text_label_for_vs);
	//choose_window_->add(reverce_button);

	ok_button->setPosition(
		(choose_size.x - ok_size.x - cancel_size.x - object_margin) / 2,
		choose_size.y - object_margin - ok_size.y);
	cancel_button->setPosition(
		(choose_size.x + ok_size.x + object_margin - cancel_size.x) / 2,
		choose_size.y - object_margin - cancel_size.y);

	ok_button->connect("pressed", &AppWidgets::okButtonClick, this);
	cancel_button->connect("pressed", &AppWidgets::cancelButtonClick, this);
	choose_window_->add(ok_button);
	choose_window_->add(cancel_button);
}
void AppWidgets::initWidgets()
{
	float kLeftMargin = draw_app_->getLeftMargin(),
		kRightMargin = draw_app_->getRightMargin(),
		kTopMargin = draw_app_->getTopMargin(),
		kObjectMargin = draw_app_->getObjectMargin(),
		kMenuSize = draw_app_->getMenuSize(),
		board_size = draw_app_->getBoardSize();
	
	main_menu_ = theme_->load("MenuBar");

	main_menu_->setSize(tgui::Layout2d(float(checkers_game_->window_.getSize().x), 25));
	main_menu_->addMenu("Game");
	main_menu_->addMenuItem("Game", "New game with ai");
	main_menu_->addMenuItem("Game", "New game for two players");
	main_menu_->addMenuItem("Game", "Staticstics");
	main_menu_->addMenuItem("Game", "Quit");
	main_menu_->addMenu("Create position");
	main_menu_->addMenuItem("Create position", "New position");
	main_menu_->connect("MenuItemClicked", &AppWidgets::menuClick, this);

	unmove_button_ = theme_->load("Button");
	next_move_button_ = theme_->load("Button");

	unmove_button_->setText("Undo move");
	unmove_button_->connect("pressed", &AppWidgets::undoButtonCLick, this);

	next_move_button_->setText("Next move");
	next_move_button_->hide();

	window_for_widgets_->add(main_menu_);
	window_for_widgets_->add(unmove_button_);
	window_for_widgets_->add(next_move_button_);

	main_menu_->setSize(tgui::Layout2d(kLeftMargin + board_size + kRightMargin, kMenuSize));
	unmove_button_->setPosition(kLeftMargin, kTopMargin + board_size + kObjectMargin);
	next_move_button_->setPosition(kLeftMargin + unmove_button_->getSize().x + kObjectMargin, kTopMargin + board_size + kObjectMargin);

	gameChooseWidgetInit();
}

