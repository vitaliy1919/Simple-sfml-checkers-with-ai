#include "checkers_board.h"

void CheckersBoard::init(bool is_white_turn)
{
	int iter = 0;
	for (int j = 1; j <= 3; ++j)
	{
		char first = (j % 2 != 0 ? 'a' : 'b');
		for (char i = first; i <= 'h'; i += 2)
		{
			white_player_[iter].setPosition({ i,j });
			white_player_[iter].setColor(CheckersPiece::WHITE);
			iter++;
		}
	}
	iter = 0;
	for (int j = 8; j >= 6; j--)
	{
		char first = (j % 2 != 0 ? 'a' : 'b');
		for (char i = first; i <= 'h'; i += 2)
		{
			black_player_[iter].setPosition({ i,j });
			black_player_[iter].setColor(CheckersPiece::BLACK);
			iter++;
		}
	}
	if (is_white_turn)
		setWhiteTurn();
	else
		setBlackTurn();
}

void CheckersBoard::setWhiteTurn()
{
	white_turn_ = true;
	cur_player_ = &white_player_;
	another_player_ = &black_player_;
}
void CheckersBoard::setBlackTurn()
{
	white_turn_ = false;
	cur_player_ = &black_player_;
	another_player_ = &white_player_;
}

void CheckersBoard::changeTurn()
{
	white_turn_ = !white_turn_;
	std::swap(cur_player_, another_player_);
}

void CheckersBoard::checkForWin()
{
	if (white_player_.empty())
		game_state_ = BLACK_WINS;
	else if (black_player_.empty())
		game_state_ = WHITE_WINS;
	else if (!checkPlayerHasMove(white_player_))
		game_state_ = BLACK_WINS;
	else if (!checkPlayerHasMove(black_player_))
		game_state_ = WHITE_WINS;
	else
		game_state_ = NOT_ENDED;
}

//bool CheckersBoard::checkPlayerHasMove(bool check_white_player)
//{
//
//	const_pieces_iterator iter = player_to_check->begin();
//	bool cur_has_move = true;
//	do
//	{
//		cur_has_move = true;
//		if (iter->possibleBeatMoves(*cur_player_, *another_player_).empty())
//			if (iter->possibleMoves(*cur_player_, *another_player_).empty())
//				cur_has_move = false;
//		++iter;
//	} while (iter != player.end() && !cur_has_move);
//	return cur_has_move;
//
//}
//
//bool CheckersBoard::checkPlayerHasMove(const vector_pieces & player)
//{
//	bool checkPlayerHasMove(const vector_pieces & player)
//	{
//		const_pieces_iterator iter = player.begin();
//		bool cur_has_move = true;
//		do
//		{
//			cur_has_move = true;
//			if (iter->possibleBeatMoves(*cur_player_, *another_player_).empty())
//				if (iter->possibleMoves(*cur_player_, *another_player_).empty())
//					cur_has_move = false;
//			++iter;
//		} while (iter != player.end() && !cur_has_move);
//		return cur_has_move;
//	}
//
//}

