#include "app_draw.h"
void DrawAppInstance::drawBoard()
{
	sf::RectangleShape cell;

	sf::Text board_coordinates("abcdefgh", text_font_, 25);
	board_coordinates.setFillColor(sf::Color::Black);
	board_coordinates.setPosition(0, 0);
	sf::FloatRect bounds_for_calculations = board_coordinates.getGlobalBounds();
	board_coordinates.setScale(sf::Vector2f(kTopTextSize / ( bounds_for_calculations.height), kTopTextSize / ( bounds_for_calculations.height)));
	bounds_for_calculations = board_coordinates.getGlobalBounds();

	float text_top_position = -bounds_for_calculations.top + kMenuSize + kObjectMargin;
	cell.setSize(sf::Vector2f(window_->getSize().x, kMenuSize));
	cell.setPosition(sf::Vector2f(0, 0));
	cell.setFillColor(sf::Color::Black);
	window_->draw(cell);
	window_->getView();
	cell.setSize(sf::Vector2f(cell_size_, cell_size_));
	for (int i = 0; i < 8; i++) //i - row
		for (int j = 0; j < 8; j++) //j - column
		{
			cell.setPosition(getRealPosition(BoardIndex('a' + j, 8 - i)));
			sf::Color cell_color;
			std::string coord;
			if ((i + j) % 2 != 0)
				cell_color = sf::Color(209, 139, 71, 255);
			//cell_color = sf::Color::Blue;
			else
				cell_color = sf::Color(255, 228, 170, 255);
			//cell_color = sf::Color::Yellow;
			cell.setFillColor(cell_color);
			window_->draw(cell);
			if (j == 0)
			{
				coord += std::to_string(8 - i);
				board_coordinates.setString(coord);
				board_coordinates.setPosition((kLeftMargin - board_coordinates.getGlobalBounds().width) / 2,
					cell_size_*(i + 0.5) - board_coordinates.getCharacterSize() / 2 + kTopMargin);
				window_->draw(board_coordinates);
			}
			if (i == 0)
			{
				coord = 'a' + j;
				board_coordinates.setString(coord);
				board_coordinates.setPosition(sf::Vector2f(0, 0));
				board_coordinates.setPosition(sf::Vector2f(cell_size_*(j + 0.5) - board_coordinates.getGlobalBounds().width / 2 + kLeftMargin, text_top_position));
				window_->draw(board_coordinates);
			}
		}
	cell.setSize(sf::Vector2f(board_size_, board_size_));
	cell.setPosition(getRealPosition(BoardIndex('a', 8)));
	cell.setFillColor(sf::Color(0, 0, 0, 0));
	cell.setOutlineColor(sf::Color::Black);
	cell.setOutlineThickness(1.0);
	window_->draw(cell);

}

void DrawAppInstance::drawTurn( bool white_turn)
{
	sf::RectangleShape turn_rectangle(sf::Vector2f(kTurnRectangleWidth, board_size_ / 2));
	turn_rectangle.setOutlineThickness(1.0);
	turn_rectangle.setOutlineColor(sf::Color::Black);
	if (white_turn)
	{
		turn_rectangle.setFillColor(sf::Color::White);
		turn_rectangle.setPosition(sf::Vector2f(kLeftMargin + board_size_ + kObjectMargin, kTopMargin + board_size_ / 2));
	}
	else
	{
		turn_rectangle.setFillColor(sf::Color::Black);
		turn_rectangle.setPosition(sf::Vector2f(kLeftMargin + board_size_ + kObjectMargin, kTopMargin));
	}
	window_->draw(turn_rectangle);
}

void DrawAppInstance::drawPlayersPieces(const list_pieces & player)
{
	sf::Sprite piece;
	const sf::Texture *piece_texture, *piece_king_texture;
	if (player.empty())
		return;
	int color = player.front().getColor();
	if (color == CheckersPiece::WHITE)
	{
		piece_texture = &white_piece_texture_;
		piece_king_texture = &white_king_texture_;
	}
	else
	{
		piece_texture = &black_piece_texture_;
		piece_king_texture = &black_king_texture_;
	}
	for (auto players_piece : player)
	{
		if (players_piece.isKing())
		{
			piece.setTexture(*piece_king_texture, true);
			piece.setScale(sf::Vector2f(
				cell_size_ / (1.2*piece_king_texture->getSize().x),
				cell_size_ / (1.2* piece_king_texture->getSize().y)));

		}
		else
		{
			piece.setTexture(*piece_texture, true);
			piece.setScale(sf::Vector2f(
				cell_size_ / (1.2*piece_texture->getSize().x),
				cell_size_ / (1.2* piece_texture->getSize().y)));
		}
		piece.setColor(sf::Color::White);
		sf::Vector2f piece_position = getRealPosition(players_piece.getPosition());
		sf::FloatRect piece_bounds = piece.getGlobalBounds();
		piece.setPosition(piece_position);
		piece.move(sf::Vector2f(
			(cell_size_ - piece_bounds.width) / 2,
			(cell_size_ - piece_bounds.height) / 2));
		window_->draw(piece);
	}
}


