// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include "ai.h"
#include <ctime>
list<move> Ai::generateAllMoves(int player)
{
	list<move> result;
	CheckersPieceWithState *cur_player = getCurrentPlayer(player),
		*another_player = getAnotherPlayer(player);
	if (iter_piece_beat_multiple_ == -1)
	{
		bool must_beat = false;
		int cur_player_size = getCurrentPlayerSize(player),
			another_player_size = getAnotherPlayerSize(player);
		for (int i = 0; i < cur_player_size; ++i)
		{
			if (cur_player[i].not_beaten)
			{
				CheckersPiece cur_piece = cur_player[i];
				vector<move_with_piece> cur_beat_moves = getPossibleBeatMoves(cur_piece);
				if (!cur_beat_moves.empty())
				{
					for (auto beat_move : cur_beat_moves)
						result.push_back(move(cur_piece.getPosition(), beat_move.first, i, beat_move.second));
					must_beat = true;
				}
			}
		}
		if (!must_beat)
		{
			for (int i = 0; i < cur_player_size; ++i)
			{
				if (cur_player[i].not_beaten)
				{
					CheckersPiece cur_piece = cur_player[i];
					vector<BoardIndex> cur_moves = getPossibleMoves(cur_piece);
					for (auto possible_move : cur_moves)
						result.push_back(move(cur_piece.getPosition(), possible_move, i));
				}
			}
		}
	}
	else
	{
		CheckersPieceWithState cur_piece = cur_player[iter_piece_beat_multiple_];
		vector<move_with_piece> beat_moves = getPossibleBeatMoves(cur_piece);
		for (auto beat_move : beat_moves)
			result.push_back(move(cur_piece.getPosition(), beat_move.first, iter_piece_beat_multiple_, beat_move.second));
	}
	return result;
}
void Ai::makeMove(int player, move & move_to_make)
{
	CheckersPieceWithState *cur_player = getCurrentPlayer(player),
		*another_player = getAnotherPlayer(player);
	CheckersPiece &piece_to_move = cur_player[move_to_make.iter_piece_to_move];
	piece_to_move.setPosition(move_to_make.end_position);
	board_.movePiece(move_to_make.start_position, move_to_make.end_position);

	if (!piece_to_move.isKing() &&
		piece_to_move.transformIntoKingIfPossible())
	{
		move_to_make.became_king = true;
		piece_to_move.makeKing();
		board_.makeKing(piece_to_move.getPosition());
	}

	// if on this move we beat piece
	if (move_to_make.iter_piece_to_beat != -1)
	{
		int iter_beaten_piece = move_to_make.iter_piece_to_beat;
		CheckersPieceWithState &beaten_piece = another_player[iter_beaten_piece];

		BoardIndex beaten_piece_position = beaten_piece.getPosition();
		beaten_piece.not_beaten = false;
		board_.emptyCell(beaten_piece_position);
	}
}

void Ai::unmakeMove(int player, move & move_to_unmake)
{
	CheckersPieceWithState *cur_player = getCurrentPlayer(player),
		*another_player = getAnotherPlayer(player);
	CheckersPiece &piece_to_unmove = cur_player[move_to_unmake.iter_piece_to_move];
	piece_to_unmove.setPosition(move_to_unmake.start_position);
	board_.movePiece(move_to_unmake.start_position, move_to_unmake.end_position);

	if (move_to_unmake.became_king)
	{
		move_to_unmake.became_king = false;
		piece_to_unmove.makePiece();
		board_.makePiece(piece_to_unmove.getPosition());
	}

	if (move_to_unmake.iter_piece_to_beat != -1)
	{
		int iter_beaten_piece = move_to_unmake.iter_piece_to_beat;
		BoardIndex beaten_piece_position = another_player[iter_beaten_piece].getPosition();
		CheckersPieceWithState &beaten_piece = another_player[iter_beaten_piece];

		beaten_piece.not_beaten = true;
		board_.getPiece(beaten_piece_position) = beaten_piece.getCheckersType();
	}
}

