// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "checkers_pieces.h"

void addPossibleMoveBeat(BoardIndex position,
	BoardIndex(BoardIndex::*pf)() const,
	vector_pieces& player_pieces, vector_pieces& opponent_pieces,
	vector<move_with_piece>& possible_moves)
{
	BoardIndex position_to_check = (position.*pf)();
	if (position_to_check.isInBoard())
	{
		pieces_iterator piece_position = position_to_check.checkForPieces(opponent_pieces);
		position_to_check = (position_to_check.*pf)();		
		if (piece_position != opponent_pieces.end() && position_to_check.isInBoard() &&
				!position_to_check.checkForPiecesBool(opponent_pieces) &&
				!position_to_check.checkForPiecesBool(player_pieces))  
		{
			possible_moves.push_back({ position_to_check,piece_position });
		}
	}
}
vector<move_with_piece> CheckersPiece::possibleBeatMoves(vector_pieces& player_pieces, vector_pieces& opponent_pieces)
{
	vector<move_with_piece> possible_moves;
	addPossibleMoveBeat(position_, &BoardIndex::upperLeft, player_pieces, opponent_pieces, possible_moves);
	addPossibleMoveBeat(position_, &BoardIndex::upperRight, player_pieces, opponent_pieces, possible_moves);
	addPossibleMoveBeat(position_, &BoardIndex::bottomLeft, player_pieces, opponent_pieces, possible_moves);
	addPossibleMoveBeat(position_, &BoardIndex::bottomRight, player_pieces, opponent_pieces, possible_moves);
	return possible_moves;
}
void addPossibleMove(BoardIndex position, BoardIndex(BoardIndex::*pf)() const,
	vector_pieces& player_pieces, vector_pieces& opponent_pieces, 
	vector<BoardIndex>& possible_moves)
{
	BoardIndex position_to_check = (position.*pf)();
	if (position_to_check.isInBoard())
	{
		if (!position_to_check.checkForPiecesBool(opponent_pieces) &&
			!position_to_check.checkForPiecesBool(player_pieces))
			possible_moves.push_back(position_to_check);
	}
}
vector<BoardIndex> CheckersPiece::possibleMoves( vector_pieces& player_pieces, vector_pieces& opponent_pieces) const
{
	vector<BoardIndex> possible_moves;
	if (!is_king_)
	{
		if (color_ == WHITE)
		{
			addPossibleMove(position_, &BoardIndex::upperLeft, player_pieces,opponent_pieces,possible_moves);
			addPossibleMove(position_, &BoardIndex::upperRight, player_pieces, opponent_pieces, possible_moves);
		}
		else
		{
			addPossibleMove(position_, &BoardIndex::bottomLeft, player_pieces, opponent_pieces, possible_moves);
			addPossibleMove(position_, &BoardIndex::bottomRight, player_pieces, opponent_pieces, possible_moves);
		}
	}
	return possible_moves;
}


ostream & operator<<(ostream & os, const BoardIndex & to_show)
{
	os << to_show.column << to_show.row;
	return os;
}

pieces_iterator checkForPieces(const BoardIndex& position,vector_pieces& pieces)
{
	return position.checkForPieces(pieces);
}

pieces_iterator BoardIndex::checkForPieces(vector_pieces & pieces) const
{
	pieces_iterator iter = pieces.begin();
	while (iter != pieces.end() && iter->getPosition() != *this)
		++iter;
	return iter;
}


//void BoardIndex::checkForBeatingAndAddToPossibleMoves(const vector_pieces & pieces, vector<move_with_piece> possible_moves) const
//{
//	BoardIndex position_to_check = this->upperRight();
//
//	if (position_to_check.isInBoard())
//	{
//		const_pieces_iterator piece_position = position_to_check.checkForPieces(pieces);
//		position_to_check = position_to_check.upperRight();
//		if (position_to_check.isInBoard())
//			possible_moves.push_back({ position_to_check,piece_position });
//	}
//}
