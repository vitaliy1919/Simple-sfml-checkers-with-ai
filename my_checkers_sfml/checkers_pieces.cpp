// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "checkers_pieces.h"

void CheckersPiece::addPossibleMoveBeat(
	BoardIndex(BoardIndex::*pf)() const,
	const CheckersPieceWithState* player_pieces, 
	const CheckersPieceWithState* opponent_pieces, Board& board,
	vector<move_with_piece>& possible_moves) const
{
	BoardIndex position_to_check = (position_.*pf)();
	if (!is_king_)
	{
		if (position_to_check.isInBoard())
		{
			int piece_to_beat_iter = position_to_check.checkForPieces(opponent_pieces);
			position_to_check = (position_to_check.*pf)();
			if (piece_to_beat_iter != -1 && position_to_check.isInBoard() &&
				!position_to_check.checkForPiecesBool(board))
			{
				possible_moves.push_back({ position_to_check,piece_to_beat_iter });
			}
		}
	}
	else
	{
		while (position_to_check.isInBoard() &&
			!position_to_check.checkForPiecesBool(board))
			position_to_check = (position_to_check.*pf)();
		int color_of_cur_player = color_;
		if (position_to_check.isInBoard() && 
			position_to_check.checkForPieces(board) != color_of_cur_player)
		{
			int piece_to_beat_iter = position_to_check.checkForPieces(opponent_pieces);
			position_to_check = (position_to_check.*pf)();
			while (position_to_check.isInBoard() &&
				!position_to_check.checkForPiecesBool(board))
			{
				possible_moves.push_back({ position_to_check,piece_to_beat_iter });
				position_to_check = (position_to_check.*pf)();
			}
		}
	}
}
vector<move_with_piece> CheckersPiece::possibleBeatMoves(
	const CheckersPieceWithState* player_pieces, 
	const CheckersPieceWithState* opponent_pieces, Board& board) const
{
	vector<move_with_piece> possible_moves;
	addPossibleMoveBeat(&BoardIndex::upperLeft, player_pieces, opponent_pieces, board, possible_moves);
	addPossibleMoveBeat(&BoardIndex::upperRight, player_pieces, opponent_pieces, board, possible_moves);
	addPossibleMoveBeat(&BoardIndex::bottomLeft, player_pieces, opponent_pieces, board, possible_moves);
	addPossibleMoveBeat(&BoardIndex::bottomRight, player_pieces, opponent_pieces, board, possible_moves);
	return possible_moves;
}
void CheckersPiece::addPossibleMove(BoardIndex(BoardIndex::*pf)() const,
	const CheckersPieceWithState* player_pieces, 
	const CheckersPieceWithState* opponent_pieces, Board& board,
	vector<BoardIndex>& possible_moves) const
{
	BoardIndex position_to_check = (position_.*pf)();
	if (!is_king_)
	{
		if (position_to_check.isInBoard())
		{
			if (!position_to_check.checkForPiecesBool(board))
				possible_moves.push_back(position_to_check);
		}
	}
	else
	{
		while (position_to_check.isInBoard() && 
			!position_to_check.checkForPiecesBool(board))
		{
			possible_moves.push_back(position_to_check);
			position_to_check = (position_to_check.*pf)();
		}
	}
}
vector<BoardIndex> CheckersPiece::possibleMoves(
	const CheckersPieceWithState* player_pieces,
	const CheckersPieceWithState* opponent_pieces,Board &board) const
{
	vector<BoardIndex> possible_moves;
	if (!is_king_)
	{
		if (color_ == WHITE)
		{
			addPossibleMove(&BoardIndex::upperLeft, player_pieces,opponent_pieces, board, possible_moves);
			addPossibleMove(&BoardIndex::upperRight, player_pieces, opponent_pieces, board, possible_moves);
		}
		else
		{
			addPossibleMove(&BoardIndex::bottomLeft, player_pieces, opponent_pieces, board, possible_moves);
			addPossibleMove(&BoardIndex::bottomRight, player_pieces, opponent_pieces, board, possible_moves);
		}
	}
	else
	{
		addPossibleMove(&BoardIndex::upperLeft, player_pieces, opponent_pieces, board, possible_moves);
		addPossibleMove(&BoardIndex::upperRight, player_pieces, opponent_pieces, board, possible_moves);
		addPossibleMove(&BoardIndex::bottomLeft, player_pieces, opponent_pieces, board, possible_moves);
		addPossibleMove(&BoardIndex::bottomRight, player_pieces, opponent_pieces, board, possible_moves);
	}
	return possible_moves;
}

