#pragma once
#include "checkers_pieces.h"
#include "ai.h"
#include <fstream>
#include <algorithm>
#include <sstream>
// useful operation on vectors and lists
template <typename T>
inline void appendVector(vector<T>& dest, const vector<T>& source)
{
	dest.insert(dest.end(), source.begin(), source.end());
}
inline void appendVector(vector<BoardIndex>& dest, const list_pieces& source)
{
	for (auto x : source)
		dest.push_back(x.getPosition());
}
template <typename T>
inline typename vector<T>::iterator findInVector(vector<T>& source, T key)
{
	return std::find(source.begin(), source.end(), key);
}
inline typename vector<move_with_piece>::iterator findInVector(vector<move_with_piece>& source, BoardIndex key)
{
	vector<move_with_piece>::iterator iter = source.begin();
	while (iter != source.end() && iter->first != key)
		++iter;
	return iter;
}
// this class represents our checkers
// it has metod run, which is all you need to create game instance
class Game
{
private:
	//margins of out board_
	double kTextMargin = 30;
	double kTopMargin = kTextMargin;
	double kLeftMargin = kTextMargin;
	double kRightMargin = kTextMargin;
	double kBottomMargin = kTextMargin;
	double kTurnRectangleWidth = 30;
	double kTurnRectangleMargin = 10;
	list_pieces white_player_;
	list_pieces black_player_;
	Board board_;
	bool white_turn_;
	list_pieces *cur_player_;
	list_pieces *another_player_;

	bool is_white_ai_;
	bool is_black_ai_;
	Ai black_ai_;
	Ai white_ai_;
	
	// hightlighted_cells_ are cells which be highlighted with yellow on board_
	// possible_beat_moves_ and possible_moves_ store possible moves for clicked piece
	vector<BoardIndex> hightlighted_cells_;
	vector<move_with_piece> possible_beat_moves_;
	vector<BoardIndex> possible_moves_;

	//last moves to show on screen
	vector<BoardIndex> last_moves_to_show;

	//last moves of player 
	vector<BoardIndex> last_moves_of_cur_player_;

	// on each move we check whether at least one piece of current player can beat 
	// (because if so player must beat)
	// and store result of our check in must_beat_
	// pieces_that_can_beat_ contains all pieces which can beat
	bool must_beat_;
	bool can_beat_many_times_;
	list_pieces pieces_that_can_beat_;

	// if we click on piece we store this in is_piece_clicked_
	// and also store pointer to the element in piece_firstly_clicked_

	bool is_piece_clicked_;
	CheckersPiece *piece_firstly_clicked_;

	float cell_size_;
	float board_size_;
	sf::RenderWindow window;
	
	// this function return real position on board_ by BoardIndex 
	sf::Vector2f getRealPosition(const BoardIndex& position) const;

	// this function returns BoardIndex by real position
	BoardIndex clickPositionInBoard(int x, int y);

	enum {NOT_ENDED,WHITE_WINS,BLACK_WINS};
	int game_state_;

	// function used for drawing
	// drawPlayersPieces draws pieces for one of players
	// drawWinState draws big text with winner, when game ended
	void drawPlayersPieces(const list_pieces& player, const sf::Texture& piece_texture, const sf::Texture& piece_king_texture);
	void drawBoard();
	void drawPieces();
	void drawWinState();
	void redrawPosition();

	// set initial state of board_
	void playersInit();

	// check current player pieces whether at least one of them can beat
	// if so, fill must_beat_ and pieces_that_can_beat_
	void checkPiecesForBeating();

	// check all pieces of current player whether they can be transformed to kings
	// and do this if can
	void transformIntoKings();

	// change turn (also checks pieces for beating)
	void changeTurn();

	// clear posible moves, possible beat moves, make is_clicked_piece_ false
	// used when move was ended or when player unclick piece
	void clearInfoForClickedPiece();

	// process mouse click
	void processMouseClick(const BoardIndex& click_position);

	// build all possible position for given CheckersPiece
	// if piece can beat, possible_moves_ will be empty
	void buildPossibleMoves(CheckersPiece& clicked_piece);

	// move clicked piece to BoardIndex if possible 
	void moveClickedPiece(const BoardIndex& click_position);

	// checks for win and saves result to game_state_
	void checkForWin();

	void makeMove(const move& move_to_make);
	// checks wthether given player has at least on move
	// used in checkForWin
	bool checkPlayerHasMove(const list_pieces& player);

	// clears board_ state, doesn't work properly now
	// used for save and load game positions
	void clearAllStates();
public:
	Game();
	
	void Run();
	list_pieces getWhitePlayer() const { return white_player_; }
	list_pieces getBlackPlayer() const { return black_player_; }
	void setWhiteTurn();
	void setBlackTurn();

	// save to file (s button) current position
	// and load from file (r button) functions
	// used file "position.check", but you can change it
	// doesn't work correctly now
	void loadFromFile(std::string file_name);
	void saveToFile(std::string file_name);
};
