// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Game.h"

int main()
{
	// create the window
	//sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

	//// run the program as long as the window is open
	//while (window.isOpen())
	//{
	//	// check all the window's events that were triggered since the last iteration of the loop
	//	sf::Event event;
	//	while (window.pollEvent(event))
	//	{
	//		// "close requested" event: we close the window
	//		if (event.type == sf::Event::Closed)
	//			window.close();
	//	}

	//	// clear the window with black color
	//	window.clear(sf::Color::Black);

	//	// draw everything here...
	//	// window.draw(...);

	//	// end the current frame
	//	window.display();
	//}
	Game checkers;
	checkers.Run();
	/*checkers.playersInit();
	vector_pieces check_array = checkers.getPlayer();
	BoardIndex pos = { 'a',4 };
	pieces_iterator iter = pos.checkForPieces(check_array);
	std::cout << (iter != check_array.end()) << endl;
	cout << pos.checkForPiecesBool(check_array) << endl;*/
	return 0;
}
