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
template <typename T>
inline void copyArray(T* dest, const T* source,int size)
{
	for (int i = 0; i < size; i++)
		dest[i] = source[i];
}

typedef std::list<move_with_piece> list_moves_with_piece;
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
	Ai(int color = WHITE_PLAYER) : white_player_(), black_player_(),
		ai_player_() ,iter_piece_beat_multiple_(-1),
		board_(),number_nodes_(0)
	{
	}
	void update(const CheckersPieceWithState* white, const CheckersPieceWithState* black, const Board& b)
	{
		copyArray(white_player_, white, 12);
		copyArray(black_player_, black, 12);
		board_ = b;
	}
	std::list<move> findBestMove(int depth);
};