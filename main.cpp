// #include "main.h"
#include "game.h"
#include <boost/program_options.hpp>
#include <chrono>
#include "config.h"
#include "gamut_parser.h"


using namespace boost::program_options;

int main(int argc, char** argv)
{
	// configuration variables
	int set_rounds = NUM_OF_ROUNDS; 
	int set_print_top = PRINT_TOP;
	int set_print_last = PRINT_LAST;

	// command line parser
	namespace po = boost::program_options;
	options_description desc("Allowed options");
	desc.add_options()
	("help,h", "print usage message")
	("rounds,r", po::value<int>()->required(), "the number of rounds in a game")
	("print_top,t", po::value<int>()->required(), "print top n rounds info")
	("print_last,l", po::value<int>()->required(), "print last n rounds info")
	;
	variables_map vm;


	// parsing arguments
	cout << "---Start to parse commandline arguments---" << endl;
	try
 	{
		store(parse_command_line(argc, argv, desc), vm);

		if (vm.count("help")) {  
			std::cout << desc << "\n";
				return 0;
		}
		if (vm.count("rounds"))
		  set_rounds = vm["rounds"].as<int>();
		if (vm.count("print_top"))
		  set_print_top = vm["print_top"].as<int>();
		if (vm.count("print_last"))
		  set_print_last = vm["print_last"].as<int>();
	}
	catch(exception& e) {
		std::cerr << e.what() << "\n";
	}
	cout << endl;

	// generate a new game from Gamut
	cout << "---Generate a game from Gamut ---" << endl;
	std::string fname = "RandNew1";
	process_Mgr.generateGame(fname, 5, 2); // action size, players
	GameParser g;
	bool r = g.parser(fname + ".game");
	cout << endl;

	// start a game
	char a{0};
	cout << "---Game start---" << endl;
	Game testgame(set_rounds, NUM_OF_PLAYERS, set_print_top, set_print_last);
	// wait to see basic information
	while(a != 'y'){cout << "Please enter y to continue the game" << endl; cin >> a;}
	// loop m rounds games
	for(int i=0;i<testgame.m_rounds;i++)
	{
	  testgame.single_step();	
	}

	//statistic analysis
	cout << "---Save data to the CSV file ---" << endl;
	testgame.dataToFile();
}
