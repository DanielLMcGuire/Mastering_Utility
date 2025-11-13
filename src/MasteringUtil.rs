#[cxx::bridge]
pub mod ffi {
    unsafe extern "C++" {
        include!("MasteringUtil_cxx.h");

        type MasteringUtilWrapper;

        fn create() -> UniquePtr<MasteringUtilWrapper>;

        fn Master(self: &MasteringUtilWrapper, markupFile: &str);
        fn ParseMarkup(self: Pin<&mut MasteringUtilWrapper>, markupFile: &str);
        fn SaveMarkup(self: Pin<&mut MasteringUtilWrapper>, markupFile: &str);

        fn ProcessAlbum(self: Pin<&mut MasteringUtilWrapper>, index: usize);
        fn ProcessSong(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize);

        fn AlbumCount(self: Pin<&mut MasteringUtilWrapper>) -> usize;
        fn SongCount(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize) -> usize;

        // ========== ALBUM GETTERS ==========
        fn GetAlbumID(self: &MasteringUtilWrapper, albumIndex: usize) -> i32;
        fn GetAlbumTitle(self: &MasteringUtilWrapper, albumIndex: usize) -> String;
        fn GetAlbumArtist(self: &MasteringUtilWrapper, albumIndex: usize) -> String;
        fn GetAlbumGenre(self: &MasteringUtilWrapper, albumIndex: usize) -> String;
        fn GetAlbumYear(self: &MasteringUtilWrapper, albumIndex: usize) -> String;
        fn GetAlbumComment(self: &MasteringUtilWrapper, albumIndex: usize) -> String;
        fn GetAlbumArguments(self: &MasteringUtilWrapper, albumIndex: usize) -> String;
        fn GetAlbumCopyright(self: &MasteringUtilWrapper, albumIndex: usize) -> String;
        fn GetAlbumPath(self: &MasteringUtilWrapper, albumIndex: usize) -> String;
        fn GetAlbumNewPath(self: &MasteringUtilWrapper, albumIndex: usize) -> String;
        fn GetAlbumAlbumArt(self: &MasteringUtilWrapper, albumIndex: usize) -> String;

        // ========== ALBUM SETTERS ==========
        fn SetAlbumID(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, id: i32);
        fn SetAlbumTitle(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, title: &str);
        fn SetAlbumArtist(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, artist: &str);
        fn SetAlbumGenre(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, genre: &str);
        fn SetAlbumYear(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, year: &str);
        fn SetAlbumComment(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, comment: &str);
        fn SetAlbumArguments(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, arguments: &str);
        fn SetAlbumCopyright(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, copyright: &str);
        fn SetAlbumPath(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, path: &str);
        fn SetAlbumNewPath(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, newPath: &str);
        fn SetAlbumAlbumArt(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, albumArt: &str);

        // ========== SONG GETTERS ==========
        fn GetSongID(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> i32;
        fn GetSongTitle(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> String;
        fn GetSongArtist(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> String;
        fn GetSongAlbum(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> String;
        fn GetSongGenre(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> String;
        fn GetSongYear(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> String;
        fn GetSongComment(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> String;
        fn GetSongArguments(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> String;
        fn GetSongCopyright(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> String;
        fn GetSongPath(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> String;
        fn GetSongNewPath(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> String;
        fn GetSongCodec(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> String;
        fn GetSongTrackNumber(self: &MasteringUtilWrapper, albumIndex: usize, songIndex: usize) -> i32;

        // ========== SONG SETTERS ==========
        fn SetSongID(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, id: i32);
        fn SetSongTitle(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, title: &str);
        fn SetSongArtist(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, artist: &str);
        fn SetSongAlbum(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, album: &str);
        fn SetSongGenre(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, genre: &str);
        fn SetSongYear(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, year: &str);
        fn SetSongComment(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, comment: &str);
        fn SetSongArguments(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, arguments: &str);
        fn SetSongCopyright(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, copyright: &str);
        fn SetSongPath(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, path: &str);
        fn SetSongNewPath(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, newPath: &str);
        fn SetSongCodec(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, codec: &str);
        fn SetSongTrackNumber(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize, trackNumber: i32);

        // ========== ADD/REMOVE METHODS ==========
        fn AddAlbum(self: Pin<&mut MasteringUtilWrapper>);
        fn AddSong(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize);
        fn RemoveAlbum(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize);
        fn RemoveSong(self: Pin<&mut MasteringUtilWrapper>, albumIndex: usize, songIndex: usize);
    }
}