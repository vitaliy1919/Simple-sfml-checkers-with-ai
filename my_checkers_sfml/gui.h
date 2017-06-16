#pragma once
#include "app_draw.h"
#include <TGUI/TGUI.hpp>
class Game;
class AppWidgets
{
	Game *checkers_game_;
	DrawAppInstance *draw_app_;
	tgui::Gui *window_for_widgets_;
	tgui::MenuBar::Ptr main_menu_;
	tgui::Button::Ptr unmove_button_, next_move_button_;

	sf::Time animations_duration_ = sf::seconds(0.2);
	tgui::ShowAnimationType animation_type_ = tgui::ShowAnimationType::SlideFromTop;
	tgui::ChildWindow::Ptr choose_window_;
	tgui::Theme::Ptr theme_ = tgui::Theme::create();
	tgui::Label::Ptr text_label_for_level_state_ = theme_->load("Label"),
		text_label_for_level_ = theme_->load("Label");
	tgui::ComboBox::Ptr player_or_ai_choose_[2];
	tgui::Slider::Ptr level_slider_ = theme_->load("Slider");

	void gameChooseWidgetInit();
	
	void comboBoxConnectWithSlider();
	void sliderDragged(int new_value);

public:
	AppWidgets(Game& game);
	void okButtonClick();
	void cancelButtonClick();
	void undoButtonCLick();
	void menuClick(const vector<sf::String>& menu_items);
	tgui::ChildWindow::Ptr getChooseWindow() { return choose_window_; }

	void initWidgets();
};