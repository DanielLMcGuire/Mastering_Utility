/**
 * @file main.cpp
 * @brief Mastering Utility Wizard
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

#include "../argumentParser.h"
#include "../MasteringUtil.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable: 4834)
#endif

bool g_AutoAddSongs = false;

/** @brief Indicate unreachable code
 *
 * Uses compiler-specific intrinsics to inform the optimizer that this code path is unreachable.
 */
[[noreturn]] inline void unreachable()
{
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
	__assume(false);
#else // GCC, Clang
	__builtin_unreachable();
#endif
}

/** @brief Cleanup and exit
 *
 * @param code Exit code
 */
void cleanup(int code)
{
	if (code != 0) std::cout << "Exited with code " << code << '\n';
	std::cout << std::endl;
	std::exit(code);
	unreachable();
}

/// @brief Mastering Wizard namespace
namespace MasteringWizard 
{

	/// @brief Preview namespace
	namespace preview {
		 /** @brief Display a summary of a SONG
		  * @param song Song to summarize
		  * @param displayInfo Display Special Styling (default = true)
		  */
		void song(const MasteringUtility::Song& song, bool displayInfo = true)
		{
			if (displayInfo) std::cout << "\n=== Song Summary ===\n";
			std::cout << "Title: " << song.Title << " | Artist: " << song.Artist << " | Album: " << song.Album << " | Track: " << song.TrackNumber << "\n";
			if (displayInfo) std::cout << "-------------------------\n";
		}
		/** @brief Display a summary of an ALBUM
		 * @param album Album to summarize
		 */
		void album(const MasteringUtility::Album& album)
		{
			std::cout << "\n=== Album Summary ===\n";
			std::cout << "Album: " << album.Title << " | Artist: " << album.Artist << " | Genre: " << album.Genre
					  << " | Year: " << album.Year << " | Songs: " << album.SongsList.size() << "\n";
			for (const auto& song : album.SongsList)
			{
				preview::song(song, false);
			}
			std::cout << "-------------------------\n";
		}
	} // namespace preview

	/** @brief Prompt user for input with validation
	 *
	 * @tparam T Type of the input value
	 * @param message Prompt message
	 * @param[out] value Reference to store the input value
	 * @param errorMessage Error message to display on invalid input
	 * @param defaultValue Optional default value if user input is empty
	 */
	template<typename T>
	void prompt(const std::string& message, T& value, const std::string& errorMessage, const T* defaultValue = nullptr)
	{
		std::string input;
		while (true)
		{
			std::cout << message;
			if (defaultValue) std::cout << " [" << *defaultValue << "]";
			std::cout << ": ";

			if (!std::getline(std::cin, input))
			{
				std::cerr << "Input error. " << errorMessage << std::endl;
				continue;
			}

			if (input.empty())
			{
				if (defaultValue)
				{
					value = *defaultValue;
					break;
				}
				else
				{
					std::cerr << errorMessage << std::endl;
					continue;
				}
			}

			if (input == "exit" || input == "quit")
			{
				cleanup(0);
			}

			if constexpr (std::is_same_v<T, std::string>) 
			{
				value = input;
				break;
			}
			else if constexpr (std::is_same_v<T, char>) 
			{
				if (input.length() == 1)
				{
					value = input[0];
					break;
				}
				else
				{
					std::cerr << "Please enter a single character. " << errorMessage << std::endl;
					continue;
				}
			}
			else
			{
				std::istringstream iss(input);
				T temp;
				char leftover;
				if ((iss >> temp) && !(iss >> leftover))
				{
					value = temp;
					break;
				}
				else
				{
					std::cerr << "Invalid input type. " << errorMessage << std::endl;
				}
			}
		}
	}

