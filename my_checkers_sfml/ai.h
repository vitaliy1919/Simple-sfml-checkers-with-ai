#pragma once
#include "checkers_pieces.h"
struct move
{
	BoardIndex start_position;
	BoardIndex end_position;
	pieces_iterator iter_piece_to_move;
	CheckersPiece piece_to_move;
	bool beat_move;
	pieces_iterator iter_beaten_piece;
	CheckersPiece beaten_piece;
	move(const BoardIndex& st_pos = BoardIndex(),
		const BoardIndex& end_pos = BoardIndex(),
		pieces_iterator iter_move_piece = pieces_iterator(),
		CheckersPiece move_piece = CheckersPiece(),
		bool beat = false,
		pieces_iterator iter = pieces_iterator(),
		CheckersPiece beat_piece = CheckersPiece()) : start_position(st_pos), end_position(end_pos),
		iter_piece_to_move(iter_move_piece),
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
	

	int ai_player_;
	list_pieces white_player_;
	list_pieces black_player_;

	move best_move_for_ai_;

	int number_nodes_;
	int start_depth_;
	std::list<move> generateAllMoves(int player);
	void makeMove(int player, move& move_to_make);
	void unmakeMove(int player, move& move_to_unmake);
	int evaluatePosition();
	int alphaBeta(int color, int depth, int max_white = INT_MIN, int min_black = INT_MAX);
	list_pieces& getCurrentPlayer(int player)
	{
		return (player == WHITE_PLAYER ? white_player_ : black_player_);
	}
	list_pieces& getAnotherPlayer(int player)
	{
		return (player == BLACK_PLAYER ? white_player_ : black_player_);
	}
public: 
	enum { WHITE_PLAYER, BLACK_PLAYER };
	Ai(const list_pieces& white = list_pieces(), const list_pieces& black = list_pieces(), const Board& b = Board(), int color = 0) :
		white_player_(white), 
		black_player_(black), 
		ai_player_(color) ,
		board_(b){}
	void update(const list_pieces& white, const list_pieces& black, const Board& b)
	{
		white_player_ = white;
		black_player_ = black;
		board_ = b;
	}
	move findBestMove(int depth);
};