/**
 * @file main.cpp
 * @brief Mastering Utility Wizard / Editor
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
#include <cstdlib>
#include <dconsole.h>
#include <filesystem>
#include <iostream>
#include <string>

bool g_AutoAddSongs = false;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4834)
#endif

/** @brief Indicate unreachable code
 *
 * Uses compiler-specific intrinsics to inform the optimizer that this code path
 * is unreachable.
 */
[[noreturn]] inline void unreachable() {
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
  __assume(false);
#else // GCC, Clang
  __builtin_unreachable();
#endif
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/** @brief Cleanup and exit
 *
 * @param code Exit code
 */
void cleanup(int code) {
  if (code != 0)
    std::cout << "Exited with code " << code << '\n';
  std::cout << std::endl;
  std::exit(code);
  unreachable();
}

/// @brief Mastering Wizard namespace
namespace MasteringWizard {

/// @brief Preview namespace
namespace preview {
/** @brief Display a summary of a SONG
 * @param song Song to summarize
 * @param displayInfo Display Special Styling (default = true)
 */
void song(const MasteringUtility::Song &song, bool displayInfo = true) {
  if (displayInfo)
    std::cout << "\n=== Song Summary ===\n";
  std::cout << "Title: " << song.Title << " | Artist: " << song.Artist
            << " | Album: " << song.Album << " | Track: " << song.TrackNumber
            << "\n";
  if (displayInfo)
    std::cout << "-------------------------\n";
}
/** @brief Display a summary of an ALBUM
 * @param album Album to summarize
 */
void album(const MasteringUtility::Album &album) {
  std::cout << "\n=== Album Summary ===\n";
  std::cout << "Album: " << album.Title << " | Artist: " << album.Artist
            << " | Genre: " << album.Genre << " | Year: " << album.Year
            << " | Songs: " << album.SongsList.size() << "\n";
  for (const auto &song : album.SongsList) {
    preview::song(song, false);
  }
  std::cout << "-------------------------\n";
}
} // namespace preview

/**
 * @brief Parses command-line arguments, handles help flag, and determines the
 * output Markup path.
 *
 * If the user passes `--help`, `-h`, or `/?`, this function will print usage
 * information and signal that the program should exit. Otherwise it extracts
 * the Markup output path from the arguments, or prompts the user for one if
 * none was provided.
 *
 * @param argc Argument count from main().
 * @param argv Argument values from main().
 * @param[out] markupPath The resulting filesystem path for the Markup output
 * file.
 * @return true if execution should continue, or false if the program should
 * exit immediately.
 */
bool handleCommandLine(int argc, char **argv,
                       std::filesystem::path &markupPath) {

  bool ret = false;

  DConsole conlib;
  conlib.registerFlag("help", DConsole::f::boolean, 'h');
  conlib.registerFlag("auto", DConsole::f::boolean, 'a');
  conlib.registerFlag("markupfile", DConsole::f::string, 'f');

  conlib.parse(argc, argv);

  if (conlib.f_boolean("help")) {
    std::cout
        << "Mastering Utility Wizard\n"
        << "Usage: mastering_wizard [--markupfile=<path>] [--auto] [--help]\n\n"
        << "Options:\n"
        << "  --markupfile, -f   Path to output Markup file.\n"
        << "  --auto, -a      Run in automatic mode with default settings.\n"
        << "  --help, -h      Display this help message.\n";
    ret = false;
  }
  if (conlib.f_boolean("auto")) {
    g_AutoAddSongs = true;
  }
  std::string markupfile = conlib.f_string("markupfile");
  if (!markupfile.empty()) {
    markupPath = markupfile;
    ret = true;
  } else {
    if (argc > 1 && std::filesystem::exists(argv[1])) {
      markupPath = argv[1];
      ret = true;
    } else {
      markupPath = "new.mas";
      std::cout << "Using default: " << markupPath << '\n';
      ret = true;
    }
  }
  return ret;
}

/**
 * @brief Interactively collects or edits album and song metadata from the user.
 *
 * This function runs the full wizard flow, prioritizing editing existing data.
 * It uses temporary std::string variables for all path fields to ensure
 * compatibility with DConsole::prompt, converting to/from
 * std::filesystem::path.
 *
 * @param[in,out] albums Container with existing (or empty) album structures.
 * @throws std::runtime_error on invalid input or unexpected user responses.
 */
void collectAlbumsInteractively(MasteringUtility::Albums &albums) {
  const int initialAlbumCount = static_cast<int>(albums.size());
  int albumCount = initialAlbumCount;

  if (initialAlbumCount > 0) {
    std::cout << "\nMarkup file contains " << initialAlbumCount
              << " album(s).\n";
    DConsole::prompt("Enter the total number of albums to process", albumCount,
                     "Must be a positive integer.", &albumCount);
  } else {
    DConsole::prompt("Enter number of albums to create", albumCount,
                     "Must be a positive integer.", &albumCount);
  }

  if (albumCount < 0) {
    std::cout << "Skipping album collection/editing.\n";
    return;
  }

  const std::string defaultPathStr = ".";
  const std::string defaultComment = " ";
  const std::string defaultCopyright = " ";
  const std::string defaultArgs = " ";
  const std::string defaultCodec = "copy";

  for (int i = 0; i < initialAlbumCount; ++i) {
    MasteringUtility::Album &album = albums[i];
    std::cout << "\n\n=== Editing Existing Album " << (i + 1) << "/"
              << initialAlbumCount << ": " << album.Title << " ===\n";
    MasteringWizard::preview::album(album);

    if (album.ID == 0)
      album.ID = i + 1;

    DConsole::prompt("Enter Album Title", album.Title,
                     "Album title cannot be empty.", &album.Title);
    DConsole::prompt("Enter Album Artist", album.Artist,
                     "Album artist cannot be empty.", &album.Artist);
    DConsole::prompt("Enter Album Genre", album.Genre,
                     "Album genre cannot be empty.", &album.Genre);
    DConsole::prompt("Enter Album Year", album.Year,
                     "Album year cannot be empty.", &album.Year);

    const std::string *albumCopyrightPtr =
        album.Copyright.empty() ? &defaultCopyright : &album.Copyright;
    DConsole::prompt("Enter Album Copyright Info", album.Copyright,
                     "Unexpected issue", albumCopyrightPtr);

    std::string albumNewPathStr = album.NewPath.string();
    const std::string *albumNewPathPtr =
        albumNewPathStr.empty() ? &defaultPathStr : &albumNewPathStr;
    DConsole::prompt("Enter Relative Path to save songs", albumNewPathStr,
                     "Album save path cannot be empty.", albumNewPathPtr);
    album.NewPath = albumNewPathStr;

    std::string albumAlbumArtStr = album.AlbumArt.string();
    const std::string *albumAlbumArtPtr =
        albumAlbumArtStr.empty() ? &defaultPathStr : &albumAlbumArtStr;
    DConsole::prompt("Enter Relative Path to Album Art", albumAlbumArtStr,
                     "Album art path cannot be empty.", albumAlbumArtPtr);
    album.AlbumArt = albumAlbumArtStr;

    const std::string *albumArgsPtr =
        album.arguments.empty() ? &defaultArgs : &album.arguments;
    DConsole::prompt("Enter additional arguments for FFMPEG (if any)",
                     album.arguments, "Unexpected issue", albumArgsPtr);

    const int initialSongCount = static_cast<int>(album.SongsList.size());
    int songCount = initialSongCount;
    std::cout << "\nAlbum \"" << album.Title << "\" currently has "
              << initialSongCount << " song(s).\n";

    DConsole::prompt("Enter the total number of songs to process in \"" +
                         album.Title + "\"",
                     songCount, "Must be 0 or a positive integer.", &songCount);

    if (songCount < 0)
      throw std::runtime_error("Song count cannot be negative.");

    for (int j = 0; j < initialSongCount; ++j) {
      MasteringUtility::Song &song = album.SongsList[j];
      std::cout << "\n--- Editing Existing Song " << (j + 1) << "/"
                << initialSongCount << ": " << song.Title << " ---\n";
      MasteringWizard::preview::song(song, false);

      if (song.ID == 0)
        song.ID = j + 1;
      if (song.Album.empty())
        song.Album = album.Title;

      std::string songPathStr = song.Path.string();
      const std::string *songPathPtr =
          songPathStr.empty() ? &defaultPathStr : &songPathStr;
      DConsole::prompt("Enter Song Source Filename", songPathStr,
                       "Song source filename cannot be empty.", songPathPtr);
      song.Path = songPathStr;
      DConsole::prompt("Enter Song Title", song.Title,
                       "Song title cannot be empty.", &song.Title);
      const std::string *songArtistPtr =
          song.Artist.empty() ? &album.Artist : &song.Artist;
      DConsole::prompt("Enter Song Artist", song.Artist, "Unexpected issue",
                       songArtistPtr);
      const std::string *songGenrePtr =
          song.Genre.empty() ? &album.Genre : &song.Genre;
      DConsole::prompt("Enter Song Genre", song.Genre, "Unexpected issue",
                       songGenrePtr);
      const std::string *songYearPtr =
          song.Year.empty() ? &album.Year : &song.Year;
      DConsole::prompt("Enter Song Year", song.Year, "Unexpected issue",
                       songYearPtr);
      const std::string *songCopyrightPtr =
          song.Copyright.empty() ? &album.Copyright : &song.Copyright;
      DConsole::prompt("Enter Song Copyright Info", song.Copyright,
                       "Unexpected issue", songCopyrightPtr);
      std::string songNewPathStr = song.NewPath.string();
      const std::string *songNewPathPtr =
          songNewPathStr.empty() ? &defaultPathStr : &songNewPathStr;
      DConsole::prompt("Enter New Filename", songNewPathStr,
                       "New filename cannot be empty.", songNewPathPtr);
      song.NewPath = songNewPathStr;
      const std::string *songCodecPtr =
          song.Codec.empty() ? &defaultCodec : &song.Codec;
      DConsole::prompt("Enter Song Codec (mp3, flac, etc.)", song.Codec,
                       "Unexpected issue", songCodecPtr);

      const std::string *songArgsPtr =
          song.arguments.empty() ? &defaultArgs : &song.arguments;
      DConsole::prompt("Enter additional arguments for FFMPEG (if any)",
                       song.arguments, "Unexpected issue", songArgsPtr);
    }

    for (int j = initialSongCount; j < songCount; ++j) {
      MasteringUtility::Song song;
      std::cout << "\n--- Adding New Song " << (j + 1) << " to album "
                << album.Title << " ---\n";

      song.TrackNumber = j + 1;
      song.ID = j + 1;
      song.Album = album.Title;

      std::string songPathStr = defaultPathStr;
      DConsole::prompt("Enter Song Source Filename", songPathStr,
                       "Song source filename cannot be empty.",
                       &defaultPathStr);
      song.Path = songPathStr;

      DConsole::prompt("Enter Song Title", song.Title,
                       "Song title cannot be empty.");
      DConsole::prompt("Enter Song Artist", song.Artist, "Unexpected issue",
                       &album.Artist);
      DConsole::prompt("Enter Song Genre", song.Genre, "Unexpected issue",
                       &album.Genre);
      DConsole::prompt("Enter Song Year", song.Year, "Unexpected issue",
                       &album.Year);
      DConsole::prompt("Enter Song Copyright Info", song.Copyright,
                       "Unexpected issue", &album.Copyright);

      std::string songNewPathStr;
      DConsole::prompt("Enter New Filename", songNewPathStr,
                       "New filename cannot be empty.", &defaultPathStr);
      song.NewPath = songNewPathStr;

      DConsole::prompt("Enter Song Codec (mp3, flac, etc.)", song.Codec,
                       "Unexpected issue", &defaultCodec);
      DConsole::prompt("Enter additional arguments for FFMPEG (if any)",
                       song.arguments, "Unexpected issue", &defaultArgs);

      MasteringWizard::preview::song(song);
      bool addSong = g_AutoAddSongs;
      if (!addSong) {
        char response = '\0';
        while (response != 'y' && response != 'n' && response != 'a')
          DConsole::prompt(
              "Would you like to add this song to the list? (y/n/a)", response,
              "Please enter 'y', 'n', or 'a'.");

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
        }
      }
      if (addSong) {
        album.SongsList.push_back(song);
      }
    }

    MasteringWizard::preview::album(album);
    char keepAlbumResponse = 'Y';
    DConsole::prompt("Would you like to keep the current data for album \"" +
                         album.Title + "\"? (y/n)",
                     keepAlbumResponse, "Please enter 'y' or 'n'.",
                     &keepAlbumResponse);

    if (keepAlbumResponse == 'n' || keepAlbumResponse == 'N') {
      std::cout
          << "Data for \"" << album.Title
          << "\" will be kept for now. Run the wizard again to remove it.\n";
    }
  }

  for (int i = initialAlbumCount; i < albumCount; ++i) {
    std::cout << "\n\n=== Adding New Album " << (i + 1) << " of " << albumCount
              << " ===\n";
    MasteringUtility::Album album;

    album.ID = i + 1;
    album.Comment = defaultComment;

    DConsole::prompt("Enter Album Title", album.Title,
                     "Album title cannot be empty.");
    DConsole::prompt("Enter Album Artist", album.Artist,
                     "Album artist cannot be empty.");
    DConsole::prompt("Enter Album Genre", album.Genre,
                     "Album genre cannot be empty.");
    DConsole::prompt("Enter Album Year", album.Year,
                     "Album year cannot be empty.");
    DConsole::prompt("Enter Album Copyright Info", album.Copyright,
                     "Unexpected issue", &defaultCopyright);

    std::string albumNewPathStr = defaultPathStr;
    DConsole::prompt("Enter Relative Path to save songs", albumNewPathStr,
                     "Album save path cannot be empty.", &defaultPathStr);
    album.NewPath = albumNewPathStr;

    std::string albumAlbumArtStr;
    DConsole::prompt("Enter Relative Path to Album Art", albumAlbumArtStr,
                     "Album art path cannot be empty.", &defaultPathStr);
    album.AlbumArt = albumAlbumArtStr;

    DConsole::prompt("Enter additional arguments for FFMPEG (if any)",
                     album.arguments, "Unexpected issue", &defaultArgs);

    int songCount = 0;
    DConsole::prompt("How many songs in \"" + album.Title + "\"?", songCount,
                     "Must be 0 or a positive integer.", &songCount);
    if (songCount < 0)
      throw std::runtime_error("Song count cannot be negative.");

    // Loop to Add New Songs for a New Album
    for (int j = 0; j < songCount; ++j) {
      MasteringUtility::Song song;
      std::cout << "\n--- Adding New Song " << (j + 1) << " ---\n";

      song.TrackNumber = j + 1;
      song.ID = j + 1;
      song.Album = album.Title;
      song.Comment = defaultComment;

      std::string songPathStr = defaultPathStr;
      DConsole::prompt("Enter Song Source Filename", songPathStr,
                       "Song source filename cannot be empty.",
                       &defaultPathStr);
      song.Path = songPathStr;

      DConsole::prompt("Enter Song Title", song.Title,
                       "Song title cannot be empty.");
      DConsole::prompt("Enter Song Artist", song.Artist, "Unexpected issue",
                       &album.Artist);
      DConsole::prompt("Enter Song Genre", song.Genre, "Unexpected issue",
                       &album.Genre);
      DConsole::prompt("Enter Song Year", song.Year, "Unexpected issue",
                       &album.Year);
      DConsole::prompt("Enter Song Copyright Info", song.Copyright,
                       "Unexpected issue", &album.Copyright);

      std::string songNewPathStr;
      DConsole::prompt("Enter New Filename", songNewPathStr,
                       "New filename cannot be empty.", &defaultPathStr);
      song.NewPath = songNewPathStr;

      DConsole::prompt("Enter Song Codec (mp3, flac, etc.)", song.Codec,
                       "Unexpected issue", &defaultCodec);
      DConsole::prompt("Enter additional arguments for FFMPEG (if any)",
                       song.arguments, "Unexpected issue", &defaultArgs);

      MasteringWizard::preview::song(song);
      bool addSong = g_AutoAddSongs;
      if (!addSong) {
        char response = '\0';
        while (response != 'y' && response != 'n' && response != 'a')
          DConsole::prompt(
              "Would you like to add this song to the list? (y/n/a)", response,
              "Please enter 'y', 'n', or 'a'.");

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
        }
      }
      if (addSong) {
        album.SongsList.push_back(song);
      }
    }

    MasteringWizard::preview::album(album);
    char addAlbumResponse = 'Y';
    DConsole::prompt("Would you like to add this new album to the list? (y/n)",
                     addAlbumResponse, "Please enter 'y' or 'n'.",
                     &addAlbumResponse);
    if (addAlbumResponse == 'y' || addAlbumResponse == 'Y') {
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
bool confirmSaveInteractively(const std::filesystem::path &markupPath) {
  char saveResponse = 'Y';
  std::string promptMsg =
      "Would you like to save \"" + markupPath.string() + "\"? (y/n)";
  DConsole::prompt(promptMsg, saveResponse, "Please enter 'y' or 'n'.",
                   &saveResponse);
  if (saveResponse == 'y' || saveResponse == 'Y') {
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
void saveMarkupFile(MasteringUtility &masterer,
                    const MasteringUtility::Albums &albums,
                    const std::filesystem::path &markupPath) {
  masterer.SaveMarkup(albums, markupPath);
  std::cout << "Markup file saved to \"" << markupPath.string()
            << "\" successfully.\n";
}

} // namespace MasteringWizard

/// @brief CRT Entry Point
int main(int argc, char **argv) {
  std::cout << "Daniel's Mastering Utility -- Wizard\n\n";

  MasteringUtility masterer;
  MasteringUtility::Albums albums;
  std::filesystem::path markupPath;

  if (!MasteringWizard::handleCommandLine(argc, argv, markupPath)) {
    cleanup(0);
    return 0;
  }

  try {
    if (std::filesystem::exists(markupPath))
      masterer.ParseMarkup(markupPath, albums);
    MasteringWizard::collectAlbumsInteractively(albums);

    if (albums.empty()) {
      std::cout << "No albums to save. Exiting.\n";
      cleanup(0);
      return 0;
    }
    if (MasteringWizard::confirmSaveInteractively(markupPath)) {
      MasteringWizard::saveMarkupFile(masterer, albums, markupPath);
    } else {
      std::cout
          << "Save operation cancelled by user. Exiting without saving.\n";
    }
  } catch (const std::exception &ex) {
    std::cerr << "[Fatal Error] " << ex.what() << "\n";
    cleanup(1);
    return 1;
  } catch (...) {
    std::cerr << "[Fatal Error] Unknown exception occurred.\n";
    cleanup(-1);
    return -1;
  }
  cleanup(0);
  return 0;
}
