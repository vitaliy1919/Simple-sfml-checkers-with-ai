#pragma once
#include "checkers_pieces.h"
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
	void processMouseClick(const BoardIndex click_position);

	// build all possible position for given CheckersPiece
	// if piece can beat, possible_moves_ will be empty
	void buildPossibleMoves(CheckersPiece& clicked_piece);

	// move clicked piece to BoardIndex if possible 
	void moveClickedPiece(const BoardIndex& click_position);

	// checks for win and saves result to game_state_
	void checkForWin();

	// checks wthether given player has at least on move
	// used in checkForWin
	bool checkPlayerHasMove(const list_pieces& player);

	// clears board_ state, doesn't work properly now
	// used for save and load game positions
	void clearAllStates();
public:
	Game() :
		white_player_(), 
		black_player_(), 
		white_turn_(true),
		board_size_(700),
		cell_size_(87.5),
		hightlighted_cells_(),
		possible_beat_moves_(),
		possible_moves_(),
		is_piece_clicked_(false),
		piece_firstly_clicked_(),
		cur_player_(&white_player_),
		another_player_(&black_player_),
		pieces_that_can_beat_(),
		must_beat_(false),
		game_state_(NOT_ENDED),
		can_beat_many_times_(false)
	{
		sf::VideoMode video_mode;

		// get display resolution
		video_mode = video_mode.getDesktopMode(); 

		//and use it to make game playable on different screen resolution
		unsigned int window_size = 0.8*std::min(video_mode.height, video_mode.width);// window_size = 900 * 0.8;;
		board_size_ = window_size;
		cell_size_ = board_size_ / 8;
		kTopMargin = kLeftMargin = kBottomMargin = 0.042*board_size_;
		kTurnRectangleWidth = 0.03*board_size_;
		kTurnRectangleMargin = 0.015 * board_size_;
		//kRightMargin = 2 * kTurnRectangleMargin + kTurnRectangleWidth;
		kRightMargin = 0.4 * board_size_;
		window.create(sf::VideoMode(window_size + kLeftMargin + kRightMargin, window_size + kTopMargin + kBottomMargin), "Checkers");
		//window.setView(sf::View(sf::FloatRect(0, 0, window_size + kLeftMargin + kRightMargin, window_size + kTopMargin + kBottomMargin)));

		//set window on the middle of a screen
		window.setPosition(sf::Vector2i((video_mode.width - window.getSize().x) / 2, 0));

		is_black_ai_;
	}
	
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
