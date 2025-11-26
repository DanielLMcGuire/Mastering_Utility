/**
 * @file MasteringUtil.h
 * @brief Library for parsing album metadata and batch audio mastering with
 * ffmpeg.
 *
 * The MasteringUtility_dll library provides functionality to parse album
 * metadata from an INI file, and use them to process audio. In addition, it
 * provides functions to save updated metadata back to an INI file.
 *
 * @author Daniel McGuire
 *
 * @code
 * album 1 ( "Album Name", "Artist", "Copyright", "cover.jpg", "./", "./new",
 * "Genre", "2025", "Comment", "Global FFMPEG Flags" )
 * {
 *     song 1 ( "Song Title", "Artist", 1, //track number  "input.wav",
 * "output.mp3", "libmp3lame", "Genre", "2025", "Comment" "FFMPEG Flags")
 * }
 * @endcode
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

#pragma once
#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
		/// @brief User Comments
		std::string Comment;
		/// @brief  Additional Arguments
		std::string arguments;
		/// @brief  Copyright info
		std::string Copyright;
		/// @brief  File path
		std::filesystem::path Path;
		/// @brief  New file path
		std::filesystem::path NewPath;

		/// @brief Equality operator for Metadata
		bool operator==(const Metadata &other) const
		{
			return ID == other.ID && Title == other.Title && Artist == other.Artist && Album == other.Album &&
			       Genre == other.Genre && Year == other.Year && arguments == other.arguments &&
			       Copyright == other.Copyright && Path == other.Path && NewPath == other.NewPath;
		}
	};

	/// @brief Song Metadata
	class Song : public Metadata
	{
	  public:
		/// @brief  Track number
		int TrackNumber{};
		/// @brief  Codec
		std::string Codec;

		/// @brief Equality operator for Song
		bool operator==(const Song &other) const
		{
			return Metadata::operator==(other) && TrackNumber == other.TrackNumber && Codec == other.Codec;
		}
	};

	/// @brief Vector of songs
	using Songs = std::vector<Song>;

	/// @brief Album Metadata
	class Album : public Metadata
	{
	  public:
		/// @brief  Album art path
		std::filesystem::path AlbumArt;
		/// @brief  List of songs
		Songs SongsList;

		std::filesystem::path markup;

		/// @brief Equality operator for Album
		bool operator==(const Album &other) const
		{
			return Metadata::operator==(other) && AlbumArt == other.AlbumArt && SongsList == other.SongsList;
		}
	};

	/// @brief Vector of albums
	using Albums = std::vector<Album>;

	/**
	 * @brief Masterer
	 *
	 * Parses a Markup File, then processes songs using FFMPEG
	 * @param markupFile File to parse
	 */
	void Master(const std::filesystem::path &markupFile);

	/**
	 * @brief Parse a Markup File
	 *
	 * @param[in] markupFile Path to Markup file
	 * @param[out] albums Vector of albums
	 */
	void ParseMarkup(const std::filesystem::path &markupFile, Albums &albums);

	/**
	 * @brief Save a Markup File
	 *
	 *
	 * @param markupFile Path to Markup File to save
	 * @param albums Vector of albums to save
	 */
	void SaveMarkup(const Albums &albums, const std::filesystem::path &markupFile);

	/**
	 * @brief Process an album
	 *
	 * Processes an album using ProcessSong()
	 * @param album Album to process
	 */
	void ProcessAlbum(const Album &album);

	/**
	 * @brief Process a song
	 *
	 * Processes a song using FFMPEG
	 * @param song Song to process
	 * @param album Parent album of song
	 */
	void ProcessSong(const Song &song, const Album &album);

	/// @brief Song Cache Entry
	class SongCacheEntry
	{
	  public:
		/// @brief Song ID (without album prefix)
		std::string SongID;
		/// @brief Input File Path
		std::filesystem::path Path;
		/// @brief Hash of the input file
		std::string Hash;

		/// @brief Equality operator for SongCacheEntry
		bool operator==(const SongCacheEntry &other) const
		{
			return SongID == other.SongID && Path == other.Path && Hash == other.Hash;
		}
	};

	/// @brief Album Cache Entry
	class AlbumCacheEntry
	{
	  public:
		/// @brief Hash of the markup file
		std::string MarkupHash;
		/// @brief Song cache entries
		std::vector<SongCacheEntry> Songs;
	};

  private:
	/// @brief Cache of processed albums: AlbumID -> AlbumCacheEntry
	using AlbumCacheMap = std::unordered_map<int, AlbumCacheEntry>;

	/// @brief Get the cache file path
	std::filesystem::path getCacheFilePath(const Album &album) const;

	/// @brief Load the cache for an album
	void loadCache(const Album &album);
	/// @brief Save the cache for an album
	void saveCache(const Album &album) const;

	/// @brief Cache of processed albums: AlbumID -> AlbumCacheEntry
	AlbumCacheMap m_albumCaches;
	/// @brief Set of audio codecs
	std::unordered_set<std::string> m_audioCodecs;
	/// @brief Markup file path
	std::filesystem::path m_markupFile;
	/// @brief Buffer size
	static constexpr size_t BUFFER_SIZE = 4096;
};