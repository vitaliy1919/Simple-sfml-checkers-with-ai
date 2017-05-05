#pragma once
#include "checkers_pieces.h"
#include <fstream>
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
	static const int kTextMargin = 30;
	static const int kTopMargin = kTextMargin;
	static const int kLeftMargin = kTextMargin;
	static const int kRightMargin = kTextMargin;
	static const int kBottomMargin = kTextMargin;
	vector_pieces white_player_;
	vector_pieces black_player_;
	Board board_;
	bool white_turn_;
	vector_pieces *cur_player_;
	vector_pieces *another_player_;
	
	vector<BoardIndex> hightlighted_cells_;
	vector<move_with_piece> possible_beat_moves_;
	vector<BoardIndex> possible_moves_;

	bool must_beat_;
	vector_pieces pieces_that_can_beat_;

	bool is_piece_clicked_;
	CheckersPiece *piece_firstly_clicked_;

	float cell_size_;
	float board_size_;
	sf::RenderWindow window;
	
	sf::Vector2f getRealPosition(const BoardIndex& position) const;
	BoardIndex clickPositionInBoard(int x, int y);

	enum {NOT_ENDED,WHITE_WINS,BLACK_WINS};
	int game_state_;

	void drawPlayersPieces(const vector_pieces player, const sf::Texture& piece_texture, const sf::Texture& piece_king_texture);
	void drawBoard();
	void drawPieces();
	void drawWinState();

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
	void clearAllStates();
public:
	Game() :
		white_player_(12), 
		black_player_(12), 
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
		game_state_(NOT_ENDED)
	{
		sf::VideoMode video_mode;
		video_mode = video_mode.getDesktopMode();
		unsigned int window_size = 0.8*std::min(video_mode.height, video_mode.width);
		board_size_ = window_size;
		cell_size_ = board_size_ / 8;
		window.create(sf::VideoMode(window_size + kLeftMargin + kRightMargin, window_size + kTopMargin + kBottomMargin), "Checkers"/*,sf::Style::Close*/);
		window.setPosition(sf::Vector2i((video_mode.width - window.getSize().x) / 2, 0));
	}
	
	void Run();
	vector_pieces getWhitePlayer() const { return white_player_; }
	vector_pieces getBlackPlayer() const { return black_player_; }
	void setWhiteTurn();
	void setBlackTurn();
	void loadFromFile(std::string file_name);
	void saveToFile(std::string file_name);
};