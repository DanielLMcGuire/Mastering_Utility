/**
 * @file MasteringUtil.cpp
 * @brief Implementation of the Mastering Utility
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

#include "MasteringUtil.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <array>
#include <cstdio>     
#include <memory>     
#include <string>






/**
 * @brief Get Audio Codecs
 * Requests audio codecs from FFMPEG
 * @return set of audio codecs 
 */
static std::unordered_set<std::string> getAudioCodecs() {
	std::unordered_set<std::string> audioCodecs;

	audioCodecs.insert("copy");
	const char* cmd = "ffmpeg -codecs";

#ifdef _WIN32
	FILE* pipe = _popen(cmd, "r");
#else
	FILE* pipe = popen(cmd, "r");
#endif
	if (!pipe) {
		std::cerr << "Failed to run ffmpeg command\n";
		return audioCodecs;
	}

	std::array<char, 512> buffer;
	bool parsing = false;

	while (fgets(buffer.data(), buffer.size(), pipe)) {
		std::string line(buffer.data());

		if (!parsing) {
			if (line.find("Codecs:") != std::string::npos) parsing = true;
			continue;
		}

		if (line.find("-------") != std::string::npos) continue;

		size_t firstChar = line.find_first_not_of(" \t");
		if (firstChar == std::string::npos) continue;
		line = line.substr(firstChar);

		if (line.size() < 7) continue;

		std::string flags = line.substr(0, 6);
		if (flags.size() >= 3 && flags[0] == 'D' && flags[1] == 'E' && flags[2] == 'A') {
			size_t nameStart = line.find_first_not_of(" \t", 6);
			if (nameStart == std::string::npos) continue;
			size_t nameEnd = line.find_first_of(" \t", nameStart);
			std::string codecName = line.substr(nameStart, nameEnd - nameStart);

			audioCodecs.insert(codecName); 
		}
	}

#ifdef _WIN32
	_pclose(pipe);
#else
	pclose(pipe);
#endif

	return audioCodecs;
}

/**
  * @brief Trim whitespace from string
  * @param input String to trim
  * @return Trimmed string
  */
static std::string trim(const std::string& input)
{
	auto start = input.find_first_not_of(" \t\r\n");
	auto end = input.find_last_not_of(" \t\r\n");
	return (start == std::string::npos) ? "" : input.substr(start, end - start + 1);
}

/**
  * @brief Clean a string
  * 
  * Trims whitespace and removes quotes from strings.   
  * 
  * @param input String to clean
  * @return Cleaned string
  */
static std::string cleanString(const std::string& input)
{
	std::string out = trim(input);
	if (!out.empty() && out.front() == '"') out.erase(out.begin());
	if (!out.empty() && out.back() == '"') out.pop_back();
	return out;
}

/**
  * @brief Split arguments
  *
  * Tranforms string of arguments wrapped in quotes '"' into a vector
  *
  * @param input String contaMarkupng arguments
  * @return Vector contaMarkupng arguments
  */
static std::vector<std::string> splitArgs(const std::string& input)
{
	std::vector<std::string> args;
	std::string current;
	bool inQuotes = false;

	for (char c : input)
	{
		if (c == '"')
		{
			inQuotes = !inQuotes;
			continue;             
		}
		else if (c == ',' && !inQuotes)
		{
			args.push_back(trim(current));
			current.clear();
		}
		else
		{
			current.push_back(c);
		}
	}

	if (!current.empty()) args.push_back(trim(current));
	return args;
}

