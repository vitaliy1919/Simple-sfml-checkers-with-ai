#pragma once
#include "checkers_pieces.h"

class CheckersBoard
{
	Board board;
	vector_pieces white_player_;
	vector_pieces black_player_;
	vector_pieces *cur_player_;
	vector_pieces *another_player_;
	
	enum { NOT_ENDED, WHITE_WINS, BLACK_WINS }; 
	int game_state_;
	bool white_turn_;
public:
	CheckersBoard() :white_player_(12),
		black_player_(12),
		cur_player_(nullptr),
		another_player_(nullptr),
		white_turn_(true),
		game_state_(NOT_ENDED){}
	Board getBoard() const { return board; }
	vector_pieces getWhitePlayer() const { return white_player_; }
	vector_pieces getBlackPlayer() const { return black_player_; }
	vector_pieces *getCurrentPlayer() const { return cur_player_; }
	vector_pieces *getAnotherPlayer() const { return another_player_; }
	bool getWhiteTurn() const { return white_turn_; }
	int getGameState() const { return game_state_; }

	void init(bool is_white_turn = true);
	void setWhiteTurn();
	void setBlackTurn();
	void changeTurn();
	void checkForWin();
	bool checkPlayerHasMove(bool check_white_player);
};