DrawAppInstance::DrawAppInstance(sf::RenderWindow &wind):window_(&wind)
{
	text_font_.loadFromFile("PT_Serif-Web-Regular.ttf");

	white_piece_texture_.loadFromFile("white_man.png");
	white_king_texture_.loadFromFile("white_queen.png");

	black_piece_texture_.loadFromFile("black_man.png");
	black_king_texture_.loadFromFile("black_queen.png");

	white_piece_texture_.setSmooth(true);
	white_king_texture_.setSmooth(true);

	black_piece_texture_.setSmooth(true);
	black_king_texture_.setSmooth(true);
	setSizesAccordingToScreenResolution();

}

sf::Vector2f DrawAppInstance::getRealPosition(const BoardIndex & position) const
{
	return sf::Vector2f(
		(position.column - 'a')*cell_size_ + kLeftMargin,
		(8 - position.row)*cell_size_ + kTopMargin);
}

void DrawAppInstance::setWindow(
	sf::RenderWindow& wind)
{
	window_ = &wind;
}



BoardIndex DrawAppInstance::clickPositionInBoard(int x, int y)
{
	sf::Vector2f real_coordinates = window_->mapPixelToCoords(sf::Vector2i(x, y));
	float row = (real_coordinates.y - kTopMargin) / cell_size_, column = (real_coordinates.x - kLeftMargin) / cell_size_;
	return BoardIndex(int(column) + 'a', 8 - int(row));
}


void DrawAppInstance::setMenuPosition(tgui::MenuBar::Ptr main_menu)
{

}

void DrawAppInstance::drawBeatenPieces(int white_player_size, int black_player_size)
{
	float left_beaten_position =
		kLeftMargin + board_size_ +
		2 * kObjectMargin + kTurnRectangleWidth;
	const int kNumberPiecesInARow = 3;
	float diametr_beaten_piece = kBeatenPiecesSectionSize/ kNumberPiecesInARow;
	sf::Text text("Beaten pieces",text_font_,25);
	text.setFillColor(sf::Color::Black);
	sf::FloatRect text_bound = text.getLocalBounds();
	text.setScale(sf::Vector2f(kTopTextSize / text_bound.height, kTopTextSize / text_bound.height));
	float text_top_position = -text_bound.top + kObjectMargin + kMenuSize;
	text.setPosition(sf::Vector2f(
		left_beaten_position + (kBeatenPiecesSectionSize - text.getGlobalBounds().width) / 2, text_top_position));
	window_->draw(text);

	sf::RectangleShape not_beaten_piece(sf::Vector2f(diametr_beaten_piece, diametr_beaten_piece));
	sf::Sprite beaten_piece_black(black_piece_texture_), beaten_piece_white(white_piece_texture_);

	// scale textures
	beaten_piece_black.setScale(
		diametr_beaten_piece / black_piece_texture_.getSize().x,
		diametr_beaten_piece / black_piece_texture_.getSize().y);
	beaten_piece_white.setScale(
		diametr_beaten_piece / white_piece_texture_.getSize().x,
		diametr_beaten_piece / white_piece_texture_.getSize().y);

	not_beaten_piece.setOutlineThickness(1.0);
	not_beaten_piece.setOutlineColor(sf::Color(0, 0, 0, 60));
	not_beaten_piece.setFillColor(sf::Color(0, 0, 0, 0));

	int number_beaten_pieces_black = 12 - black_player_size,
		number_beaten_pieces_white = 12 - white_player_size;

	for (int i = 0; i < 12; i++)
	{
		int row = i / kNumberPiecesInARow,
			column = i % kNumberPiecesInARow;

		if (i < number_beaten_pieces_black)
		{
			beaten_piece_black.setPosition(sf::Vector2f(
				left_beaten_position + column*diametr_beaten_piece,
				kTopMargin + row*diametr_beaten_piece));
			window_->draw(beaten_piece_black);
		}
		else
		{
			not_beaten_piece.setPosition(sf::Vector2f(
				left_beaten_position + column*diametr_beaten_piece,
				kTopMargin + row*diametr_beaten_piece));
			window_->draw(not_beaten_piece);
		}

		if (i < number_beaten_pieces_white)
		{
			beaten_piece_white.setPosition(sf::Vector2f(
				left_beaten_position + column*diametr_beaten_piece,
				kTopMargin + board_size_ / 2 + row*diametr_beaten_piece));
			window_->draw(beaten_piece_white);
		}
		else
		{
			not_beaten_piece.setPosition(sf::Vector2f(
				left_beaten_position + column*diametr_beaten_piece,
				kTopMargin + board_size_ / 2 + row*diametr_beaten_piece));
			window_->draw(not_beaten_piece);
		}
	}
}

