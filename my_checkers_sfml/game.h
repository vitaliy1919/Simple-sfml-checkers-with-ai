#pragma once
#include "checkers_pieces.h"

class Game
{
private:
	static const int kTextMargin = 20;
	vector_pieces white_player_;
	vector_pieces black_player_;
	vector<BoardIndex> hightlighted_cells_;
	vector<move_with_piece> possible_beat_moves;
	vector<BoardIndex> possible_moves;
	bool piece_clicked;
	float cell_size_;
	sf::RenderWindow window;
	bool white_turn_;

	void drawPlayersPieces(const vector_pieces player, sf::Texture piece_texture);
	sf::Vector2f getRealPosition(const BoardIndex& position) const;
	void drawBoard();
	void drawPieces();
	void playersInit();
	void movePiece(CheckersPiece& clicked_piece);
	BoardIndex clickPositionInBoard(int x,int y);
public:
	Game() :window(sf::VideoMode(700, 700), "My window"), white_player_(12), black_player_(12), white_turn_(true),cell_size_(87.5),hightlighted_cells_() {}
	
	void Run();
	
};