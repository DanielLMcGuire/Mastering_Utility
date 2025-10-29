/**
 * @file main.cpp
 * @brief Mastering Utility Launcher
 * @author Daniel McGuire
 */

// Copyright 2025 Daniel McGuire
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "../dconsole.h"
#include "../MasteringUtil.h"
#include <filesystem>
#include <iostream>
#include <string>

/// @brief CRT Entry Point
int main( int argc, char* argv[] )
{
	std::cout << "Daniel's Mastering Utility -- Masterer\n";

	MasteringUtility masterer;

	DConsole conlib;
	conlib.registerArg("help", DConsole::ArgType::Bool, 'h');
	conlib.registerArg("markupfile", DConsole::ArgType::String, 'f');

	conlib.parse(argc, argv);

	std::filesystem::path markupPath{ conlib.f_string("markupfile") };
	if (markupPath.empty())
	{
		if (argc > 1 && std::filesystem::exists(argv[1]))
			markupPath = argv[1];
		else
			return 1;
	}
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