bool CheckersPiece::transformIntoKingIfPossible()
{
	if ((color_ == WHITE && position_.row == 8) || (color_ == BLACK && position_.row == 1))
	{
		is_king_ = true;
		return true;
	}
	return false;
}

int CheckersPiece::getCheckersType() const
{
	if (color_ == WHITE)
	{
		if (is_king_)
			return static_cast<int>(CheckersType::WHITE_KING);
		else
			return static_cast<int>(CheckersType::WHITE_PIECE);
	}
	else
	{
		if (is_king_)
			return static_cast<int>(CheckersType::BLACK_KING);
		else
			return static_cast<int>(CheckersType::BLACK_PIECE);
	}
}


int sizeOfPieces(const CheckersPieceWithState* pieces)
{
	int count = 0;
	for (int i = 0; i < 12; ++i)
		if (pieces[i].not_beaten)
			count++;
	return count;
}

ostream & operator<<(ostream & os, const BoardIndex & to_show)
{
	os << to_show.column << to_show.row;
	return os;
}

istream & operator >> (istream & is, BoardIndex & to_input)
{
	is >> to_input.column >> to_input.row;
	return is;
}



ostream & operator<<(ostream & os, const CheckersPiece & piece_to_show)
{
	os << piece_to_show.position_ << ' ' << piece_to_show.color_ << ' ' << piece_to_show.is_king_;
	return os;
}

istream & operator >> (istream & is, CheckersPiece & piece_to_input)
{
	is >> piece_to_input.position_ >> piece_to_input.color_ >> piece_to_input.is_king_;
	return is;
}

int BoardIndex::checkForPieces(const CheckersPieceWithState* pieces) const
{
	int i = 0;
	while (i < 12 && *this == pieces[i].piece.getPosition())
		i++;
	if (i >= 12)
		return -1;
	return i;
}

int BoardIndex::checkForPieces(const Board & board) const
{
	int piece = board.getPiece(*this);
	if (piece == 0)
		return -1;
	else if (piece == static_cast<int>(CheckersType::WHITE_PIECE) ||
		piece == static_cast<int>(CheckersType::WHITE_KING))
		return CheckersPiece::WHITE;
	else
		return CheckersPiece::BLACK;
}

bool BoardIndex::checkForPiecesBool(const Board & board) const
{
	return board.getPiece(*this) != static_cast<int>(CheckersType::EMPTY);
}


void Board::movePiece(const BoardIndex & start_position, const BoardIndex & end_position)
{
	std::swap(this->getPiece(start_position),
		this->getPiece(end_position));
}

bool Board::makeKing(const BoardIndex & position)
{
	int &piece_on_position = this->getPiece(position);
	if (piece_on_position == static_cast<int>(CheckersType::WHITE_PIECE))
		piece_on_position = static_cast<int>(CheckersType::WHITE_KING);
	else if (piece_on_position == static_cast<int>(CheckersType::BLACK_PIECE))
		piece_on_position = static_cast<int>(CheckersType::BLACK_KING);
	else
		return false;
	return true;
}

bool Board::makePiece(const BoardIndex & position)
{
	int &piece_on_position = this->getPiece(position);
	if (piece_on_position == static_cast<int>(CheckersType::WHITE_KING))
		piece_on_position = static_cast<int>(CheckersType::WHITE_PIECE);
	else if (piece_on_position == static_cast<int>(CheckersType::BLACK_KING))
		piece_on_position = static_cast<int>(CheckersType::BLACK_PIECE);
	else
		return false;
	return true;
}

void Board::clear()
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			checkers_map[i][j] = static_cast<int>(CheckersType::EMPTY);
}
