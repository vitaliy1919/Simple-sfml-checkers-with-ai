// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Game.h"

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
	
	std::string ch;
	//std::cin >> depth;
	do
	{
		cout << "Choose mode of the game:"
			"\n1. Player vs Player"
			"\n2. Player(white) vs Ai(black)"
			"\n3. Ai(white) vs Player(black)"
			"\n4. Ai vs Ai\n";
		int inp = getNumber(1,4);
		int depth = 9;
		if (inp != 1)
		{
			cout << "Input level of Ai (from 1 to 9): ";
			depth = getNumber(1, 9);
		}
		int game_mode = Game::TWO_PLAYERS;
		switch (inp)
		{
		case 1:
			game_mode = Game::TWO_PLAYERS;
			break;
		case 2:
			game_mode = Game::BLACK_AI;
			break;
		case 3: 
			game_mode = Game::WHITE_AI;
			break;
		case 4:
			game_mode = Game::TWO_AI;
			break;
		}
		cout << "If you want to play new game just close window with game (not the console)\nPress <Enter> to continue\n";
		std::cin.get();
		Game checkers(game_mode, depth);
		checkers.Run();
		cout << "Do you want to play new game (y - yes, n - no)?\n";
		do
		{
			getline(std::cin, ch);
		} while (ch != "y" && ch != "n");
	}while(ch != "n");
	return 0;
}
