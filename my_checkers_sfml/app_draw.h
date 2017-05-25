#pragma once
#include "checkers_pieces.h"
#include <TGUI/TGUI.hpp>

class DrawAppInstance
{
	sf::RenderWindow *window_;
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

	//tgui::Gui *window_for_widgets_;
	sf::Texture white_piece_texture_;
	sf::Texture black_piece_texture_;
	sf::Texture white_king_texture_;
	sf::Texture black_king_texture_;
	sf::Font text_font_;
	void drawPlayersPieces(const CheckersPieceWithState* player,int game_state,int turn);
public:
	// this function return real position on board_ by BoardIndex 
	DrawAppInstance(sf::RenderWindow& wind);
	void init(sf::RenderWindow& wind);
	void setWindow(sf::RenderWindow& wind);
	void setSizesAccordingToScreenResolution();

	sf::Vector2f getRealPosition(const BoardIndex& position) const;
	BoardIndex clickPositionInBoard(int x, int y);
	float getLeftMargin() const { return kLeftMargin; }
	float getRightMargin() const { return kRightMargin; }
	float getTopMargin() const { return kTopMargin; }
	float getBottomMargin() const { return kBottomMargin; }
	float getObjectMargin() const { return kObjectMargin; }
	float getBoardSize() const { return board_size_; }
	float getCellSize() const { return cell_size_; }
	float getAspectRatio() const { return aspect_ratio_; }
	void drawBoard(); 
	void drawTurn(bool white_turn);
	void drawBeatenPieces(int white_player_size, int black_player_size);

	void setMenuPosition(tgui::MenuBar::Ptr main_menu);

	void highlightCells(const vector<BoardIndex>& cells_to_highlight, sf::Color color, float thickness);
	void drawStaticElements(bool is_white_move,
		const vector<BoardIndex>& higlighted, 
		const vector<BoardIndex>& last_move,
		int white_size_,
		int black_size_);
	void setWidgetsPosition(tgui::MenuBar::Ptr main_menu, tgui::Button::Ptr unmove_button, tgui::Button::Ptr move_button);
	void drawPieces(const CheckersPieceWithState * white_player, const CheckersPieceWithState * black_pieces, int game_state, int turn);
	void drawWinState(int game_state);
};

class SelectGameType
{
	tgui::Gui *window_for_widgets_;
	tgui::ChildWindow::Ptr choose_window_;
	tgui::Button::Ptr ok_button_, cancel_button_;
};