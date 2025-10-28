/**
 * @file MasteringUtil.h
 * @brief Library for parsing album metadata and batch audio mastering with ffmpeg.
 *
 * The MasteringUtility_dll library provides functionality to parse album metadata from an INI file,
 * and use them to process audio. In addition, it provides functions to save updated metadata back to an INI file.
 *
 * @author Daniel McGuire
 * Example INI metadata format:
 * @code
 * album 1 ( "Album Name", "Artist", "Copyright", "cover.jpg", "./", "./new", "Genre", "2025", "Comment" )
 * {
 *     song 1 ( "Song Title", "Artist", 1, //track number  "input.wav", "output.mp3", "libmp3lame", "Genre", "2025", "Comment" )
 * }
 * @endcode
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

#pragma once
#include <filesystem>
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
		std::filesystem::path Path;
		/// @brief  New file path
		std::filesystem::path NewPath;

		/// @brief Equality operator for Metadata
		bool operator==(const Metadata& other) const {
			return ID == other.ID &&
				Title == other.Title &&
				Artist == other.Artist &&
				Album == other.Album &&
				Genre == other.Genre &&
				Year == other.Year &&
				Comment == other.Comment &&
				Copyright == other.Copyright &&
				Path == other.Path &&
				NewPath == other.NewPath;
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
		bool operator==(const Song& other) const {
			return Metadata::operator==(other) &&
				TrackNumber == other.TrackNumber &&
				Codec == other.Codec;
		}
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

		/// @brief Equality operator for Album
		bool operator==(const Album& other) const {
			return Metadata::operator==(other) &&
				AlbumArt == other.AlbumArt &&
				SongsList == other.SongsList; 
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
	void Master(const std::filesystem::path& markupFile);

	/**
	 * @brief Parse a Markup File
	 *
	 * @param[in] markupFile Path to Markup file
	 * @param[out] albums Vector of albums
	 */
	void ParseMarkup(const std::filesystem::path& markupFile, Albums& albums);

	/**
	 * @brief Save a Markup File
	 *
	 *
	 * @param markupFile Path to Markup File to save
	 * @param albums Vector of albums to save
	 */
	void SaveMarkup(const Albums& albums, const std::filesystem::path& markupFile);


	/**
	 * @brief Process an album
	 *
	 * Processes an album using ProcessSong()
	 * @param album Album to process
	 */
	void ProcessAlbum(const Album& album);

	/**
	 * @brief Process a song
	 *
	 * Processes a song using FFMPEG
	 * @param song Song to process
	 * @param destFolder album.NewPath of Songs parent album
	 */
	void ProcessSong(const Song& song, const std::filesystem::path& destFolder);
};
