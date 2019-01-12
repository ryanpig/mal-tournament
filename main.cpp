// #include "main.h"
#include "game.h"
#include <boost/program_options.hpp>
using namespace boost::program_options;


int main(int argc, char** argv)
{
	// parser
    try {

				options_description desc("Allowed options");
				desc.add_options()
				("help,h", "print usage message")
				;
		
				variables_map vm;
				store(parse_command_line(argc, argv, desc), vm);

				if (vm.count("help")) {  
					std::cout << desc << "\n";
						return 0;
				}

    }
    catch(exception& e) {
			std::cerr << e.what() << "\n";
    }
	// TODO: arguments parser
	
	cout << "--Game start--" << endl;
	Game testgame(NUM_OF_TURNS, NUM_OF_PLAYERS);
	for(int i=0;i<testgame.m_turns;i++)
	{
	  testgame.single_step();	
	}

}
