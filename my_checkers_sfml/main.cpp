// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Game.h"

int main()
{
	int depth = 9;
	//std::cin >> depth;
	Game checkers(Game::TWO_PLAYERS, depth);
	checkers.Run();
	return 0;
}
