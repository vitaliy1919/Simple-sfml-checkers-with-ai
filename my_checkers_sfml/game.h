#pragma once
#include <iostream>
#include <vector>
using std::vector;
using std::ostream;
using std::endl;

class CheckersPiece;
struct BoardIndex;
typedef vector<CheckersPiece> vector_pieces;
typedef vector_pieces::iterator pieces_iterator;
typedef vector_pieces::const_iterator const_pieces_iterator;


struct BoardIndex
{
	char	column;
	int		row;
	BoardIndex(char col, int r) :column(col), row(r) {}
	BoardIndex upperLeft() const { return BoardIndex(column - 1, row + 1); }
	BoardIndex upperRight() const { return BoardIndex(column + 1, row - 1); }
	BoardIndex bottomLeft() const { return BoardIndex(column - 1, row - 1); }
	BoardIndex bottomRight() const {return BoardIndex(column + 1, row - 1);}
	inline bool isInBoard() const { return (column >= 'a' && column <= 'h') && (row >= 1 && row <= 8);}
};
inline bool isInBoard(const BoardIndex& to_check) { return to_check.isInBoard(); }

ostream& operator<<(ostream& os, const BoardIndex& to_show);

const_pieces_iterator checkForPieces(const BoardIndex& position, const vector_pieces& pieces);

bool operator==(const BoardIndex& a, const BoardIndex& b) { return (a.column == b.column) && (a.row == b.row); }

class CheckersPiece
{
private:
	BoardIndex position_;
	int color_;
	bool is_king_;
public:
	enum { BLACK, WHITE };
	CheckersPiece() :position_(0, 0), color_(0), is_king_(false) {}
	bool canBeat(const vector<CheckersPiece>& opponent_pieces);
	BoardIndex getPosition() const { return position_; }
	int getColor() const { return color_; }
	bool getIsKing() const { return is_king_; }
};