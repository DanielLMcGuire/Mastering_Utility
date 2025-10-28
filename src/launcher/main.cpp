/**
 * @file main.cpp
 * @brief Mastering Utility Launcher
 * @author Daniel McGuire
 */
#include "../argumentParser.h"
#include "../MasteringUtil.h"
#include <filesystem>
#include <iostream>
#include <string>

/// @brief CRT Entry Point
int main( int argc, char* argv[] )
{
	std::cout << "Daniel's Mastering Utility -- Masterer\n";

	MasteringUtility masterer;

	ArgParser* parser = new ArgParser;
	parser->registerArg("help", ArgParser::ArgType::Bool, 'h');
	parser->registerArg("markupfile", ArgParser::ArgType::String, 'f');

	parser->parse(argc, argv);

	std::filesystem::path markupPath{ parser->f_string("markupfile") };
	if (markupPath.empty())
	{
		if (argc > 1 && std::filesystem::exists(argv[1]))
			markupPath = argv[1];
		else
			return 1;
	}
	delete parser;
	try
	{
		masterer.Master( markupPath );
		std::cout << "Mastering finished!\n";
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Error during mastering: " << ex.what() << "\n";
		return 1;
	}
	catch ( ... )
	{
		std::cerr << "Unknown error during mastering.\n";
		return -1;
	}

	return 0;
}
