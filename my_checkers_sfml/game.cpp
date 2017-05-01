#include "game.h"

bool CheckersPiece::canBeat(const vector<CheckersPiece>& opponent_pieces)
{
	if (position_.upperLeft())
}


ostream & operator<<(ostream & os, const BoardIndex & to_show)
{
	os << to_show.x << to_show.y;
	return os;
}

const_pieces_iterator checkForPieces(const BoardIndex& position,const vector_pieces& pieces)
{
	const_pieces_iterator iter = pieces.begin();
	while (iter != pieces.end() && iter->getPosition() != position)
		iter++;
	return iter;
}
