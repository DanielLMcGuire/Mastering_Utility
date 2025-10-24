#include <iostream>
#include <string>
#include <sstream>
#include <limits>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include "../MasteringUtil.h"

/** @brief Cleanup and exit
 * 
 * @param code Exit code
 */
void cleanup(int code)
{
    if (code != 0) std::cout << "Exited with code " << code << '\n';
    std::cout << std::endl;
    std::exit(code);
}
/// @brief Mastering Wizard namespace
namespace MasteringWizard 
{

    /** @brief Prompt user for input with validation
     *
     * @tparam T Type of the input value
     * @param message Prompt message
     * @param value Reference to store the input value
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

    /** @brief Display a summary of the albums 
     * @param albums Vector of albums to summarize
     */
    void displaySummary(const MasteringUtility::Albums& albums)
    {
        std::cout << "\n=== Album Summary ===\n";
        for (const auto& album : albums)
        {
            std::cout << "Album: " << album.Title << " | Artist: " << album.Artist << " | Genre: " << album.Genre
                      << " | Year: " << album.Year << " | Songs: " << album.SongsList.size() << "\n";
            for (const auto& song : album.SongsList)
            {
                std::cout << "  [" << song.TrackNumber << "] " << song.Title << " (" << song.Artist << ")\n";
            }
            std::cout << "-------------------------\n";
        }
    }
} // namespace MasteringWizard

/// @brief CRT Entry Point
int main(int argc, char* argv[])
{
    std::cout << "Daniel's Mastering Utility -- Wizard\n";
    std::filesystem::path iniPath;
    if (argc > 1) {
        std::string arg0 = argv[0];
        std::string arg1 = argv[1];
        if (arg1 == "--help" || arg1 == "-h" || arg1 == "/?")
        {
            std::cout << "\nThis wizard will help you create an INI file for the mastering utility.\n"
                      << "You can enter 'exit' or 'quit' at any prompt to exit the wizard.\n"
                      << "Usage: masteringwizard [output_ini_file]\n";
            cleanup(0);
            return 0;
        }

        iniPath = arg1;
    }
    if (iniPath.empty()) iniPath = "new.ini";
    std::string iniPathStr = iniPath.string();
    MasteringWizard::prompt("Enter path to save INI file", iniPathStr, "INI file path cannot be empty.", &iniPathStr);
    iniPath = iniPathStr;
    if (iniPath.extension() != ".ini")
    {
        iniPath += ".ini";
    }

    MasteringUtility masterer;
    MasteringUtility::Albums albums;

    try
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
            album.Copyright = "N/A";

            MasteringWizard::prompt("Enter Album Title", album.Title, "Album title cannot be empty.");
            MasteringWizard::prompt("Enter Album Artist", album.Artist, "Album artist cannot be empty.");
            MasteringWizard::prompt("Enter Album Genre", album.Genre, "Album genre cannot be empty.");
            MasteringWizard::prompt("Enter Album Year", album.Year, "Album year cannot be empty.");
            MasteringWizard::prompt("Enter Album Copyright Info", album.Copyright, "Album copyright cannot be empty.", &album.Copyright);
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
				song.Codec = "copy";

                MasteringWizard::prompt("Enter Song Source Filename", song.Path, "Song source filename cannot be empty.");
                MasteringWizard::prompt("Enter Song Title", song.Title, "Song title cannot be empty.");
                MasteringWizard::prompt("Enter Song Artist", song.Artist, "Song artist cannot be empty", &album.Artist);
                MasteringWizard::prompt("Enter Song Genre", song.Genre, "Song genre cannot be empty", &album.Genre);
                MasteringWizard::prompt("Enter Song Year", song.Year, "Song year cannot be empty", &album.Year);
                MasteringWizard::prompt("Enter Song Copyright Info", song.Copyright, "Song copyright cannot be empty", &album.Copyright);
                MasteringWizard::prompt("Enter New Filename", song.NewPath, "New filename cannot be empty.");
                MasteringWizard::prompt("Enter Song Codec (libmp3lame, flac, etc.)", song.Codec, "Song codec cannot be empty.", &song.Codec);

                album.SongsList.push_back(song);
            }

            albums.push_back(album);
        }

        MasteringWizard::displaySummary(albums);

        masterer.SaveINI(albums, iniPath.string());
        std::cout << "Wrote " << iniPath << "\n";

        char response = 'n';
        MasteringWizard::prompt("Would you like to master this album(s)? (y/n)", response, "Please enter 'y' or 'n'.", &response);
        if (response == 'y' || response == 'Y')
        {
            for (const auto& album : albums) masterer.ProcessAlbum(album, std::filesystem::path(album.Path).string());
            std::cout << "Mastering complete.\n";
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "[Fatal Error] " << ex.what() << "\n";
        cleanup(1);
        return 1;
    }
    catch (...)
    {
        std::cerr << "[Fatal Error] Unknown exception occurred.\n";
        cleanup(-1);
        return -1;
    }
    cleanup(0);
    return 0;
}
