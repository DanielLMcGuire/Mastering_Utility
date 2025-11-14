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
#include "MasteringUtil.h"
#include "rust/cxx.h"
#include <memory>

class MasteringUtilWrapper {
public:
    MasteringUtilWrapper() = default;

    static std::unique_ptr<MasteringUtilWrapper> create() {
        return std::make_unique<MasteringUtilWrapper>();
    }

    void Master(rust::Str markupFile) const {
        m_util.Master(std::filesystem::path(std::string(markupFile.data(), markupFile.size())));
    }

    void ParseMarkup(rust::Str markupFile) {
        m_util.ParseMarkup(
            std::filesystem::path(std::string(markupFile.data(), markupFile.size())),
            m_albums
        );
    }

    void SaveMarkup(rust::Str markupFile) {
        m_util.SaveMarkup(
            m_albums,
            std::filesystem::path(std::string(markupFile.data(), markupFile.size()))
        );
    }

    void ProcessAlbum(size_t albumIndex) {
        if (albumIndex < m_albums.size()) {
            m_util.ProcessAlbum(m_albums[albumIndex]);
        }
    }

    void ProcessSong(size_t albumIndex, size_t songIndex) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_util.ProcessSong(
                m_albums[albumIndex].SongsList[songIndex],
                m_albums[albumIndex]
            );
        }
    }

    size_t AlbumCount() { return m_albums.size(); }

    size_t SongCount(size_t albumIndex) {
        if (albumIndex < m_albums.size()) return m_albums[albumIndex].SongsList.size();
        return 0;
    }

    
    int GetAlbumID(size_t albumIndex) const {
        if (albumIndex < m_albums.size()) return m_albums[albumIndex].ID;
        return 0;
    }

    rust::String GetAlbumTitle(size_t albumIndex) const {
        if (albumIndex < m_albums.size()) return rust::String(m_albums[albumIndex].Title);
        return rust::String("");
    }

    rust::String GetAlbumArtist(size_t albumIndex) const {
        if (albumIndex < m_albums.size()) return rust::String(m_albums[albumIndex].Artist);
        return rust::String("");
    }

    rust::String GetAlbumGenre(size_t albumIndex) const {
        if (albumIndex < m_albums.size()) return rust::String(m_albums[albumIndex].Genre);
        return rust::String("");
    }

    rust::String GetAlbumYear(size_t albumIndex) const {
        if (albumIndex < m_albums.size()) return rust::String(m_albums[albumIndex].Year);
        return rust::String("");
    }

    rust::String GetAlbumComment(size_t albumIndex) const {
        if (albumIndex < m_albums.size()) return rust::String(m_albums[albumIndex].Comment);
        return rust::String("");
    }

    rust::String GetAlbumArguments(size_t albumIndex) const {
        if (albumIndex < m_albums.size()) return rust::String(m_albums[albumIndex].arguments);
        return rust::String("");
    }

    rust::String GetAlbumCopyright(size_t albumIndex) const {
        if (albumIndex < m_albums.size()) return rust::String(m_albums[albumIndex].Copyright);
        return rust::String("");
    }

    rust::String GetAlbumPath(size_t albumIndex) const {
        if (albumIndex < m_albums.size()) return rust::String(m_albums[albumIndex].Path.string());
        return rust::String("");
    }

    rust::String GetAlbumNewPath(size_t albumIndex) const {
        if (albumIndex < m_albums.size()) return rust::String(m_albums[albumIndex].NewPath.string());
        return rust::String("");
    }

    rust::String GetAlbumAlbumArt(size_t albumIndex) const {
        if (albumIndex < m_albums.size()) return rust::String(m_albums[albumIndex].AlbumArt.string());
        return rust::String("");
    }

    void SetAlbumID(size_t albumIndex, int id) {
        if (albumIndex < m_albums.size()) m_albums[albumIndex].ID = id;
    }

    void SetAlbumTitle(size_t albumIndex, rust::Str title) {
        if (albumIndex < m_albums.size()) {
            m_albums[albumIndex].Title = std::string(title.data(), title.size());
        }
    }

    void SetAlbumArtist(size_t albumIndex, rust::Str artist) {
        if (albumIndex < m_albums.size()) {
            m_albums[albumIndex].Artist = std::string(artist.data(), artist.size());
        }
    }

    void SetAlbumGenre(size_t albumIndex, rust::Str genre) {
        if (albumIndex < m_albums.size()) {
            m_albums[albumIndex].Genre = std::string(genre.data(), genre.size());
        }
    }

    void SetAlbumYear(size_t albumIndex, rust::Str year) {
        if (albumIndex < m_albums.size()) {
            m_albums[albumIndex].Year = std::string(year.data(), year.size());
        }
    }

    void SetAlbumComment(size_t albumIndex, rust::Str comment) {
        if (albumIndex < m_albums.size()) {
            m_albums[albumIndex].Comment = std::string(comment.data(), comment.size());
        }
    }

    void SetAlbumArguments(size_t albumIndex, rust::Str arguments) {
        if (albumIndex < m_albums.size()) {
            m_albums[albumIndex].arguments = std::string(arguments.data(), arguments.size());
        }
    }

    void SetAlbumCopyright(size_t albumIndex, rust::Str copyright) {
        if (albumIndex < m_albums.size()) {
            m_albums[albumIndex].Copyright = std::string(copyright.data(), copyright.size());
        }
    }

    void SetAlbumPath(size_t albumIndex, rust::Str path) {
        if (albumIndex < m_albums.size()) {
            m_albums[albumIndex].Path = std::filesystem::path(std::string(path.data(), path.size()));
        }
    }

    void SetAlbumNewPath(size_t albumIndex, rust::Str newPath) {
        if (albumIndex < m_albums.size()) {
            m_albums[albumIndex].NewPath = std::filesystem::path(std::string(newPath.data(), newPath.size()));
        }
    }

    void SetAlbumAlbumArt(size_t albumIndex, rust::Str albumArt) {
        if (albumIndex < m_albums.size()) {
            m_albums[albumIndex].AlbumArt = std::filesystem::path(std::string(albumArt.data(), albumArt.size()));
        }
    }

    int GetSongID(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return m_albums[albumIndex].SongsList[songIndex].ID;
        }
        return 0;
    }

    rust::String GetSongTitle(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return rust::String(m_albums[albumIndex].SongsList[songIndex].Title);
        }
        return rust::String("");
    }

    rust::String GetSongArtist(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return rust::String(m_albums[albumIndex].SongsList[songIndex].Artist);
        }
        return rust::String("");
    }

    rust::String GetSongAlbum(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return rust::String(m_albums[albumIndex].SongsList[songIndex].Album);
        }
        return rust::String("");
    }

    rust::String GetSongGenre(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return rust::String(m_albums[albumIndex].SongsList[songIndex].Genre);
        }
        return rust::String("");
    }

    rust::String GetSongYear(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return rust::String(m_albums[albumIndex].SongsList[songIndex].Year);
        }
        return rust::String("");
    }

    rust::String GetSongComment(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return rust::String(m_albums[albumIndex].SongsList[songIndex].Comment);
        }
        return rust::String("");
    }

    rust::String GetSongArguments(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return rust::String(m_albums[albumIndex].SongsList[songIndex].arguments);
        }
        return rust::String("");
    }

    rust::String GetSongCopyright(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return rust::String(m_albums[albumIndex].SongsList[songIndex].Copyright);
        }
        return rust::String("");
    }

    rust::String GetSongPath(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return rust::String(m_albums[albumIndex].SongsList[songIndex].Path.string());
        }
        return rust::String("");
    }

    rust::String GetSongNewPath(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return rust::String(m_albums[albumIndex].SongsList[songIndex].NewPath.string());
        }
        return rust::String("");
    }

    rust::String GetSongCodec(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return rust::String(m_albums[albumIndex].SongsList[songIndex].Codec);
        }
        return rust::String("");
    }

    int GetSongTrackNumber(size_t albumIndex, size_t songIndex) const {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            return m_albums[albumIndex].SongsList[songIndex].TrackNumber;
        }
        return 0;
    }

    void SetSongID(size_t albumIndex, size_t songIndex, int id) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].ID = id;
        }
    }

    void SetSongTitle(size_t albumIndex, size_t songIndex, rust::Str title) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].Title = std::string(title.data(), title.size());
        }
    }

    void SetSongArtist(size_t albumIndex, size_t songIndex, rust::Str artist) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].Artist = std::string(artist.data(), artist.size());
        }
    }

    void SetSongAlbum(size_t albumIndex, size_t songIndex, rust::Str album) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].Album = std::string(album.data(), album.size());
        }
    }

    void SetSongGenre(size_t albumIndex, size_t songIndex, rust::Str genre) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].Genre = std::string(genre.data(), genre.size());
        }
    }

    void SetSongYear(size_t albumIndex, size_t songIndex, rust::Str year) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].Year = std::string(year.data(), year.size());
        }
    }

    void SetSongComment(size_t albumIndex, size_t songIndex, rust::Str comment) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].Comment = std::string(comment.data(), comment.size());
        }
    }

    void SetSongArguments(size_t albumIndex, size_t songIndex, rust::Str arguments) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].arguments = std::string(arguments.data(), arguments.size());
        }
    }

    void SetSongCopyright(size_t albumIndex, size_t songIndex, rust::Str copyright) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].Copyright = std::string(copyright.data(), copyright.size());
        }
    }

    void SetSongPath(size_t albumIndex, size_t songIndex, rust::Str path) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].Path = 
                std::filesystem::path(std::string(path.data(), path.size()));
        }
    }

    void SetSongNewPath(size_t albumIndex, size_t songIndex, rust::Str newPath) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].NewPath = 
                std::filesystem::path(std::string(newPath.data(), newPath.size()));
        }
    }

    void SetSongCodec(size_t albumIndex, size_t songIndex, rust::Str codec) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].Codec = std::string(codec.data(), codec.size());
        }
    }

    void SetSongTrackNumber(size_t albumIndex, size_t songIndex, int trackNumber) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList[songIndex].TrackNumber = trackNumber;
        }
    }

    void AddAlbum() {
        MasteringUtility::Album album;
        album.ID = static_cast<int>(m_albums.size()) + 1;
        m_albums.push_back(album);
    }

    void AddSong(size_t albumIndex) {
        if (albumIndex < m_albums.size()) {
            MasteringUtility::Song song;
            song.ID = static_cast<int>(m_albums[albumIndex].SongsList.size()) + 1;
            song.TrackNumber = song.ID;
            m_albums[albumIndex].SongsList.push_back(song);
        }
    }

    void RemoveAlbum(size_t albumIndex) {
        if (albumIndex < m_albums.size()) {
            m_albums.erase(m_albums.begin() + albumIndex);
        }
    }

    void RemoveSong(size_t albumIndex, size_t songIndex) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_albums[albumIndex].SongsList.erase(m_albums[albumIndex].SongsList.begin() + songIndex);
        }
    }

private:
    mutable MasteringUtility m_util;
    MasteringUtility::Albums m_albums;
};

inline std::unique_ptr<MasteringUtilWrapper> create() {
    return std::make_unique<MasteringUtilWrapper>();
}