int Ai::evaluatePlayerPosition(int player) const
{
	int piece_weight = 1, king_weight = 5;
	int cur_player_evaluation = 0;

	// get pieces of this player and the number of them
	const CheckersPieceWithState *cur_player = getCurrentPlayer(player);
	int cur_player_size = getCurrentPlayerSize(player);
	for (int i = 0; i < cur_player_size; i++)
	{
		CheckersPieceWithState cur_piece = cur_player[i];
		if (cur_piece.not_beaten)
		{
			if (cur_piece.isKing())
				cur_player_evaluation += king_weight * 100;
			else
			{
				cur_player_evaluation += piece_weight * 100;
				if (player == moveWithPlayer::WHITE_PLAYER)
					cur_player_evaluation += 5 * cur_piece.getPosition().row;
				else
					cur_player_evaluation += 5 * (9 - cur_piece.getPosition().row);
				cur_player_evaluation += 7 - (2 * cur_piece.getPosition().column - 'a' - 'h');
			}
		}
	}
	return cur_player_evaluation;
}

int Ai::evaluatePosition() const
{
	int player = (
		ai_player_ == moveWithPlayer::WHITE_PLAYER ? 
		moveWithPlayer::BLACK_PLAYER : 
		moveWithPlayer::WHITE_PLAYER);
	int cur_player_evaluation = evaluatePlayerPosition(ai_player_),
		another_player_evaluation = evaluatePlayerPosition(player);
	return cur_player_evaluation - another_player_evaluation;
}

// ai - maximasing player
// player (pearson) - minimasing player
int Ai::alphaBeta(int player, int depth, int max_ai, int min_player)
{
	number_nodes_++;
	if (depth == 0)
		return evaluatePosition();
	int score = INT_MIN;
	if (ai_player_ != player)
		score = INT_MAX;
	
	int opponent_player = (player == moveWithPlayer::WHITE_PLAYER ? moveWithPlayer::BLACK_PLAYER : moveWithPlayer::WHITE_PLAYER);
	CheckersPieceWithState *cur_player = getCurrentPlayer(player), *another_player = getAnotherPlayer(player);
	list<move> all_moves = generateAllMoves(player);
	list<move>::iterator cur_move = all_moves.begin();
	
	if (cur_move == all_moves.end())
		return score;
	move best_move = *cur_move;
	while (cur_move != all_moves.end())
	{
		/*if (cur_move->start_position == BoardIndex('d', 6) && depth == 2)
			cout << "debug\n";*/
		makeMove(player, *cur_move);
		CheckersPiece moven_piece = cur_player[cur_move->iter_piece_to_move];
		int opponent_result;
		//list_moves_with_piece debug_moves = getPossibleBeatMoves(moven_piece);
		if (cur_move->iter_piece_to_beat != -1 && !getPossibleBeatMoves(moven_piece).empty())
		{
			iter_piece_beat_multiple_ = cur_move->iter_piece_to_move;
			opponent_result = alphaBeta(player, depth, max_ai, min_player);
		}
		else
		{
			iter_piece_beat_multiple_ = -1;
			opponent_result = alphaBeta(opponent_player, depth - 1, max_ai, min_player);
			/*if (all_moves.size() != 1)
				opponent_result = alphaBeta(opponent_player, depth - 1, max_ai, min_player);
			else
				opponent_result = alphaBeta(opponent_player, depth, max_ai, min_player);*/
		}
		unmakeMove(player, *cur_move);
		if (ai_player_ == player)
		{
			if (opponent_result > score)
			{
				score = opponent_result;
				best_move = *cur_move;
			}
			if (score > max_ai)
			{
				max_ai = score;
				best_move = *cur_move;
			}
			if (max_ai < score)
			{
				best_move_for_ai_ = best_move;
				return max_ai;
			}
		}
		else
		{
			if (opponent_result < score)
			{
				score = opponent_result;
				//best_move_for_ai_ = *cur_move;
			}
			if (score < min_player)
				min_player = score;
			if (score > min_player)
				return min_player;
		}
		++cur_move;
	}
	best_move_for_ai_ = best_move;
	return score;
}


