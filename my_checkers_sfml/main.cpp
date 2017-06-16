// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Game.h"
#include "app_draw.h"
#include "checkers_pieces.h"
int getNumber(int min, int max)
{
	int res;
	do
	{
		std::cin >> res;
		if (!std::cin)
		{
			std::cin.clear();
			while (std::cin.get() != '\n');
			res = max + 1;
		}
	} while (res < min && res > max);
	std::cin.get();
	return res;
}

int main()
{
	Game checkers;
	checkers.Run();
	
	return 0;
}