void DrawAppInstance::highlightCells(const vector<BoardIndex>& cells_to_highlight, sf::Color color)
{
	float kThickness = 0.08*cell_size_;
	sf::RectangleShape cell(sf::Vector2f(cell_size_ - kThickness * 2, cell_size_ - kThickness * 2));
	for (auto x : cells_to_highlight)
	{

		sf::Vector2f cell_position = getRealPosition(x);
		cell.setPosition(sf::Vector2f(cell_position.x + kThickness, cell_position.y + kThickness));
		cell.setFillColor(sf::Color(0, 0, 0, 0));
		cell.setOutlineColor(color);
		cell.setOutlineThickness(kThickness);
		window_->draw(cell);
	}
}

void DrawAppInstance::drawStaticElements(bool is_white_move,
	const vector<BoardIndex>& higlighted, 
	const vector<BoardIndex>& last_moves,
	int white_size_,
	int black_size_)
{
	drawBoard();
	highlightCells(last_moves, sf::Color(118, 255, 3, 255));
	highlightCells(higlighted, sf::Color(255, 193, 7, 255));
	drawTurn(is_white_move);
	drawBeatenPieces(white_size_,black_size_);

}

void DrawAppInstance::setWidgetsPosition(tgui::MenuBar::Ptr main_menu, tgui::Button::Ptr unmove_button, tgui::Button::Ptr move_button)
{
	main_menu->setSize(tgui::Layout2d(kLeftMargin + board_size_ + kRightMargin, kMenuSize));
	unmove_button->setPosition(kLeftMargin, kTopMargin + board_size_ + kObjectMargin);
	move_button->setPosition(kLeftMargin + unmove_button->getSize().x+kObjectMargin, kTopMargin + board_size_ + kObjectMargin);
}

void DrawAppInstance::drawPieces(const list_pieces & white_player, const list_pieces & black_pieces)
{
	drawPlayersPieces(white_player);
	drawPlayersPieces(black_pieces);
}

void DrawAppInstance::setSizesAccordingToScreenResolution()
{
	sf::VideoMode video_mode;

	// get display resolution
	video_mode = video_mode.getDesktopMode();

	//and use it to make game playable on different screen resolution
	unsigned int window_size = 0.75*std::min(video_mode.height, video_mode.width);// window_size = 900 * 0.8;;
	board_size_ = window_size;
	cell_size_ = board_size_ / 8;

	kObjectMargin = 0.008*board_size_;

	kLeftTextSize = 0.032*board_size_;
	kLeftMargin = 2*kObjectMargin + kLeftTextSize;

	kTopTextSize = 0.032*board_size_;
	kMenuSize = 0.032*board_size_;
	kTopMargin = 2 * kObjectMargin + kTopTextSize + kMenuSize;
	
	kTurnRectangleWidth = 0.03*board_size_;
	kBeatenPiecesSectionSize = 0.34*board_size_;
	kRightMargin = 3 * kObjectMargin + kTurnRectangleWidth + kBeatenPiecesSectionSize;

	
	kUnmoveButtonSize = 0.05*board_size_;
	kBottomMargin = 2 * kObjectMargin + kUnmoveButtonSize;


	window_->create(sf::VideoMode(window_size + kLeftMargin + kRightMargin, window_size + kTopMargin + kBottomMargin), "Checkers");


	//set window in the middle of a screen
	window_->setPosition(sf::Vector2i((video_mode.width - window_->getSize().x) / 2, 0));
}
