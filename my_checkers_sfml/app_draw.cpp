// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "app_draw.h"
#include "Game.h"
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

void DrawAppInstance::drawPlayersPieces(const CheckersPieceWithState * player, int game_state, int turn)
{
	sf::Sprite piece;
	const sf::Texture *piece_texture, *piece_king_texture;
	int color = player[0].getColor();
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
	for (int i = 0; i<12;i++)
	{
		if (player[i].not_beaten && !(checkers_game_->cur_player_==player && i == piece_to_animate))
		{
			if (player[i].isKing())
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
			if (checkers_game_->game_mode_ == checkers_game_->POSITION_EDITOR)
				piece.setColor(sf::Color(255, 255, 255, 255));
			else if (game_state == GameState::NOT_RUNNING)
				piece.setColor(sf::Color(255, 255, 255, 64));
			else if (game_state == GameState::RUNNING && color != turn)
				piece.setColor(sf::Color(255, 255, 255, 150));

			sf::Vector2f piece_position = getRealPosition(player[i].getPosition());
			sf::FloatRect piece_bounds = piece.getGlobalBounds();
			piece.setPosition(piece_position);
			piece.move(sf::Vector2f(
				(cell_size_ - piece_bounds.width) / 2,
				(cell_size_ - piece_bounds.height) / 2));
			window_->draw(piece);
		}
	}
}


DrawAppInstance::DrawAppInstance(Game &game)
{
	checkers_game_ = &game;
	window_ = &game.window_;
	text_font_.loadFromFile("resources/fonts/PT_Serif-Web-Regular.ttf");

	white_piece_texture_.loadFromFile("resources/images/white_man.png");
	white_king_texture_.loadFromFile("resources/images/white_queen.png");

	black_piece_texture_.loadFromFile("resources/images/black_man.png");
	black_king_texture_.loadFromFile("resources/images/black_queen.png");

	white_piece_texture_.setSmooth(true);
	white_king_texture_.setSmooth(true);

	black_piece_texture_.setSmooth(true);
	black_king_texture_.setSmooth(true);
}

sf::Vector2f DrawAppInstance::getRealPosition(const BoardIndex & position) const
{
	return sf::Vector2f(
		(position.column - 'a')*cell_size_ + kLeftMargin,
		(8 - position.row)*cell_size_ + kTopMargin);
}