	/**
	 * @brief Parses command-line arguments, handles help flag, and determines the output Markup path.
	 *
	 * If the user passes `--help`, `-h`, or `/?`, this function will print usage information
	 * and signal that the program should exit. Otherwise it extracts the Markup output path
	 * from the arguments, or prompts the user for one if none was provided.
	 *
	 * @param argc Argument count from main().
	 * @param argv Argument values from main().
	 * @param[out] markupPath The resulting filesystem path for the Markup output file.
	 * @return true if execution should continue, or false if the program should exit immediately.
	 */
	bool handleCommandLine(int argc, char** argv, std::filesystem::path& markupPath) 
	{
		
		bool ret = false;

		ArgParser* parser = new ArgParser;
		parser->registerArg("help", ArgParser::ArgType::Bool, 'h');
		parser->registerArg("auto", ArgParser::ArgType::Bool, 'a');
		parser->registerArg("markupfile", ArgParser::ArgType::String, 'f');

		parser->parse(argc, argv);

		if (parser->f_bool("help")) 
		{
			std::cout << "Mastering Utility Wizard\n"
					  << "Usage: mastering_wizard [--markupfile=<path>] [--auto] [--help]\n\n"
					  << "Options:\n"
					  << "  --markupfile, -f   Path to output Markup file.\n"
					  << "  --auto, -a      Run in automatic mode with default settings.\n"
					  << "  --help, -h      Display this help message.\n";
			ret = false;
		}
		if (parser->f_bool("auto")) 
		{
			g_AutoAddSongs = true;
		}
		std::string markupfile = parser->f_string("markupfile");
		if (!markupfile.empty()) {
			markupPath = markupfile;
			ret = true;
		} 
		else 
		{
			if (argc > 1 && std::filesystem::exists(argv[1]))
			{    
				markupPath = argv[1];
				ret = true;
			}
			else
			{ 
				markupPath = "new.mas";
				std::cout << "Using default: " << markupPath << '\n';
				ret = true;
			}        
		}
		delete parser;
		return ret;
	}


	/**
	 * @brief Interactively collects album and song metadata from the user.
	 *
	 * This function runs the full wizard flow, prompting the user for album data,
	 * song data, and song inclusion preferences. It may preview content and allow
	 * the user to reject or auto-accept songs. When fMarkupshed, the resulting list
	 * of albums is returned through the output parameter.
	 *
	 * @param[out] albums Container that will receive the completed album structures.
	 * @throws std::runtime_error on invalid input or unexpected user responses.
	 */
	void collectAlbumsInteractively(MasteringUtility::Albums& albums) 
	{
		int albumCount = 1;
		MasteringWizard::prompt("Enter number of albums to create", albumCount, "Must be a positive integer.", &albumCount);
		if (albumCount < 1)
		{
			albumCount = 1;
		}

		for (int i = 0; i < albumCount; ++i)
		{
			MasteringUtility::Album album;
			album.Path = ".";
			album.ID = i + 1;
			std::string defaultCopyright = "N/A";

			MasteringWizard::prompt("Enter Album Title", album.Title, "Album title cannot be empty.");
			MasteringWizard::prompt("Enter Album Artist", album.Artist, "Album artist cannot be empty.");
			MasteringWizard::prompt("Enter Album Genre", album.Genre, "Album genre cannot be empty.");
			MasteringWizard::prompt("Enter Album Year", album.Year, "Album year cannot be empty.");
			MasteringWizard::prompt("Enter Album Copyright Info", album.Copyright, "Album copyright cannot be empty.", &defaultCopyright);
			MasteringWizard::prompt("Enter Relative Path to save songs", album.NewPath, "Album save path cannot be empty.");
			MasteringWizard::prompt("Enter Relative Path to Album Art", album.AlbumArt, "Album art path cannot be empty.");

			int songCount = 0;
			MasteringWizard::prompt("How many songs in \"" + album.Title + "\"?", songCount, "Must be 0 or a positive integer.", &songCount);
			if (songCount < 0) throw std::runtime_error("Song count cannot be negative.");

			for (int j = 0; j < songCount; ++j)
			{
				MasteringUtility::Song song;
				song.TrackNumber = j + 1;
				song.Path = ".";
				song.ID = j + 1;
				song.Album = album.Title;
				std::string defaultCodec = "copy";

				MasteringWizard::prompt("Enter Song Source Filename", song.Path, "Song source filename cannot be empty.");
				MasteringWizard::prompt("Enter Song Title", song.Title, "Song title cannot be empty.");
				MasteringWizard::prompt("Enter Song Artist", song.Artist, "Unexpected issue", &album.Artist);
				MasteringWizard::prompt("Enter Song Genre", song.Genre, "Unexpected issue", &album.Genre);
				MasteringWizard::prompt("Enter Song Year", song.Year, "Unexpected issue", &album.Year);
				MasteringWizard::prompt("Enter Song Copyright Info", song.Copyright, "Unexpected issue", &album.Copyright);
				MasteringWizard::prompt("Enter New Filename", song.NewPath, "New filename cannot be empty.");
				MasteringWizard::prompt("Enter Song Codec (mp3, flac, etc.)", song.Codec, "Unexpected issue", &defaultCodec);
				
				MasteringWizard::preview::song(song);
				bool addSong = g_AutoAddSongs;
				if (!addSong) {
					char response = '\0';
					while (response != 'y' && response != 'n' && response != 'a') 
					{ 
					MasteringWizard::prompt("Would you like to add this song to the list? (y/n/a)", response, "Please enter 'y', 'n', or 'a'.");
					}
					switch (response) {
					case 'a':
						g_AutoAddSongs = true;
						[[fallthrough]];
					case 'y':
						addSong = true;
						break;
					case 'n':
						break;
					default:
						throw std::runtime_error("Unexpected response.");
						unreachable();
					}
				}
				if (addSong)
				{
					album.SongsList.push_back(song);
				}
			}
		
			MasteringWizard::preview::album(album);
			char addAlbumResponse = 'Y';
			MasteringWizard::prompt("Would you like to add this album to the list? (y/n)", addAlbumResponse, "Please enter 'y' or 'n'.", &addAlbumResponse);
			if (addAlbumResponse == 'y' || addAlbumResponse == 'Y')
			{
				albums.push_back(album);
			}
		}
	}


