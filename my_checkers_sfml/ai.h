#pragma once
#include "checkers_pieces.h"
struct move
{
	BoardIndex start_position;
	BoardIndex end_position;
	//static const int NOT_BEAT_MOVE = -1;
	int iter_piece_to_move, iter_piece_to_beat;
	bool became_king;
	move(const BoardIndex& st_pos = BoardIndex(),
		const BoardIndex& end_pos = BoardIndex(),
		int iter_move_piece = -1,
		int iter_beaten_piece = -1,
		bool king = false): start_position(st_pos),
		end_position(end_pos),
		iter_piece_to_move(iter_move_piece),
		iter_piece_to_beat(iter_beaten_piece),
		became_king(king)
	{}
};
struct CheckersPieceWithState
{
	CheckersPiece piece;
	bool not_beaten;
	CheckersPieceWithState(const CheckersPiece& ch_piece = CheckersPiece(), bool n_beaten = true) :piece(ch_piece), not_beaten(n_beaten) {}
};
void fromListToArray(const list_pieces& source, CheckersPieceWithState *dest, int& dest_size);
typedef std::list<pair<BoardIndex, int>> list_moves_with_piece;
class Ai
{
private:
	//std::list<move> all_moves_;
	Board board_;
	

	int ai_player_;
	CheckersPieceWithState white_player_[12];
	CheckersPieceWithState black_player_[12];
	int white_player_size_, black_player_size_;

	move best_move_for_ai_;

	int number_nodes_;
	
	// iter to piece that can beat multiple times
	// if ther is no piece - iter_piece_beat_multiple_ == -1
	int iter_piece_beat_multiple_;
	
	std::list<move> generateAllMoves(int player);

	void makeMove(int player, move& move_to_make);
	void unmakeMove(int player, move& move_to_unmake);

	int evaluatePlayerPosition(int player) const;
	int evaluatePosition() const;

	int alphaBeta(int color, int depth, int max_white = INT_MIN, int min_black = INT_MAX);
	void addPossibleMove(
		const CheckersPiece& piece_to_check,
		BoardIndex(BoardIndex::*pf)() const,
		std::list<BoardIndex>& possible_moves) const;

	void addPossibleMoveBeat(
		const CheckersPiece& piece_to_check,
		BoardIndex(BoardIndex::*pf)() const,
		list_moves_with_piece& possible_moves) const;

	std::list<BoardIndex> getPossibleMoves(const CheckersPiece& piece_to_check);

	list_moves_with_piece getPossibleBeatMoves(const CheckersPiece& piece_to_check) const;

	int checkForPieces(BoardIndex position_to_check, const CheckersPieceWithState *pieces, int pieces_size) const;

	CheckersPieceWithState* getCurrentPlayer(int player);
	CheckersPieceWithState* getAnotherPlayer(int player);
	const CheckersPieceWithState* getCurrentPlayer(int player) const;
	const CheckersPieceWithState* getAnotherPlayer(int player) const;
	int getCurrentPlayerSize(int player) const;
	int getAnotherPlayerSize(int player) const;
public: 
	enum { WHITE_PLAYER, BLACK_PLAYER };
	Ai(const list_pieces& white = list_pieces(), const list_pieces& black = list_pieces(), const Board& b = Board(), int color = 0) :
		ai_player_(color) ,iter_piece_beat_multiple_(-1),
		board_(b)
	{
		fromListToArray(white, white_player_, white_player_size_);
		fromListToArray(black, black_player_, black_player_size_);
	}
	void update(const list_pieces& white, const list_pieces& black, const Board& b)
	{
		fromListToArray(white, white_player_, white_player_size_);
		fromListToArray(black, black_player_, black_player_size_);
		board_ = b;
	}
	std::list<move> findBestMove(int depth);
};