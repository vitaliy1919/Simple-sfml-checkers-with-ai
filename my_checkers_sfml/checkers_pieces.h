#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include <SFML/Graphics.hpp>
using std::vector;
using std::ostream;
using std::pair;
using std::endl;
using std::cout;

class CheckersPiece;
struct BoardIndex;
typedef vector<CheckersPiece> vector_pieces;
typedef vector_pieces::iterator pieces_iterator;
typedef vector_pieces::const_iterator const_pieces_iterator;
typedef pair<BoardIndex, pieces_iterator> move_with_piece;

struct BoardIndex
{
	char	column;
	int		row;
	BoardIndex(char col = 0, int r = 0) :column(col), row(r) {}
	BoardIndex upperLeft() const { return BoardIndex(column - 1, row + 1); }
	BoardIndex upperRight() const { return BoardIndex(column + 1, row + 1); }
	BoardIndex bottomLeft() const { return BoardIndex(column - 1, row - 1); }
	BoardIndex bottomRight() const {return BoardIndex(column + 1, row - 1);}
	inline bool isInBoard() const { return (column >= 'a' && column <= 'h') && (row >= 1 && row <= 8);}
	pieces_iterator checkForPieces(vector_pieces& pieces) const;
	bool checkForPiecesBool(vector_pieces& pieces) const { return checkForPieces(pieces) != pieces.end();  }
	//void checkForBeatingAndAddToPossibleMoves(const vector_pieces& pieces, vector<move_with_piece> possible_moves) const;
};
inline bool isInBoard(const BoardIndex& to_check) { return to_check.isInBoard(); }

ostream& operator<<(ostream& os, const BoardIndex& to_show);

pieces_iterator checkForPieces(const BoardIndex& position, vector_pieces& pieces);

inline bool operator==(const BoardIndex& a, const BoardIndex& b) { return (a.column == b.column) && (a.row == b.row); }
inline bool operator!=(const BoardIndex& a, const BoardIndex& b) { return !(a == b); }
class CheckersPiece
{
private:
	BoardIndex position_;
	int color_;
	bool is_king_;
public:
	enum { BLACK, WHITE };
	CheckersPiece() :position_(0, 0), color_(0), is_king_(false) {}
	vector<move_with_piece> possibleBeatMoves(vector_pieces& player_pieces, vector_pieces& opponent_pieces);
	vector<BoardIndex> possibleMoves(vector_pieces& player_pieces,  vector_pieces& opponent_pieces) const;
	BoardIndex getPosition() const { return position_; }
	void setPosition(const BoardIndex& board_index) { position_ = board_index;}
	void setColor(int color) { color_ = color; }
	int getColor() const { return color_; }
	bool isKing() const { return is_king_; }
	void makeKing() { is_king_ = true; }
	void makePiece() { is_king_ = false; }
	friend bool operator==(const CheckersPiece& a, const CheckersPiece& b)
	{
		return a.position_ == b.position_ && a.color_ == b.color_ && a.is_king_ == b.is_king_;
	}
	friend bool operator!=(const CheckersPiece& a, const CheckersPiece& b)
	{
		return !(a == b);
	}

};