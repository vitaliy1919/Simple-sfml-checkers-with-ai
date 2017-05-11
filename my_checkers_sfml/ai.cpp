#include "ai.h"

std::list<move> Ai::generateAllMoves(int player)
{
	std::list<move> result;
	bool must_beat = false;
	list_pieces &cur_player = (player == WHITE_PLAYER ? white_player_ : black_player_),
		another_player = (player == BLACK_PLAYER ? white_player_ : black_player_);
	for (auto iter = cur_player.begin();iter!=cur_player.end();++iter)
	{
		vector<move_with_piece> cur_beat_moves = iter->possibleBeatMoves(cur_player,another_player,board_);
		if (!cur_beat_moves.empty())
		{
			for (auto beat_move : cur_beat_moves)
				result.push_back(move(iter->getPosition(), beat_move.first, iter, true, beat_move.second, *beat_move.second));
			must_beat = true;
		}
	}
	if (!must_beat)
	{
		for (auto iter = cur_player.begin(); iter != cur_player.end(); ++iter)
		{
			vector<BoardIndex> cur_moves = iter->possibleMoves(cur_player, another_player, board_);
			for (auto possible_move : cur_moves)
				result.push_back(move(iter->getPosition(), possible_move, iter, false));
		}
	}
}

void Ai::makeMove(int player, move & move_to_make)
{
	pieces_iterator move_piece = move_to_make.piece_to_move;
	move_piece->setPosition(move_to_make.end_position);
	std::swap(
		board_.getPiece(move_piece->getPosition()),
		board_.getPiece(move_to_make.end_position));
	if (move_to_make.beat_move)
	{
		list_pieces &cur_player = (player == WHITE_PLAYER ? white_player_ : black_player_);
		cur_player.erase(move_to_make.iter_beaten_piece);
	}
}

void Ai::unmakeMove(int player, move & move_to_unmake)
{
	pieces_iterator unmove_piece = move_to_unmake.piece_to_move;
	unmove_piece->setPosition(move_to_unmake.start_position);
	std::swap(
		board_.getPiece(unmove_piece->getPosition()),
		board_.getPiece(move_to_unmake.start_position));
	if (move_to_unmake.beat_move)
	{
		list_pieces &cur_player = (player == WHITE_PLAYER ? white_player_ : black_player_);
		cur_player.push_back(move_to_unmake.beaten_piece);
	}
}

int Ai::evaluatePosition()
{
	return white_player_.size() - black_player_.size();
}

pair<int, move> Ai::alphaBeta(int player, int depth, int max_white, int max_black)
{
	if (depth == 0)
		return std::make_pair(evaluatePosition(),best_move_);
	int score = INT_MIN;
	int opponent_player = (player == WHITE_PLAYER ? BLACK_PLAYER : WHITE_PLAYER);
	std::list<move> all_moves = generateAllMoves(player);
	auto cur_move = all_moves.begin();
	while (cur_move != all_moves.end())
	{
		makeMove(player, *cur_move);
		int opponent_result = -alphaBeta(opponent_player, depth - 1, max_white, max_black).first;
		unmakeMove(player, *cur_move);
		if (opponent_result > score)
		{
			score = opponent_result;
			best_move_ = *cur_move;
		}
	}
	return std::make_pair(score, best_move_);
}

pair<int, move> Ai::findBestMove(int depth)
{
	return alphaBeta(ai_color_,depth,0,0);
}
