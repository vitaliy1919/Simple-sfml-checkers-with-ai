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
	

	//tgui::Gui *window_for_widgets_;
	sf::Texture white_piece_texture_;
	sf::Texture black_piece_texture_;
	sf::Texture white_king_texture_;
	sf::Texture black_king_texture_;
	sf::Font text_font_;
	void drawPlayersPieces(const list_pieces& player);
public:
	// this function return real position on board_ by BoardIndex 
	DrawAppInstance(sf::RenderWindow& wind);
	void init(sf::RenderWindow& wind);
	void setWindow(sf::RenderWindow& wind);
	void setSizesAccordingToScreenResolution();

	sf::Vector2f getRealPosition(const BoardIndex& position) const;
	BoardIndex clickPositionInBoard(int x, int y);

	
	void drawBoard(); 
	void drawTurn(bool white_turn);
	void drawBeatenPieces(int white_player_size, int black_player_size);

	void setMenuPosition(tgui::MenuBar::Ptr main_menu);

	void highlightCells(const vector<BoardIndex>& cells_to_highlight, sf::Color color);
	void drawStaticElements(bool is_white_move,
		const vector<BoardIndex>& higlighted, 
		const vector<BoardIndex>& last_move,
		int white_size_,
		int black_size_);
	void setWidgetsPosition(tgui::MenuBar::Ptr main_menu, tgui::Button::Ptr unmove_button, tgui::Button::Ptr move_button);
	void drawPieces(const list_pieces& white_player, const list_pieces& black_pieces);
	void drawWinState(int game_state);
};