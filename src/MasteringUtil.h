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
#include <filesystem>

/// @brief  Mastering Utility
class MasteringUtility
{
public:
    /// @brief Generic Metadata
    class Metadata
    {
    public:
        /// @brief Numeric ID
        int ID{};
        /// @brief  Title
        std::string Title;
        /// @brief  Artist
        std::string Artist;
        /// @brief  Album
        std::string Album;
        /// @brief  Genre
        std::string Genre;
        /// @brief  Release year
        std::string Year;
        /// @brief  User comment
        std::string Comment;
        /// @brief  Copyright info
        std::string Copyright;
        /// @brief  File path
        std::string Path;
        /// @brief  New file path
        std::string NewPath;
    };
    /// @brief Song Metadata
    class Song : public Metadata
    {
    public:
        /// @brief  Track number
        int TrackNumber{};
        /// @brief  Codec
        std::string Codec;
    };

    /// @brief Vector of songs
    using Songs = std::vector<Song>;

    /// @brief Album Metadata
    class Album : public Metadata
    {
    public:
        /// @brief  Album art path
        std::string AlbumArt;
        /// @brief  List of songs
        Songs SongsList;
    };

    /// @brief Vector of albums
    using Albums = std::vector<Album>;
    
    /**
     *  @brief Masterer
     *  @param albumINI Path to ini file
     */
    void Master(const std::filesystem::path& albumINI);

    /**
     * @brief INI Parser
     * 
     * @param albumINI Path to ini file
     * @param albums Empty vector of Albums to be returned
     */
    void ParseINI(const std::filesystem::path& albumINI, Albums& albums);
    
    /**
     * @brief Save albums to an INI-like file
     * @param albums Vector of albums to save
     * @param iniFile Path to output INI file
     */
    void SaveINI(const Albums& albums, const std::filesystem::path& iniFile);

    /**
     * @brief Processes albums
     * 
     * @param album Album
     * @param newFolder Path to new folder
     */
    void ProcessAlbum(const Album& album);

    /**
     * @brief Processes songs
     *
	 * @param song Song
	 * @param newFolder Path to new folder
     */
    void ProcessSong(const Song& song, const std::filesystem::path& newFolder);
};
