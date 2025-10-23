/**
 * @file MasteringUtil.h
 * @brief Library for parsing album metadata and batch audio mastering with ffmpeg.
 *
 * The MasteringUtility_dll library provides functionality to parse album metadata from an INI file,
 * and use them to process audio. In addition, it provides functions to save updated metadata back to an INI file.
 *
 * Example INI metadata format:
 * @code
 * album 1 ( "Album Name", "Artist", "Copyright", "cover.jpg", "./", "./new", "Genre", "2025", "Comment" )
 * {
 *     song 1 ( "Song Title", "Artist", 1, //track number  "input.wav", "output.mp3", "libmp3lame", "Genre", "2025", "Comment" )
 * }
 * @endcode
 */

#pragma once
#include <string>
#include <vector>

/// @brief  Mastering Utility
class MasteringUtility
{
public:
    /// @brief Generic Metadata
    class Metadata
    {
    public:

        int ID{};
        std::string Title;
        std::string Artist;
        std::string Album;
        std::string Genre;
        std::string Year;
        std::string Comment;
        std::string Copyright;
        std::string Path;
        std::string NewPath;
    };
    /// @brief Song Metadata
    class Song : public Metadata
    {
    public:
        int TrackNumber{};
        int SortOrder{};
        std::string Codec;
    };

    /// @brief Vector of song
    using Songs = std::vector<Song>;

    /// @brief Album Metadata
    class Album : public Metadata
    {
    public:
        std::string AlbumArt;
        Songs SongsList;
    };

    /// @brief Vector of album
    using Albums = std::vector<Album>;
    
    /**
     *  @brief Masterer
     *  @param albumINI Path to ini file
     */
    void Master(const std::string& albumINI);

    /**
     * @brief INI Parser
     * 
     * @param albumINI Path to ini file
     * @param albums Empty vector of Albums to be returned
     */
    void ParseINI(const std::string& albumINI, Albums& albums);
    
    /**
     * @brief Save albums to an INI-like file
     * @param albums Vector of albums to save
     * @param iniFile Path to output INI file
     */
    void SaveINI(const Albums& albums, const std::string& iniFile);

    /**
     * @brief Processes albums
     * 
     * @param album Album
     * @param iniFolder Path to ini folder
     */
    void ProcessAlbum(const Album& album, const std::string& iniFolder);

    /**
     * @brief Processes songs
     *
	 * @param song Song
	 * @param iniFolder Path to ini folder
     */
    void ProcessSong(const Song& song, const std::string& iniFolder);
};
