#pragma once
#include "checkers_pieces.h"
#include <TGUI/TGUI.hpp>
class Game;
class DrawAppInstance
{
	//friend class Game;
	Game *checkers_game_;
	sf::RenderWindow *window_;

	int piece_to_animate = -1;

	float cell_size_;
	float board_size_;
	
	float kTopMargin;
	float kLeftMargin;
	float kRightMargin;
	float kBottomMargin;

	float kObjectMargin;

	float kTopTextSize;
	float kLeftTextSize;
	float kMenuSize;

	float kBeatenPiecesSectionSize;
	float kUnmoveButtonSize;
	float kTurnRectangleWidth;
	
	float aspect_ratio_;

	
	pair<int,sf::FloatRect> select_checkers_positions[4];
	//tgui::Gui *window_for_widgets_;
	sf::Texture white_piece_texture_;
	sf::Texture black_piece_texture_;
	sf::Texture white_king_texture_;
	sf::Texture black_king_texture_;
	sf::Font text_font_;
	void drawPlayersPieces(const CheckersPieceWithState* player,int game_state,int turn);
	int display_width_ = -1;
	int display_height_ = -1;
public:
	// this function return real position on board_ by BoardIndex 
	DrawAppInstance(Game& game);
	void setSizesAccordingToScreenResolution();

	sf::Vector2f getRealPosition(const BoardIndex& position) const;
	BoardIndex clickPositionInBoard(int x, int y);
	int getDisplayWidth() { return display_width_; }
	int getDisplayHeight() { return display_height_; }
	float getLeftMargin() const { return kLeftMargin; }
	float getRightMargin() const { return kRightMargin; }
	float getTopMargin() const { return kTopMargin; }
	float getBottomMargin() const { return kBottomMargin; }
	float getObjectMargin() const { return kObjectMargin; }
	float getMenuSize() const { return kMenuSize; }
	float getBoardSize() const { return board_size_; }
	float getCellSize() const { return cell_size_; }
	float getAspectRatio() const { return aspect_ratio_; }
	const pair<int, sf::FloatRect>* getSelectCheckersPositions() const { return select_checkers_positions; };
	void drawBoard(); 
	void drawTurn(bool white_turn);
	void drawBeatenPieces(int white_player_size, int black_player_size);

	void animate(const BoardIndex& start, const BoardIndex& end, int checkers_type);

	void highlightCells(const vector<BoardIndex>& cells_to_highlight, sf::Color color, float thickness);
	void drawSelectCheckers();
	void drawStaticElements();
	void setWidgetsPosition(tgui::MenuBar::Ptr main_menu, tgui::Button::Ptr unmove_button, tgui::Button::Ptr move_button);
	void drawPieces();
	void drawWinState();
};

class SelectGameType
{
	tgui::Gui *window_for_widgets_;
	tgui::ChildWindow::Ptr choose_window_;
	tgui::Button::Ptr ok_button_, cancel_button_;
};