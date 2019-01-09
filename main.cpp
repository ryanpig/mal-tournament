// #include "main.h"
#include "game.h"
#define NUM_OF_PLAYERS 2
#define NUM_OF_TURNS 5

int main()
{
	cout << "--Game start--" << endl;
	Game testgame(NUM_OF_TURNS, NUM_OF_PLAYERS);
	for(int i=0;i<testgame.m_turns;i++)
	{
		cout << "step " << i << ":" << endl;
	  testgame.single_step();	
	}

}
