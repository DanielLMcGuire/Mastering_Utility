/**
 * @file MasteringUtil.cpp
 * @brief Implementation of the Mastering Utility
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
#include <regex>

/**
 * @brief Get Audio Codecs
 * Requests audio codecs from FFMPEG
 * @return set of audio codecs
 */
static std::unordered_set<std::string> getAudioCodecs() {
	std::unordered_set<std::string> audioCodecs;

	audioCodecs.insert("copy");
	audioCodecs.insert("libmp3lame");
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
  * @param input String containing arguments
  * @return Vector containing arguments
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

/**
  * @brief Sanitize arguments to prevent command injection
  *
  * Ensures arguments don't contain dangerous patterns that could
  * overwrite critical ffmpeg parameters or inject shell commands.
  *
  * @param args Raw arguments string
  * @return Sanitized arguments string
  */
static std::string sanitizeArguments(const std::string& args)
{
	std::cout << "Specified: " << args << '\n';
	if (args.empty()) return args;

	std::string sanitized = trim(args);

	// List of dangerous patterns that could overwrite critical parameters
	std::vector<std::string> dangerousPatterns = {
		"-i ", "--input",           
		"ffmpeg", "ffprobe",        
		"-metadata",                
		"-c:a", "-codec:a",         
		"-map",                    
		"&&", "||", "|", ";",      
		"$(", "`",                 
		">", "<", ">>",             
		"-y", "-n"                 
	};

	std::string lower = sanitized;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

	for (const auto& pattern : dangerousPatterns) {
		std::string lowerPattern = pattern;
		std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), ::tolower);
		if (lower.find(lowerPattern) != std::string::npos) {
			std::cerr << "[sanitizeArguments] Warning: Removing dangerous pattern '"
				<< pattern << "' from arguments\n";
			return "";
		}
	}

	if (!sanitized.empty() && sanitized[0] != '-') {
		std::cerr << "[sanitizeArguments] Warning: Arguments must start with '-'. "
			<< "Adding prefix.\n";
		sanitized = "- " + sanitized; 
	}
	std::cout << "Cleaned: " << args << '\n';
	return sanitized;
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
						currentAlbum.Title = cleanString(args[0]);
						currentAlbum.Artist = cleanString(args[1]);
						currentAlbum.Copyright = cleanString(args[2]);
						currentAlbum.AlbumArt = cleanString(args[3]);
						currentAlbum.Path = cleanString(args[4]);
						currentAlbum.NewPath = cleanString(args[5]);
						currentAlbum.Genre = cleanString(args[6]);
						currentAlbum.Year = cleanString(args[7]);
						if (args.size() > 8) currentAlbum.Comment = cleanString(args[8]);
						if (args.size() > 9) currentAlbum.arguments = sanitizeArguments(cleanString(args[9]));
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
						song.Title = cleanString(args[0]);
						song.Artist = cleanString(args[1]);
						try { song.TrackNumber = std::stoi(args[2]); }
						catch (...) { std::cerr << "[ParseMarkup] Warning: invalid track number '" << args[2] << "'\n"; song.TrackNumber = 0; }
						song.Path = cleanString(args[3]);
						song.NewPath = cleanString(args[4]);
						song.Codec = cleanString(args[5]);
						song.Genre = cleanString(args[6]);
						song.Year = cleanString(args[7]);
						if (args.size() > 8) song.Comment = cleanString(args[8]);
						if (args.size() > 9) song.arguments = sanitizeArguments(cleanString(args[9]));
					}
				}

				song.Album = currentAlbum.Title;
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

			if (!album.arguments.empty())
				file << ", \"" << album.arguments << "\"";

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

				if (!song.arguments.empty())
				{ 
					if (song.Comment.empty()) file << ". \" \"";
					file << ", \"" << song.arguments << "\"";
				}

				file << ")\n";
			}

			file << "}\n\n";
		}
	}
	catch (const std::exception& ex) {
		std::cerr << "[SaveMarkup] Exception: " << ex.what() << std::endl;
	}
	catch (...) {
		std::cerr << "[SaveMarkup] Unknown exception" << std::endl;
	}
}