vector<BoardIndex> Ai::getPossibleMoves(const CheckersPiece& piece_to_check) const
{
	int player;
	if (piece_to_check.getColor() == CheckersPiece::WHITE)
		player = moveWithPlayer::WHITE_PLAYER;
	else
		player = moveWithPlayer::BLACK_PLAYER;
	return piece_to_check.possibleMoves(getCurrentPlayer(player), getAnotherPlayer(player), board_);
}

vector<move_with_piece> Ai::getPossibleBeatMoves(const CheckersPiece& piece_to_check) const
{
	int player;
	if (piece_to_check.getColor() == CheckersPiece::WHITE)
		player = moveWithPlayer::WHITE_PLAYER;
	else
		player = moveWithPlayer::BLACK_PLAYER;
	return piece_to_check.possibleBeatMoves(getCurrentPlayer(player), getAnotherPlayer(player), board_);
}

int Ai::checkForPieces(BoardIndex position_to_check, const CheckersPieceWithState * pieces, int pieces_size) const
{
	int i = 0;
	while (i < pieces_size &&
		!(pieces[i].not_beaten &&
		pieces[i].getPosition() == position_to_check))
		i++;
	if (i >= pieces_size)
		return -1;
	else
		return i;
}

const CheckersPieceWithState * Ai::getCurrentPlayer(int player) const
{
	return (player == moveWithPlayer::WHITE_PLAYER ? white_player_ : black_player_);
}

const CheckersPieceWithState * Ai::getAnotherPlayer(int player) const
{
	return (player == moveWithPlayer::BLACK_PLAYER ? white_player_ : black_player_);
}

CheckersPieceWithState * Ai::getCurrentPlayer(int player)
{
	return (player == moveWithPlayer::WHITE_PLAYER ? white_player_ : black_player_);
}

CheckersPieceWithState * Ai::getAnotherPlayer(int player)
{
	return (player == moveWithPlayer::BLACK_PLAYER ? white_player_ : black_player_);
}
int Ai::getCurrentPlayerSize(int player) const
{
	return (player == moveWithPlayer::WHITE_PLAYER ? white_player_size_ : black_player_size_);
}

int Ai::getAnotherPlayerSize(int player) const
{
	return (player == moveWithPlayer::BLACK_PLAYER ? white_player_size_ : black_player_size_);
}



list<move> Ai::findBestMove(int depth, int color)
{
	ai_player_ = color;
	number_nodes_ = 0;
	time_t start_time = clock();
	iter_piece_beat_multiple_ = -1;
	list<move> result_moves;
	CheckersPieceWithState *cur_player = getCurrentPlayer(ai_player_);
	cout << (color == moveWithPlayer::BLACK_PLAYER ? "Black" : "White") << " Ai thinking...\n********************************\n";
	bool can_beat_multiple = false;
	do
	{
		list<move> cur_position_moves = generateAllMoves(ai_player_);
		if (cur_position_moves.size() != 1)
			alphaBeta(ai_player_, depth);
		else
			best_move_for_ai_ = cur_position_moves.front();
		makeMove(ai_player_,best_move_for_ai_);
		result_moves.push_back(best_move_for_ai_);
		CheckersPiece piece_to_check_for_multiple_beat = cur_player[best_move_for_ai_.iter_piece_to_move];
		if (best_move_for_ai_.iter_piece_to_beat != -1 &&
			!getPossibleBeatMoves(piece_to_check_for_multiple_beat).empty())
		{
			can_beat_multiple = true;
			iter_piece_beat_multiple_ = best_move_for_ai_.iter_piece_to_move;
		}
		else
			can_beat_multiple = false;
	} while (can_beat_multiple);

	time_t end_time = clock();
	float time_lapsed = (end_time - start_time) / ((float)CLOCKS_PER_SEC);
	cout << "Time lapsed " <<"(depth = "<<depth<<"): "<< time_lapsed << endl;
	cout << "Nodes created: " << number_nodes_ << endl;
	cout << "********************************\n";
	return result_moves;
}
