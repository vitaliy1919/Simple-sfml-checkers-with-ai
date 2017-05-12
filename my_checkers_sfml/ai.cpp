// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include "ai.h"
#include <ctime>
std::list<move> Ai::generateAllMoves(int player)
{
	std::list<move> result;
	bool must_beat = false;
	list_pieces &cur_player = (player == WHITE_PLAYER ? white_player_ : black_player_),
		&another_player = (player == BLACK_PLAYER ? white_player_ : black_player_);
	for (auto piece_to_move = cur_player.begin();piece_to_move!=cur_player.end();++piece_to_move)
	{
		vector<move_with_piece> cur_beat_moves = piece_to_move->possibleBeatMoves(cur_player,another_player,board_);
		if (!cur_beat_moves.empty())
		{
			for (auto beat_move : cur_beat_moves)
				result.push_back(move(piece_to_move->getPosition(), beat_move.first, piece_to_move,*piece_to_move, true, beat_move.second, *beat_move.second));
			must_beat = true;
		}
	}
	if (!must_beat)
	{
		for (auto piece_to_move = cur_player.begin(); piece_to_move != cur_player.end(); ++piece_to_move)
		{
			//cout << *piece_to_move << endl;
			vector<BoardIndex> cur_moves = piece_to_move->possibleMoves(cur_player, another_player, board_);
			for (auto possible_move : cur_moves)
				result.push_back(move(piece_to_move->getPosition(), possible_move, piece_to_move, *piece_to_move, false));
		}
	}
	return result;
}
void Ai::makeMove(int player, move & move_to_make)
{
	//pieces_iterator move_piece = move_to_make.iter_piece_to_move;
	list_pieces &cur_player = (player == WHITE_PLAYER ? white_player_ : black_player_);
	auto new_iter_move_piece = std::find(cur_player.begin(), cur_player.end(), move_to_make.piece_to_move);
	if (new_iter_move_piece != cur_player.end())
		new_iter_move_piece->setPosition(move_to_make.end_position);
	else
	{
		cout << "Error!\n";
	}
	move_to_make.piece_to_move.setPosition(move_to_make.end_position);
	std::swap(
		board_.getPiece(move_to_make.start_position),
		board_.getPiece(move_to_make.end_position));
	if (move_to_make.beat_move)
	{
		list_pieces &another_player = (player == BLACK_PLAYER ? white_player_ : black_player_);
		auto new_iter_beaten_pos = std::find(another_player.begin(), another_player.end(), move_to_make.beaten_piece);
		if (new_iter_beaten_pos != another_player.end())
			another_player.erase(new_iter_beaten_pos);
		else
		{
			cout << "Error!\n";
		}
	}
}

void Ai::unmakeMove(int player, move & move_to_unmake)
{
	//pieces_iterator unmove_piece = move_to_unmake.iter_piece_to_move;
	list_pieces &cur_player = (player == WHITE_PLAYER ? white_player_ : black_player_);

	auto new_iter_unmove_piece = std::find(cur_player.begin(), cur_player.end(), move_to_unmake.piece_to_move);
	if (new_iter_unmove_piece != cur_player.end())
		new_iter_unmove_piece->setPosition(move_to_unmake.start_position);
	else
	{
		cout << "Error!\n";
	}
	move_to_unmake.piece_to_move.setPosition(move_to_unmake.start_position);
	std::swap(
		board_.getPiece(move_to_unmake.end_position),
		board_.getPiece(move_to_unmake.start_position));
	if (move_to_unmake.beat_move)
	{
		list_pieces &another_player = (player == BLACK_PLAYER ? white_player_ : black_player_);
		another_player.push_front(move_to_unmake.beaten_piece);
	}
}

int Ai::evaluatePosition()
{
	list_pieces *cur_player, *another_player;
	if (ai_player_ == WHITE_PLAYER)
	{
		cur_player = &white_player_;
		another_player = &black_player_;
	}
	else
	{
		cur_player = &black_player_;
		another_player = &white_player_;
	}
	int piece_weight = 1, king_weight = 5;
	int cur_player_evaluation = 0, another_player_evaluation = 0;
	for (auto x : *cur_player)
	{
		if (x.isKing())
			cur_player_evaluation += 10 * king_weight;
		else
			cur_player_evaluation += 10 * piece_weight;
	}
	for (auto x : *another_player)
	{
		if (x.isKing())
			another_player_evaluation += 10 * king_weight;
		else
			another_player_evaluation += 10 * piece_weight;
	}
	return cur_player_evaluation - another_player_evaluation;
}

int Ai::alphaBeta(int player, int depth, int max_ai, int min_player)
{
	number_nodes_++;
	if (depth == 0)
		return evaluatePosition();
	int score = INT_MIN;
	if (ai_player_ != player)
		score = INT_MAX;
	
	int opponent_player = (player == WHITE_PLAYER ? BLACK_PLAYER : WHITE_PLAYER);
	list_pieces &cur_player = getCurrentPlayer(player), &another_player = getAnotherPlayer(player);
	std::list<move> all_moves = generateAllMoves(player);
	std::list<move>::iterator cur_move = all_moves.begin();
	move best_move = *cur_move;
	while (cur_move != all_moves.end())
	{
		/*if (cur_move->start_position == BoardIndex('h', 6) && cur_move->end_position == BoardIndex('g', 5))
			cout << "debug";*/
		makeMove(player, *cur_move);
		if (!cur_move->piece_to_move.possibleBeatMoves(cur_player, another_player, board_).empty())
			opponent_player = player;
		int opponent_result = -alphaBeta(opponent_player, depth - 1, max_ai, min_player);
		unmakeMove(player, *cur_move);
		if (ai_player_ == player)
		{
			if (opponent_result > score)
			{
				score = opponent_result;
				best_move = *cur_move;
			}
			/*if (score > max_ai)
				max_ai = score;
			if (min_player <= score)
				return score;*/
		}
		else
		{
			if (opponent_result < score)
			{
				score = opponent_result;
				//best_move_for_ai_ = *cur_move;
			}
			/*if (score < min_player)
				min_player = score;
			if (max_ai > score)
				return  score;*/
		}
		
		
		++cur_move;
	}
	best_move_for_ai_ = best_move;
	return score;
}

move Ai::findBestMove(int depth)
{
	number_nodes_ = 0;
	//time_t start_time = clock();
	int best_score = alphaBeta(ai_player_,depth);
	//time_t end_time = clock();
	//float time_lapsed = (end_time - start_time) / ((float)CLOCKS_PER_SEC);
	//cout << "Time lapsed: " << time_lapsed << endl;
	cout << "Nodes created: " << number_nodes_ << endl;
	return best_move_for_ai_;
}
