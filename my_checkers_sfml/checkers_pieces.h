#pragma once
#include <iostream>
#include <iterator>
#include <vector>
#include <list>
#include <array>
#include <utility>
#include <SFML/Graphics.hpp>
using std::vector;
using std::ostream;
using std::istream;
using std::pair;
using std::endl;
using std::cout;
using std::list;

class CheckersPiece;
class CheckersPieceWithState;
struct BoardIndex;
struct Board;

// used to store pieces of each player
// list is used because we don't need random access, but interested in constant time element erasa
//typedef std::array<CheckersPieceWithState,12> list_pieces; 

int sizeOfPieces(const CheckersPieceWithState* pieces);
//typedef list_pieces::iterator pieces_iterator;
//typedef list_pieces::const_iterator const_pieces_iterator;
typedef pair<BoardIndex, int> move_with_piece;

//this struct represents index of our checkers board_
struct BoardIndex
{
	char	column;	//from 'a' to 'h'
	int		row;	//from 1 to 8	
	BoardIndex() :column(0), row(0) {}
	explicit BoardIndex(char col, int r) :column(col), row(r) {}
	BoardIndex upperLeft() const { return BoardIndex(column - 1, row + 1); }
	BoardIndex upperRight() const { return BoardIndex(column + 1, row + 1); }
	BoardIndex bottomLeft() const { return BoardIndex(column - 1, row - 1); }
	BoardIndex bottomRight() const {return BoardIndex(column + 1, row - 1);}

	//checks whether position is in board_
	inline bool isInBoard() const { return (column >= 'a' && column <= 'h') && (row >= 1 && row <= 8);}

	//return itetaror to piece (in pieces) on position, if there is no piece, return pieces.end()
	int checkForPieces(const CheckersPieceWithState* pieces) const; 

	//return color of piece on position, if position is empty - return -1
	int checkForPieces(const Board& board) const; 

	//retun if there is piece on position:
	bool checkForPiecesBool(const CheckersPieceWithState* pieces) const { return checkForPieces(pieces) != -1;  }
	bool checkForPiecesBool(const Board& board) const;
};
ostream& operator<<(ostream& os, const BoardIndex& to_show);
istream& operator>>(istream& is, BoardIndex& to_input);
inline bool operator==(const BoardIndex& a, const BoardIndex& b) { return (a.column == b.column) && (a.row == b.row); }
inline bool operator!=(const BoardIndex& a, const BoardIndex& b) { return !(a == b); }

//simply names of our checkers, which are used when we represent board_ as massive 8x8
//so it used only for board_ struct and everything conected with it
enum CheckersType:int
{
	EMPTY = 0,
	WHITE_PIECE,
	BLACK_PIECE,
	WHITE_KING,
	BLACK_KING
};
enum GameState:int
{ 
	NOT_RUNNING,
	PAUSED,
	RUNNING,
	DRAW,
	WHITE_WINS, 
	BLACK_WINS 
};
inline BoardIndex intoBoardCoordinates(int row, int col) { return BoardIndex(col + 'a', 8 - row); }
struct Board
{
	
	int checkers_map[8][8] = {};

	//get board_ piece (as represented in CheckersType) for given BoardIndex
	int getPiece(const BoardIndex& position) const { return checkers_map[8 - position.row][position.column - 'a'];}
	int& getPiece(const BoardIndex& position) { return checkers_map[8 - position.row][position.column - 'a']; }
	void emptyCell(const BoardIndex& position) { this->getPiece(position) = CheckersType::EMPTY; }
	void movePiece(const BoardIndex& start_position, const BoardIndex& end_position);
	bool makeKing(const BoardIndex& position);
	bool makePiece(const BoardIndex& position);
	void clear();
};

// this class describes CheckersPiece, that have its own posion on board_, color (white or black)
// and can be king
class CheckersPiece
{
private:
	BoardIndex position_;
	int color_; //color takes values from enum in the public section
	bool is_king_;

	// add posible beat position for CheckersPiece in one given direction (pf pointer)
	// directions are declared in BoardIndex and are upperLeft, upperRight, bottomLeft, bottomRight
	// for this function we need only position of player pieces and opponent pieces,
	// but board_ is used for improving performance: checkForPieces without board_ take at maximum around 12 operations
	// with board_ - only 1
	// resulting moves are stored in possible_moves and in which we store both position of move and piece which we beat on this move
	// this information we be used to delete pieces when making moves