	/**
	 * @brief Prompts the user to confirm whether the Markup file should be written.
	 *
	 * Presents the user with a yes/no prompt. If the user declines, the caller may
	 * choose to exit without saving. This function only gathers the user's choice.
	 *
	 * @param markupPath Path of the file being saved, used for display in messages.
	 * @return true if the user confirmed saving, false otherwise.
	 */
	bool confirmSaveInteractively(const std::filesystem::path& markupPath) 
	{
		 char saveResponse = 'Y';
		std::string promptMsg = "Would you like to save \"" + markupPath.string() + "\"? (y/n)";
		MasteringWizard::prompt(promptMsg, saveResponse, "Please enter 'y' or 'n'.", &saveResponse);
		if (saveResponse == 'y' || saveResponse == 'Y')
		{
			return true;
		}
		return false;
	}


	/**
	 * @brief Writes the album metadata to disk as an Markup file.
	 *
	 * This function wraps MasteringUtility::SaveMarkup and provides a single place
	 * for output, logging, or error handling related to saving the Markup file.
	 *
	 * @param masterer Instance of MasteringUtility used to write the file.
	 * @param albums Albums to serialize into the Markup format.
	 * @param markupPath Destination file path, including filename.
	 */
	void saveMarkupFile(MasteringUtility& masterer,
					const MasteringUtility::Albums& albums,
					const std::filesystem::path& markupPath) 
	{   
		masterer.SaveMarkup(albums, markupPath);
		std::cout << "Markup file saved to \"" << markupPath.string() << "\" successfully.\n";
	}

} // namespace MasteringWizard

/// @brief CRT Entry Point
int main(int argc, char** argv)
{
	std::cout << "Daniel's Mastering Utility -- Wizard\n\n";
	
	MasteringUtility masterer;
	MasteringUtility::Albums albums;
	std::filesystem::path markupPath;

	if (!MasteringWizard::handleCommandLine(argc, argv, markupPath))
	{
		cleanup(0);
		return 0;
	}

	try
	{
		MasteringWizard::collectAlbumsInteractively(albums);

		if (albums.empty())
		{
			std::cout << "No albums to save. Exiting.\n";
			cleanup(0);
			return 0;
		}
		if (MasteringWizard::confirmSaveInteractively(markupPath))
		{
			MasteringWizard::saveMarkupFile(masterer, albums, markupPath);
		}
		else
		{
			std::cout << "Save operation cancelled by user. Exiting without saving.\n";
		}
	} catch (const std::exception& ex)
	{
		std::cerr << "[Fatal Error] " << ex.what() << "\n";
		cleanup(1);
		return 1; 
	} catch (...)
	{
		std::cerr << "[Fatal Error] Unknown exception occurred.\n";
		cleanup(-1);
		return -1;
	}
	cleanup(0);
	return 0;
}
