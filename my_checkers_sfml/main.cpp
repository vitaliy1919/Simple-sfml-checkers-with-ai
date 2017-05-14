// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Game.h"

int main()
{
	Game checkers(Game::BLACK_AI, 7);
	checkers.Run();
	return 0;
}
