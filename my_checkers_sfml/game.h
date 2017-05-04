#pragma once
#include "checkers_pieces.h"
template <typename T>
inline void appendVector(vector<T>& dest, const vector<T>& source)
{
	dest.insert(dest.end(), source.begin(), source.end());
}
inline void appendVector(vector<BoardIndex>& dest, const vector_pieces& source)
{
	for (auto x : source)
		dest.push_back(x.getPosition());
}

class Game
{
private:
	static const int kTextMargin = 20;
	vector_pieces white_player_;
	vector_pieces black_player_;
	
	bool white_turn_;
	vector_pieces *cur_player_;
	vector_pieces *another_player_;
	
	vector<BoardIndex> hightlighted_cells_;
	vector<move_with_piece> possible_beat_moves_;
	vector<BoardIndex> possible_moves_;

	bool must_beat_;
	vector_pieces pieces_that_can_beat_;

	bool piece_clicked_;
	CheckersPiece *piece_firstly_clicked_;

	float cell_size_;
	sf::RenderWindow window;
	
	sf::Vector2f getRealPosition(const BoardIndex& position) const;
	BoardIndex clickPositionInBoard(int x, int y);

	enum {NOT_ENDED,WHITE_WINS,BLACK_WINS};
	int game_state;
	void drawPlayersPieces(const vector_pieces player, sf::Texture piece_texture);
	void drawBoard();
	void drawPieces();

	void playersInit();

	void checkPiecesForBeating();
	void transformIntoKings();
	void changeTurn();

	void clearInfoForClickedPiece();
	void processMouseClick(const BoardIndex click_position);
	void buildPossibleMoves(CheckersPiece& clicked_piece);
	void movePiece(const BoardIndex& click_position);
	void checkForWin();
	bool checkPlayerHasMove(const vector_pieces& player);
	
public:
	Game() :window(sf::VideoMode(700, 700), "Checkers"), 
		white_player_(12), 
		black_player_(12), 
		white_turn_(true),
		cell_size_(87.5),
		hightlighted_cells_(),
		possible_beat_moves_(),
		possible_moves_(),
		piece_clicked_(false),
		piece_firstly_clicked_(),
		cur_player_(&white_player_),
		another_player_(&black_player_),
		pieces_that_can_beat_(),
		must_beat_(false),
		game_state(NOT_ENDED){}
	
	void Run();
	vector_pieces getWhitePlayer() const { return white_player_; }
	vector_pieces getBlackPlayer() const { return black_player_; }
};