	void addPossibleMoveBeat(
		BoardIndex(BoardIndex::*pf)() const,
		const CheckersPieceWithState* player_pieces, 
		const CheckersPieceWithState* opponent_pieces, const Board& board,
		vector<move_with_piece>& possible_moves) const;

	//the same, but generate possible moves
	//resulting moves are stored in possible_moves which contains only moves 

	void addPossibleMove(BoardIndex(BoardIndex::*pf)() const,
		const CheckersPieceWithState* player_pieces, 
		const CheckersPieceWithState* opponent_pieces,const Board& board,
		vector<BoardIndex>& possible_moves) const;
public:
	enum { BLACK, WHITE };
	explicit CheckersPiece(const BoardIndex& pos = BoardIndex(0,0),int color = WHITE,bool is_king = false) :position_(pos), color_(color), is_king_(is_king) {}

	// generate possible moves and possible beat moves for CheckerPiece
	vector<move_with_piece> possibleBeatMoves(
		const CheckersPieceWithState* player_pieces, 
		const CheckersPieceWithState* opponent_pieces, 
		const Board& board) const;
	vector<BoardIndex> possibleMoves(
		const CheckersPieceWithState* player_pieces, 
		const CheckersPieceWithState* opponent_pieces, 
		const Board& board) const;

	// checks whether this piece is on right row to became a king
	// return true if can

	bool transformIntoKingIfPossible();

	//setters and getters
	BoardIndex getPosition() const { return position_; }
	void fillInfoByType(int type);
	void setPosition(const BoardIndex& board_index) { position_ = board_index;}
	void setColor(int color) { color_ = color; }
	int getColor() const { return color_; }
	bool isKing() const { return is_king_; }
	void makeKing() { is_king_ = true; }
	void makePiece() { is_king_ = false; }
	int getCheckersType() const;

	//some useful operations
	friend bool operator==(const CheckersPiece& a, const CheckersPiece& b)
	{
		return a.position_ == b.position_ && a.color_ == b.color_ && a.is_king_ == b.is_king_;
	}
	friend bool operator!=(const CheckersPiece& a, const CheckersPiece& b)
	{
		return !(a == b);
	}
	friend ostream& operator<<(ostream& os, const CheckersPiece& piece_to_show);
	friend istream& operator>>(istream& is, CheckersPiece& piece_to_input);
};

class CheckersPieceWithState:public CheckersPiece
{
public:
	bool not_beaten;
	CheckersPieceWithState(const CheckersPiece& ch_piece = CheckersPiece(), bool n_beaten = true):CheckersPiece(ch_piece), not_beaten(n_beaten) {}
};

struct move
{
	BoardIndex start_position;
	BoardIndex end_position;
	//static const int NOT_BEAT_MOVE = -1;
	int iter_piece_to_move, iter_piece_to_beat;
	bool became_king;
	void showBeatPartOfMove() const;
	move(const BoardIndex& st_pos = BoardIndex(),
		const BoardIndex& end_pos = BoardIndex(),
		int iter_move_piece = -1,
		int iter_beaten_piece = -1,
		bool king = false) : start_position(st_pos),
		end_position(end_pos),
		iter_piece_to_move(iter_move_piece),
		iter_piece_to_beat(iter_beaten_piece),
		became_king(king)
	{}
};
ostream& operator<<(ostream& os, const move& move_to_show);

struct moveWithPlayer :public move
{
	enum { WHITE_PLAYER, BLACK_PLAYER };
	int player;
	
	moveWithPlayer(int pl, const BoardIndex& st_pos = BoardIndex(),
		const BoardIndex& end_pos = BoardIndex(),
		int iter_move_piece = -1,
		int iter_beaten_piece = -1,
		bool king = false): move(st_pos, end_pos, iter_move_piece, iter_beaten_piece, king), player(pl){ }
	moveWithPlayer(int pl, const move& mv) :player(pl), move(mv) {}
};

void unmakeMove(
	moveWithPlayer& move_to_unmake,
	CheckersPieceWithState* white_player, CheckersPieceWithState* black_player, 
	Board& board);