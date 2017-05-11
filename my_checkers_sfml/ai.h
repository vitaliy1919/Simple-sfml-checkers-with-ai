#pragma once
#include "checkers_pieces.h"
struct move
{
	BoardIndex start_position;
	BoardIndex end_position;
	pieces_iterator piece_to_move;
	bool beat_move;
	pieces_iterator iter_beaten_piece;
	CheckersPiece beaten_piece;
	move(const BoardIndex& st_pos = BoardIndex(),
		const BoardIndex& end_pos = BoardIndex(),
		pieces_iterator move_piece = pieces_iterator(),
		bool beat = false,
		pieces_iterator iter = pieces_iterator(),
		CheckersPiece beat_piece = CheckersPiece()) :
		start_position(st_pos), end_position(end_pos), 
		piece_to_move(move_piece),
		beat_move(beat),
		iter_beaten_piece(iter),
		beaten_piece(beat_piece){}
};
class Ai
{
private:
	//std::list<move> all_moves_;
	Board board_;
	

	int ai_color_;
	list_pieces white_player_;
	list_pieces black_player_;

	move best_move_;

	std::list<move> generateAllMoves(int player);
	void makeMove(int player, move& move_to_make);
	void unmakeMove(int player, move& move_to_unmake);
	int evaluatePosition();
	pair<int,move> alphaBeta(int color, int depth, int maxWhite, int maxBlack);
public: 
	enum { WHITE_PLAYER, BLACK_PLAYER };
	Ai(const list_pieces& white, const list_pieces& black, const Board& b, int color) :
		white_player_(white), 
		black_player_(black), 
		ai_color_(color) ,
		board_(b){}
	void update(const list_pieces& white, const list_pieces& black, const Board& b)
	{
		white_player_ = white;
		black_player_ = black;
		board_ = b;
	}
	pair<int, move> findBestMove(int depth);
};