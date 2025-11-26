/**
 * @file main.cpp
 * @brief Mastering Utility Launcher
 * @author Daniel McGuire
 */

// Copyright 2025 Daniel McGuire
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <MasteringUtil.h>
#include <dconsole.h>
#include <filesystem>
#include <iostream>
#include <string>

/// @brief CRT Entry Point
int main(int argc, char *argv[])
{
	std::cout << "Daniel's Mastering Utility -- Masterer\n";

	MasteringUtility masterer;

	DConsole conlib;
	conlib.supressUnknownArgument = true;
	conlib.registerFlag("help", DConsole::f::boolean, 'h');
	conlib.registerFlag("markupfile", DConsole::f::string, 'f');

	conlib.parse(argc, argv);

	std::filesystem::path markupPath{conlib.f_string("markupfile")};
	if (markupPath.empty())
	{
		if (argc > 1 && std::filesystem::exists(argv[1]))
			markupPath = argv[1];
		else
			return 1;
	}
	try
	{
		masterer.Master(markupPath);
		std::cout << "Mastering finished!\n";
	}
	catch (const std::exception &ex)
	{
		std::cerr << "Error during mastering: " << ex.what() << "\n";
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown error during mastering.\n";
		return -1;
	}
	return 0;
}
