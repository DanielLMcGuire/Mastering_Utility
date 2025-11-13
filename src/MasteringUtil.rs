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
    }
}