BoardIndex DrawAppInstance::clickPositionInBoard(int x, int y)
{
	sf::Vector2f real_coordinates = window_->mapPixelToCoords(sf::Vector2i(x, y));
	if (real_coordinates.y - kTopMargin < 0 || real_coordinates.x - kLeftMargin < 0)
		return BoardIndex('a' - 1, -1);
	float row = (real_coordinates.y - kTopMargin) / cell_size_, column = (real_coordinates.x - kLeftMargin) / cell_size_;
	return BoardIndex(int(column) + 'a', 8 - int(row));
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

void DrawAppInstance::animate(const BoardIndex & start, const BoardIndex & end, int checkers_type)
{
	auto real_start = getRealPosition(start), real_end = getRealPosition(end);
	auto diff = real_end - real_start;
	sf::Sprite animate;
	piece_to_animate = start.checkForPieces(checkers_game_->cur_player_);
	switch (checkers_type)
	{
		case CheckersType::BLACK_KING:
		{
			animate.setTexture(black_king_texture_);
			break;
		}
		case CheckersType::BLACK_PIECE:
		{
			animate.setTexture(black_piece_texture_);
			break;
		}
		case CheckersType::WHITE_KING:
		{
			animate.setTexture(white_king_texture_);
			break;
		}
		case CheckersType::WHITE_PIECE:
		{
			animate.setTexture(white_piece_texture_);
			break;
		}
	}
	sf::Time timer;
	sf::Clock clock;
	animate.setScale(cell_size_ / white_king_texture_.getSize().x, cell_size_ / white_king_texture_.getSize().y);
	animate.setPosition(getRealPosition(start));
	const int kNumberOfIterations = 40;
	for (int i = 0; i < kNumberOfIterations; i++)
	{
		clock.restart();
		timer = clock.getElapsedTime();
		window_->clear(sf::Color(255, 228, 170, 255));
		//checkers_game_->redrawPosition();
		drawStaticElements();
		drawPieces();
		checkers_game_->window_for_widgets_.draw();
		//drawPieces();
		animate.move(diff.x / kNumberOfIterations, diff.y / kNumberOfIterations);
		while ((clock.getElapsedTime()-timer) < sf::milliseconds(10));
		window_->draw(animate);
		window_->display();
	}
	piece_to_animate = -1;
}

void DrawAppInstance::highlightCells(const vector<BoardIndex>& cells_to_highlight, sf::Color color, float thickness)
{
	
	sf::RectangleShape cell(sf::Vector2f(cell_size_ - thickness * 2, cell_size_ - thickness * 2));
	for (auto x : cells_to_highlight)
	{

		sf::Vector2f cell_position = getRealPosition(x);
		cell.setPosition(sf::Vector2f(cell_position.x + thickness, cell_position.y + thickness));
		cell.setFillColor(sf::Color(0, 0, 0, 0));
		cell.setOutlineColor(color);
		cell.setOutlineThickness(thickness);
		window_->draw(cell);
	}
}

void DrawAppInstance::drawSelectCheckers()
{
	float select_checkers_size = (board_size_ / 2 - 2 * kObjectMargin) / 2;
	sf::Sprite checker(white_piece_texture_);
	for (int i = 0; i < 4; i++)
	{
		sf::Texture checker_texture;
		switch (i)
		{
		case 0:
			checker_texture = black_piece_texture_;
			select_checkers_positions[i].first = CheckersType::BLACK_PIECE;
			break;
		case 1:
			checker_texture = black_king_texture_;
			select_checkers_positions[i].first = CheckersType::BLACK_KING;
			break;
		case 2:
			checker_texture = white_piece_texture_;
			select_checkers_positions[i].first = CheckersType::WHITE_PIECE;
			break;
		case 3:
			checker_texture = white_king_texture_;
			select_checkers_positions[i].first = CheckersType::WHITE_KING;
			break;
		default:
			break;
		}
		select_checkers_positions[i].second = sf::FloatRect(
			kLeftMargin + board_size_ + kObjectMargin,
			kTopMargin + i*(select_checkers_size + kObjectMargin),
			select_checkers_size,
			select_checkers_size);
		checker.setTexture(checker_texture, true);
		sf::Color new_color = checker.getColor();
		if (i == checkers_game_->active_checker_in_editor_)
			checker.setColor(sf::Color(255, 255, 255, 255));
		else
			checker.setColor(sf::Color(255, 255, 255, 80));
		checker.setScale(select_checkers_size / checker_texture.getSize().x, select_checkers_size / checker_texture.getSize().y);
		checker.setPosition(kLeftMargin + board_size_ + kObjectMargin, kTopMargin + i*(select_checkers_size + kObjectMargin));
		window_->draw(checker);
	}
}

void DrawAppInstance::drawStaticElements()
{
	drawBoard();
	float kThickness = 0.08*cell_size_;
	highlightCells(checkers_game_->last_moves_to_show_, sf::Color(118, 255, 3, 255),kThickness/1.5);
	highlightCells(checkers_game_->hightlighted_cells_, sf::Color(255, 193, 7, 255),kThickness);
	if (checkers_game_->game_mode_ == checkers_game_->CHECKERS_GAME)
	{
		drawTurn(checkers_game_->white_turn_);
		int white_size = sizeOfPieces(checkers_game_->white_player_),
			black_size = sizeOfPieces(checkers_game_->black_player_);
		drawBeatenPieces(white_size, black_size);
	}
	else
		drawSelectCheckers();
}

void DrawAppInstance::setWidgetsPosition(tgui::MenuBar::Ptr main_menu, tgui::Button::Ptr unmove_button, tgui::Button::Ptr move_button)
{

}

void DrawAppInstance::drawPieces()
{
	int cur_color = (checkers_game_->white_turn_ ? CheckersPiece::WHITE : CheckersPiece::BLACK);
	drawPlayersPieces(checkers_game_->white_player_, checkers_game_->game_state_, cur_color);
	drawPlayersPieces(checkers_game_->black_player_, checkers_game_->game_state_, cur_color);
}

void DrawAppInstance::drawWinState()
{
	
	sf::Text win_text("", text_font_, 100);
	win_text.setPosition(sf::Vector2f(
		kLeftMargin + (board_size_ / 2 - win_text.getGlobalBounds().width),
		kTopMargin + (board_size_ / 2 - win_text.getGlobalBounds().height)));
	switch (checkers_game_->game_state_)
	{
	case static_cast<int>(GameState::WHITE_WINS):
		win_text.setString("White wins!");
		win_text.setOutlineThickness(1.0);
		win_text.setOutlineColor(sf::Color::Black);
		win_text.setPosition(sf::Vector2f(
			kLeftMargin + (board_size_ - win_text.getGlobalBounds().width) / 2,
			kTopMargin + (board_size_ - win_text.getGlobalBounds().height) / 2));
		window_->draw(win_text);
		break;
	case static_cast<int>(GameState::BLACK_WINS) :
		win_text.setFillColor(sf::Color::Black);
		win_text.setString("Black wins!");
		win_text.setPosition(sf::Vector2f(
			kLeftMargin + (board_size_ - win_text.getGlobalBounds().width) / 2,
			kTopMargin + (board_size_ - win_text.getGlobalBounds().height) / 2));
		window_->draw(win_text);
		break;
	}

}



void DrawAppInstance::setSizesAccordingToScreenResolution()
{
	sf::VideoMode video_mode;

	// get display resolution
	video_mode = video_mode.getDesktopMode();
	display_height_ = video_mode.height;
	display_width_ = video_mode.width;

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


	aspect_ratio_ = float(window_->getSize().x) / window_->getSize().y;
	//set window in the middle of a screen
	window_->setPosition(sf::Vector2i((video_mode.width - window_->getSize().x) / 2, 0));
}