void MasteringUtility::ProcessAlbum(const Album& album)
{
	try
	{
		std::cout << "Album: " << album.Title
			<< " (" << album.Artist << ", " << album.Year << ")" << std::endl;

		if (!album.NewPath.empty()) std::filesystem::create_directories(album.NewPath);

			auto codec = album.SongsList[1].Codec;
			if (codec == "wav" || codec == "WAV" || codec == "flac" || codec == "FLAC") 
			{
				std::filesystem::path source = album.Path / album.AlbumArt;
				std::filesystem::path destination = album.NewPath / ("cover" + album.AlbumArt.extension().string());

				std::ifstream src(source, std::ios::binary);
				if (!src)
					throw std::runtime_error("Failed to open source: " + source.string());

				std::ofstream dest(destination, std::ios::binary);
				if (!dest)
					throw std::runtime_error("Failed to open destination: " + destination.string());

				dest << src.rdbuf();
			}

		for (const auto& song : album.SongsList) ProcessSong(song, album);
	}
	catch (const std::exception& ex) { std::cerr << "[ProcessAlbum] Exception: " << ex.what() << std::endl; }
	catch (...) { std::cerr << "[ProcessAlbum] Unknown exception" << std::endl; }
}

void MasteringUtility::ProcessSong(const Song& song, const Album& album)
{
	try
	{
		std::unordered_set<std::string> audioCodecs = getAudioCodecs();
		if (!audioCodecs.contains(trim(song.Codec))) throw std::runtime_error("Invalid audio codec: " + trim(song.Codec));
		if (!std::filesystem::exists(song.Path)) throw std::runtime_error("File not found: " + song.Path.string());
		std::cout << "Encoding: " << song.Title
			<< " -> " << song.NewPath << " [" << song.Codec << "]" << std::endl;

		std::filesystem::path new_songPath = album.NewPath / song.NewPath;
		std::filesystem::create_directories(new_songPath.parent_path());

		std::ostringstream cmd;
		cmd << "ffmpeg -y "
			<< "-i \"" << song.Path.string() << "\" ";
		if (song.Codec != "flac" || song.Codec != "FLAC" || song.Codec != "wav" || song.Codec != "WAV")
			if (!song.Codec.empty() && !album.AlbumArt.empty())
						cmd << "-i \"" << album.AlbumArt.string() << "\" -map 0:a -map 1:v -id3v2_version 3 ";
		if (!song.Title.empty())      cmd << "-metadata title=\"" << song.Title << "\" ";
		if (!song.Artist.empty())     cmd << "-metadata artist=\"" << song.Artist << "\" ";
		if (!song.Album.empty())      cmd << "-metadata album=\"" << song.Album << "\" ";
		if (!song.Genre.empty())      cmd << "-metadata genre=\"" << song.Genre << "\" ";
		if (!song.Year.empty())       cmd << "-metadata date=\"" << song.Year << "\" ";
		if (!song.Copyright.empty())  cmd << "-metadata copyright=\"" << song.Copyright << "\" ";
		if (!song.Comment.empty())    cmd << "-metadata comment=\"" << song.Comment << "\" ";
		cmd << "-metadata encoder-info=\"Daniel's Mastering Utility\" ";
		if (!song.Codec.empty())      cmd << "-c:a \"" << song.Codec << "\" ";

		if (!album.arguments.empty())  cmd << album.arguments << " ";
		if (!song.arguments.empty())  cmd << song.arguments << " ";

		cmd << "-metadata track=\"" << song.TrackNumber << "\" "
			<< "\"" << new_songPath.string() << "\"";

		std::string command = cmd.str();
		int result = -1;

		for (int attempt = 1; attempt <= 3; ++attempt)
		{
			std::cout << "  Running (attempt " << attempt << "): " << command << std::endl;

#ifdef _WIN32
			std::unique_ptr<FILE, int(*)(FILE*)> pipe(_popen(command.c_str(), "r"), _pclose);
#else
			std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(command.c_str(), "r"), (int(*)(FILE*))pclose);
#endif
			if (!pipe) 
			{
				std::cerr << "  Failed to open pipe for command execution\n";
				result = -1;
				continue;
			}

			std::string output;
			char buffer[256];
			while (fgets(buffer, sizeof(buffer), pipe.get()))
				output += buffer;

			result = 0;
			
			if (result == 0) { std::cout << "  Success!" << std::endl; break; }
			else 
			{
				std::cerr << "  ffmpeg failed with code: " << result << std::endl;
				std::cerr << output;
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