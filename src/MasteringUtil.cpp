#include "MasteringUtil.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <filesystem>

// --- helper funcs ---
static std::string trim(const std::string& s)
{
    auto start = s.find_first_not_of(" \t\r\n");
    auto end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

static std::string cleanString(const std::string& s)
{
    std::string out = trim(s);
    if (!out.empty() && out.front() == '"') out.erase(out.begin());
    if (!out.empty() && out.back() == '"') out.pop_back();
    return out;
}

static std::vector<std::string> splitArgs(const std::string& argStr)
{
    std::vector<std::string> args;
    std::string current;
    bool inQuotes = false;

    for (char c : argStr)
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


void MasteringUtility::ParseINI(const std::filesystem::path& albumINI, Albums& albums)
{
    try
    {
        std::ifstream file(albumINI);
        if (!file.is_open())
        {
            std::cerr << "[ParseINI] Could not open INI file: " << albumINI << std::endl;
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
                    catch (...) { std::cerr << "[ParseINI] Warning: invalid album ID '" << idStr << "'\n"; }
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
                    catch (...) { std::cerr << "[ParseINI] Warning: invalid song ID '" << idStr << "'\n"; }
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
                        catch (...) { std::cerr << "[ParseINI] Warning: invalid track number '" << args[2] << "'\n"; song.TrackNumber = 0; }
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
    catch (const std::exception& ex) { std::cerr << "[ParseINI] Exception: " << ex.what() << std::endl; }
    catch (...) { std::cerr << "[ParseINI] Unknown exception" << std::endl; }
}

void MasteringUtility::SaveINI(const Albums& albums, const std::filesystem::path& iniFile) {
    try {
        std::ofstream file(iniFile);
        if (!file.is_open()) {
            std::cerr << "[SaveINI] Could not open output INI file: " << iniFile << std::endl;
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
        std::cerr << "[SaveINI] Exception: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "[SaveINI] Unknown exception" << std::endl;
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


void MasteringUtility::ProcessSong(const Song& song, const std::filesystem::path& newFolder)
{
    try
    {
        std::cout << "Encoding: " << song.Title
            << " -> " << song.NewPath << " [" << song.Codec << "]" << std::endl;

        std::filesystem::path new_songPath = newFolder / song.NewPath;

        std::ostringstream cmd;
        cmd << "ffmpeg -y "
            << "-i \"" << song.Path << "\" "  // ✅ Use song.Path as INPUT
            << "-c:a " << song.Codec << " ";

        if (!song.Title.empty())     cmd << "-metadata title=\"" << song.Title << "\" ";
        if (!song.Artist.empty())    cmd << "-metadata artist=\"" << song.Artist << "\" ";
        if (!song.Album.empty())     cmd << "-metadata album=\"" << song.Album << "\" ";
        if (!song.Genre.empty())     cmd << "-metadata genre=\"" << song.Genre << "\" ";
        if (!song.Year.empty())      cmd << "-metadata date=\"" << song.Year << "\" ";
        if (!song.Comment.empty())   cmd << "-metadata comment=\"" << song.Comment << "\" ";
        if (!song.Copyright.empty()) cmd << "-metadata copyright=\"" << song.Copyright << "\" ";

        if (song.Codec == "flac")        cmd << "-compression_level 12 ";
        if (song.Codec == "libmp3lame")  cmd << "-qscale:a 3 ";

        cmd << "-metadata track=\"" << song.TrackNumber << "\" ";

        cmd << "\"" << new_songPath.string() << "\"";  // ✅ Use new_songPath as OUTPUT

        std::string command = cmd.str();

        int result = -1;
        for (int attempt = 1; attempt <= 3; ++attempt)
        {
            std::cout << "  Running (attempt " << attempt << "): " << command << std::endl;
            result = std::system(command.c_str());
            if (result == 0) { std::cout << "  Success!" << std::endl; break; }
            else { std::cerr << "  ffmpeg failed with code: " << result << std::endl; if (attempt < 3) std::cerr << "  Retrying...\n"; }
        }

        if (result != 0)
            std::cerr << "  Giving up on " << song.Title << " after 3 failed attempts\n";
    }
    catch (const std::exception& ex) { std::cerr << "[ProcessSong] Exception: " << ex.what() << std::endl; }
    catch (...) { std::cerr << "[ProcessSong] Unknown exception" << std::endl; }
}

void MasteringUtility::Master(const std::filesystem::path& albumINI)
{
    try
    {
        Albums albums;
        ParseINI(albumINI, albums);

        for (const auto& album : albums)
            ProcessAlbum(album);
    }
    catch (const std::exception& ex) { std::cerr << "[Master] Exception: " << ex.what() << std::endl; }
    catch (...) { std::cerr << "[Master] Unknown exception" << std::endl; }
}

