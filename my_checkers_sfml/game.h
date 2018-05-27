#pragma once
#include "checkers_pieces.h"
#include "gui.h"
#include "ai.h"
#include "icon.h"
#include <thread>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "app_draw.h"



// this class represents our checkers
// it has metod run, which is all you need to create game instance
class Game
{
private:
	friend class DrawAppInstance;
	friend class AppWidgets;
	CheckersPieceWithState white_player_[12];
	CheckersPieceWithState black_player_[12];
	Board board_;
	bool white_turn_ = true;
	CheckersPieceWithState *cur_player_;
	CheckersPieceWithState *another_player_;

	bool is_white_ai_;
	bool is_black_ai_;
	Ai ai_;
	
	// hightlighted_cells_ are cells which be highlighted with yellow on board_
	// possible_beat_moves_ and possible_moves_ store possible moves for clicked piece
	vector<BoardIndex> hightlighted_cells_;
	vector<move_with_piece> possible_beat_moves_;
	vector<BoardIndex> possible_moves_;

	//last moves to show on screen
	vector<BoardIndex> last_moves_to_show_;

	//last moves of player 
	vector<BoardIndex> last_moves_of_cur_player_;

	list<moveWithPlayer> all_moves_;
	// on each move we check whether at least one piece of current player can beat 
	// (because if so player must beat)
	// and store result of our check in must_beat_
	// pieces_that_can_beat_ contains all pieces which can beat
	bool must_beat_ = false;
	bool can_beat_many_times_ = false;
	list<BoardIndex> pieces_that_can_beat_;

	// if we click on piece we store this in is_piece_clicked_
	// and also store pointer to the element in piece_firstly_clicked_

	bool is_piece_clicked_ = false;
	int piece_firstly_clicked_ = -1;
	bool move_done_ = false;

	sf::RenderWindow window_;
	tgui::Gui window_for_widgets_;
	
	AppWidgets widgets_app_;

	int game_state_;
	bool game_ended_;
	int ai_depth_;
	bool ai_done_;
	bool ai_start_thinking_ = false;

	enum { CHECKERS_GAME, POSITION_EDITOR };
	int game_mode_ = CHECKERS_GAME;
	int active_checker_in_editor_ = -1;
	int checkers_type_in_editor_ = -1;
	int white_last_checker_ = 0;
	int black_last_checker_ = 0;

	// -1 - black ai, 0 - user move, 1 - mhite ai
	int isAiMove();
	sf::Time moveAi(list<move>& best_moves_for_ai);

	void applyAndShowMoves(const list<move>& moves_to_apply, sf::Time already_elapsed_time = sf::seconds(0), bool change_turn_after = true);
	// function used for drawing
	// drawPlayersPieces draws pieces for one of players
	// drawWinState draws big text with winner, when game ended

	DrawAppInstance draw_app_;
	void redrawPosition();
	void draw();
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


	void clickInEditor(const sf::Event& click_event);
	// build all possible position for given CheckersPiece
	// if piece can beat, possible_moves_ will be empty
	void buildPossibleMoves(int clicked_piece_iter);

	// move clicked piece to BoardIndex if possible 
	void moveClickedPiece(const BoardIndex& click_position);

	// checks for win and saves result to game_state_
	void checkForWin();

	void makeMove(const move& move_to_make);
	// checks wthether given player has at least on move
	// used in checkForWin
	bool checkPlayerHasMove(const CheckersPieceWithState* player);
	void showMoves();
	// clears board_ state, doesn't work properly now
	// used for save and load game positions
	void clearAllStates();
public:
	Game();
	void playersInit();

	void Run();
	const CheckersPieceWithState* getWhitePlayer() const { return white_player_; }
	const CheckersPieceWithState* getBlackPlayer() const { return black_player_; }
	void setWhiteTurn();
	void setBlackTurn();

	// save to file (s button) current position
	// and load from file (r button) functions
	// used file "position.check", but you can change it
	// doesn't work correctly now
	void loadFromFile(std::string file_name);
	void saveToFile(std::string file_name);
};
