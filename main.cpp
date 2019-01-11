// #include "main.h"
#include "game.h"

int main()
{
	// TODO: arguments parser
	
	cout << "--Game start--" << endl;
	Game testgame(NUM_OF_TURNS, NUM_OF_PLAYERS);
	for(int i=0;i<testgame.m_turns;i++)
	{
	  testgame.single_step();	
	}

}
