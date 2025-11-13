#pragma once
#include "MasteringUtil.h"
#include "rust/cxx.h"
#include <memory>

class MasteringUtilWrapper {
public:
    MasteringUtilWrapper() = default;

    // Master a markup file (const because it doesn't modify wrapper state)
    void Master(rust::Str markupFile) const {
        m_util.Master(std::filesystem::path(std::string(markupFile.data(), markupFile.size())));
    }

    // Parse a markup file (non-const because it modifies m_albums)
    void ParseMarkup(rust::Str markupFile) {
        m_util.ParseMarkup(
            std::filesystem::path(std::string(markupFile.data(), markupFile.size())),
            m_albums
        );
    }

    // Save a markup file (non-const because it reads m_albums)
    void SaveMarkup(rust::Str markupFile) {
        m_util.SaveMarkup(
            m_albums,
            std::filesystem::path(std::string(markupFile.data(), markupFile.size()))
        );
    }

    // Process an album (non-const because it may modify internal state)
    void ProcessAlbum(size_t albumIndex) {
        if (albumIndex < m_albums.size()) {
            m_util.ProcessAlbum(m_albums[albumIndex]);
        }
    }

    // Process a song (non-const because it may modify internal state)
    void ProcessSong(size_t albumIndex, size_t songIndex) {
        if (albumIndex < m_albums.size() && songIndex < m_albums[albumIndex].SongsList.size()) {
            m_util.ProcessSong(
                m_albums[albumIndex].SongsList[songIndex],
                m_albums[albumIndex]
            );
        }
    }

    // Album count (non-const to match Rust signature, though could be const)
    size_t AlbumCount() { return m_albums.size(); }

    // Song count for a given album (non-const to match Rust signature, though could be const)
    size_t SongCount(size_t albumIndex) {
        if (albumIndex < m_albums.size()) return m_albums[albumIndex].SongsList.size();
        return 0;
    }

private:
    mutable MasteringUtility m_util;
    MasteringUtility::Albums m_albums;
};

// Free function factory (not a member function)
inline std::unique_ptr<MasteringUtilWrapper> create() {
    return std::make_unique<MasteringUtilWrapper>();
}
