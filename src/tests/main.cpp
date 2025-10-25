#include <iostream>
#include <string>
#include <random>
#include <filesystem>
#include <fstream>
#include "../MasteringUtil.h"
#include "../argumentParser.h"

std::string generateRandomString(int length) {
    const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string randomString;
    for (int i = 0; i < length; ++i) {
        randomString += characters[distribution(generator)];
    }
    return randomString;
}

bool compareStrings(const std::string& a, const std::string& b, const std::string& fieldName) {
    if (a != b) {
        std::cerr << "Mismatch in " << fieldName << ": '" << a << "' != '" << b << "'\n";
        return false;
    }
    std::cout << fieldName << " OK: " << a << "\n";
    return true;
}

int main(int argc, char** argv) {
    std::filesystem::path outFile =
        std::filesystem::temp_directory_path() / ("tempdir_" + generateRandomString(8)) / "test.ini";

    std::filesystem::path tempDir = outFile.parent_path();

    if (!std::filesystem::exists(tempDir)) {
        std::filesystem::create_directory(tempDir);
    }
    if (!std::filesystem::exists(outFile)) {
        std::ofstream(outFile); 
    }


    MasteringUtility masterer;

    MasteringUtility::Albums albums;
    MasteringUtility::Album album;
    album.ID = 1;
    album.Title = "Example Album";
    album.Artist = "Example Artist";
    album.Copyright = "Example Copyright";
    album.AlbumArt = "Example Art.png";
    album.Path = "./Example Artist/Example Album/";
    album.NewPath = "./Example Artist/Example Album/Example Output/";
    album.Genre = "Example Genre";
    album.Year = "0000";
    album.Comment = "Example Comment";

    MasteringUtility::Song song1;
    song1.ID = 1;
    song1.Title = "Example Song 1";
    song1.Artist = album.Artist;
    song1.TrackNumber = 1;
    song1.Path = "Example Song.wav";
    song1.NewPath = "Example Song.mp3";
    song1.Codec = "libmp3lame";
    song1.Genre = album.Genre;
    song1.Year = album.Year;
    song1.Comment = album.Comment;
    album.SongsList.push_back(song1);

    MasteringUtility::Song song2;
    song2.ID = 2;
    song2.Title = "Example Song 2";
    song2.Artist = album.Artist;
    song2.TrackNumber = 2;
    song2.Path = "Example Song 2.wav";
    song2.NewPath = "Example Song 2.mp3";
    song2.Codec = "libmp3lame";
    song2.Genre = album.Genre;
    song2.Year = album.Year;
    song2.Comment = album.Comment;
    album.SongsList.push_back(song2);

    albums.push_back(album);

    masterer.SaveINI(albums, outFile);

    MasteringUtility::Albums inputAlbums;
    masterer.ParseINI(outFile, inputAlbums);

    bool allOk = true;

    if (albums.size() != inputAlbums.size()) {
        std::cerr << "Album count mismatch!\n";
        allOk = false;
    }

    for (size_t i = 0; i < albums.size(); ++i) {
        const auto& orig = albums[i];
        const auto& parsed = inputAlbums[i];

        allOk &= compareStrings(std::to_string(orig.ID), std::to_string(parsed.ID), "Album ID");
        allOk &= compareStrings(orig.Title, parsed.Title, "Album Title");
        allOk &= compareStrings(orig.Artist, parsed.Artist, "Album Artist");
        allOk &= compareStrings(orig.Copyright, parsed.Copyright, "Album Copyright");
        allOk &= compareStrings(orig.AlbumArt, parsed.AlbumArt, "Album Art");
        allOk &= compareStrings(orig.Path.string(), parsed.Path.string(), "Album Path");
        allOk &= compareStrings(orig.NewPath.string(), parsed.NewPath.string(), "Album NewPath");
        allOk &= compareStrings(orig.Genre, parsed.Genre, "Album Genre");
        allOk &= compareStrings(orig.Year, parsed.Year, "Album Year");
        allOk &= compareStrings(orig.Comment, parsed.Comment, "Album Comment");

        if (orig.SongsList.size() != parsed.SongsList.size()) {
            std::cerr << "Song count mismatch in album " << orig.Title << "!\n";
            allOk = false;
        }

        for (size_t j = 0; j < orig.SongsList.size(); ++j) {
            const auto& oSong = orig.SongsList[j];
            const auto& pSong = parsed.SongsList[j];

            allOk &= compareStrings(std::to_string(oSong.ID), std::to_string(pSong.ID), "Song ID");
            allOk &= compareStrings(oSong.Title, pSong.Title, "Song Title");
            allOk &= compareStrings(oSong.Artist, pSong.Artist, "Song Artist");
            allOk &= compareStrings(std::to_string(oSong.TrackNumber), std::to_string(pSong.TrackNumber), "Song TrackNumber");
            allOk &= compareStrings(oSong.Path.string(), pSong.Path.string(), "Song Path");
            allOk &= compareStrings(oSong.NewPath.string(), pSong.NewPath.string(), "Song NewPath");
            allOk &= compareStrings(oSong.Codec, pSong.Codec, "Song Codec");
            allOk &= compareStrings(oSong.Genre, pSong.Genre, "Song Genre");
            allOk &= compareStrings(oSong.Year, pSong.Year, "Song Year");
            allOk &= compareStrings(oSong.Comment, pSong.Comment, "Song Comment");
        }
    }

    
    std::filesystem::remove_all(tempDir);

    if (allOk) {
        std::cout << "All metadata matched successfully!\n";
        return 0;
    }
    else {
        std::cerr << "Metadata mismatch detected!\n";
        return 1;
    }
}