void MasteringUtility::ParseMarkup(const std::filesystem::path& markupFile, Albums& albums)
{
	try
	{
		std::ifstream file(markupFile);
		if (!file.is_open())
		{
			std::cerr << "[ParseMarkup] Could not open Markup file: " << markupFile << std::endl;
			return;
		}

		std::string line;
		Album currentAlbum;
		bool insideAlbum = false;

		while (std::getline(file, line))
		{
			line = trim(line);
			if (line.empty() || line[0] == ';' || line[0] == '[') continue;

			if (line.rfind("album", 0) == 0)
			{
				insideAlbum = true;
				currentAlbum = Album();

				auto idStart = std::string("album").size();
				auto idEnd = line.find('(');
				if (idEnd != std::string::npos) {
					std::string idStr = trim(line.substr(idStart, idEnd - idStart));
					try { if (!idStr.empty()) currentAlbum.ID = std::stoi(idStr); }
					catch (...) { std::cerr << "[ParseMarkup] Warning: invalid album ID '" << idStr << "'\n"; }
				}

				auto start = line.find('(');
				auto end = line.find_last_of(')');
				if (start != std::string::npos && end != std::string::npos)
				{
					auto args = splitArgs(line.substr(start + 1, end - start - 1));
					if (args.size() >= 8)
					{
						currentAlbum.Title     = cleanString(args[0]);
						currentAlbum.Artist    = cleanString(args[1]);
						currentAlbum.Copyright = cleanString(args[2]);
						currentAlbum.AlbumArt  = cleanString(args[3]);
						currentAlbum.Path      = cleanString(args[4]);
						currentAlbum.NewPath   = cleanString(args[5]);
						currentAlbum.Genre     = cleanString(args[6]);
						currentAlbum.Year      = cleanString(args[7]);
						if (args.size() > 8) currentAlbum.Comment = cleanString(args[8]);
					}
				}
			}
			else if (line.rfind("song", 0) == 0 && insideAlbum)
			{
				Song song;

				auto idStart = std::string("song").size();
				auto idEnd = line.find('(');
				if (idEnd != std::string::npos) {
					std::string idStr = trim(line.substr(idStart, idEnd - idStart));
					try { if (!idStr.empty()) song.ID = std::stoi(idStr); }
					catch (...) { std::cerr << "[ParseMarkup] Warning: invalid song ID '" << idStr << "'\n"; }
				}

				auto start = line.find('(');
				auto end = line.find_last_of(')');
				if (start != std::string::npos && end != std::string::npos)
				{
					auto args = splitArgs(line.substr(start + 1, end - start - 1));
					if (args.size() >= 8)
					{
						song.Title  = cleanString(args[0]);
						song.Artist = cleanString(args[1]);
						try { song.TrackNumber = std::stoi(args[2]); }
						catch (...) { std::cerr << "[ParseMarkup] Warning: invalid track number '" << args[2] << "'\n"; song.TrackNumber = 0; }
						song.Path    = cleanString(args[3]);
						song.NewPath = cleanString(args[4]);
						song.Codec   = cleanString(args[5]);
						song.Genre   = cleanString(args[6]);
						song.Year    = cleanString(args[7]);
						if (args.size() > 8) song.Comment = cleanString(args[8]);
					}
				}

				song.Album     = currentAlbum.Title;
				song.Copyright = currentAlbum.Copyright;

				currentAlbum.SongsList.push_back(song);
			}
			else if (line.find("}") != std::string::npos && insideAlbum)
			{
				albums.push_back(currentAlbum);
				insideAlbum = false;
			}
		}
	}
	catch (const std::exception& ex) { std::cerr << "[ParseMarkup] Exception: " << ex.what() << std::endl; }
	catch (...) { std::cerr << "[ParseMarkup] Unknown exception" << std::endl; }
}

void MasteringUtility::SaveMarkup(const Albums& albums, const std::filesystem::path& markupFile) {
	try {
		std::ofstream file(markupFile);
		if (!file.is_open()) {
			std::cerr << "[SaveMarkup] Could not open output Markup file: " << markupFile << std::endl;
			return;
		}

		for (const auto& album : albums) {
			file << "album " << album.ID << " ("
				 << "\"" << album.Title << "\", "
				 << "\"" << album.Artist << "\", "
				 << "\"" << album.Copyright << "\", "
				 << "\"" << album.AlbumArt << "\", "
				 << "\"" << album.Path << "\", "
				 << "\"" << album.NewPath << "\", "
				 << "\"" << album.Genre << "\", "
				 << "\"" << album.Year << "\"";

			if (!album.Comment.empty())
				file << ", \"" << album.Comment << "\"";

			file << ")\n{\n";

			for (const auto& song : album.SongsList) {
				file << "    song " << song.ID << " ("
					 << "\"" << song.Title << "\", "
					 << "\"" << song.Artist << "\", "
					 << song.TrackNumber << ", "
					 << "\"" << song.Path << "\", "
					 << "\"" << song.NewPath << "\", "
					 << "\"" << song.Codec << "\", "
					 << "\"" << song.Genre << "\", "
					 << "\"" << song.Year << "\"";

				if (!song.Comment.empty())
					file << ", \"" << song.Comment << "\"";

				file << ")\n";
			}

			file << "}\n\n";
		}
	} catch (const std::exception& ex) {
		std::cerr << "[SaveMarkup] Exception: " << ex.what() << std::endl;
	} catch (...) {
		std::cerr << "[SaveMarkup] Unknown exception" << std::endl;
	}
}

void MasteringUtility::ProcessAlbum(const Album& album)
{
	try
	{
		std::cout << "Album: " << album.Title
			<< " (" << album.Artist << ", " << album.Year << ")" << std::endl;

		if (!album.NewPath.empty())
			std::filesystem::create_directories(album.NewPath);

		for (const auto& song : album.SongsList)
		{
			ProcessSong(song, album.NewPath);
		}
	}
	catch (const std::exception& ex) { std::cerr << "[ProcessAlbum] Exception: " << ex.what() << std::endl; }
	catch (...) { std::cerr << "[ProcessAlbum] Unknown exception" << std::endl; }
}

void MasteringUtility::ProcessSong(const Song& song, const std::filesystem::path& destFolder)
{
    try
    {
        std::unordered_set<std::string> audioCodecs = getAudioCodecs();
        if (!audioCodecs.contains(trim(song.Codec))) throw std::runtime_error("Invalid audio codec: " + trim(song.Codec));
        if (!std::filesystem::exists(song.Path)) throw std::runtime_error("File not found: " + song.Path.string());
        std::cout << "Encoding: " << song.Title
                  << " -> " << song.NewPath << " [" << song.Codec << "]" << std::endl;

        std::filesystem::path new_songPath = destFolder / song.NewPath;

        std::ostringstream cmd;
        cmd << "ffmpeg -y "
            << "-i \"" << song.Path.string() << "\" ";  

        if (!song.Title.empty())     cmd << "-metadata title=\"" << song.Title << "\" ";
        if (!song.Artist.empty())    cmd << "-metadata artist=\"" << song.Artist << "\" ";
        if (!song.Album.empty())     cmd << "-metadata album=\"" << song.Album << "\" ";
        if (!song.Genre.empty())     cmd << "-metadata genre=\"" << song.Genre << "\" ";
        if (!song.Year.empty())      cmd << "-metadata date=\"" << song.Year << "\" ";
        if (!song.Comment.empty())   cmd << "-metadata comment=\"" << song.Comment << "\" ";
        if (!song.Copyright.empty()) cmd << "-metadata copyright=\"" << song.Copyright << "\" ";

        if (song.Codec == "flac")        cmd << "-compression_level 12 ";
        if (song.Codec == "mp3")         cmd << "-qscale:a 3 ";

        cmd << "-metadata track=\"" << song.TrackNumber << "\" "
            << "\"" << new_songPath.string() << "\"";

        std::string command = cmd.str();
        int result = -1;

        for (int attempt = 1; attempt <= 3; ++attempt)
        {
            std::cout << "  Running (attempt " << attempt << "): " << command << std::endl;

#ifdef _WIN32
            FILE* pipe = _popen(command.c_str(), "r");
#else
            FILE* pipe = popen(command.c_str(), "r");
#endif
            if (!pipe) { 
                std::cerr << "  Failed to open pipe for command execution\n"; 
                result = -1; 
                continue; 
            }

            std::string output;
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), pipe))
                output += buffer;
#ifdef _WIN32
			int result = _pclose(pipe); 
#else
			int status = pclose(pipe);   
			int result = WIFEXITED(status) ? WEXITSTATUS(status) : -1; 
#endif

		    if (result == 0) { std::cout << "  Success!" << std::endl; break; }
            else {
                std::cerr << "  ffmpeg failed with code: " << result << std::endl;
                std::cerr << output;  // show captured output only on failure
                if (attempt < 3) std::cerr << "  Retrying...\n";
            }
        }

        if (result != 0)
            std::cerr << "  Giving up on " << song.Title << " after 3 failed attempts\n";
    }
    catch (const std::exception& ex) { std::cerr << "[ProcessSong] Exception: " << ex.what() << std::endl; }
    catch (...) { std::cerr << "[ProcessSong] Unknown exception" << std::endl; }
}

void MasteringUtility::Master(const std::filesystem::path& markupFile)
{
	try
	{
		Albums albums;
		ParseMarkup(markupFile, albums);

		for (const auto& album : albums)
			ProcessAlbum(album);
	}
	catch (const std::exception& ex) { std::cerr << "[Master] Exception: " << ex.what() << std::endl; }
	catch (...) { std::cerr << "[Master] Unknown exception" << std::endl; }
}