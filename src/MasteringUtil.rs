#[cxx::bridge]
mod ffi {
    extern "C++" {
        include!("MasteringUtil_cxx.h");

        type MasteringUtility;
        type MasteringUtilityMetadata;
        type MasteringUtilitySong;
        type MasteringUtilityAlbum;

        // Constructor
        fn new_mastering_utility() -> UniquePtr<MasteringUtility>;

        // MasteringUtility methods
        fn Master(self: &MasteringUtility, markupFile: &CxxString);
        fn ParseMarkup(self: &MasteringUtility, markupFile: &CxxString) -> Vec<UniquePtr<MasteringUtilityAlbum>>;
        fn SaveMarkup(self: &MasteringUtility, albums: &Vec<UniquePtr<MasteringUtilityAlbum>>, markupFile: &CxxString);
        fn ProcessAlbum(self: &MasteringUtility, album: &MasteringUtilityAlbum);
        fn ProcessSong(self: &MasteringUtility, song: &MasteringUtilitySong, album: &MasteringUtilityAlbum);

        // Metadata
        fn new_metadata() -> UniquePtr<MasteringUtilityMetadata>;
        fn metadata_get_id(self: &MasteringUtilityMetadata) -> i32;
        fn metadata_set_id(self: &mut MasteringUtilityMetadata, id: i32);
        fn metadata_get_title(self: &MasteringUtilityMetadata) -> &CxxString;
        fn metadata_set_title(self: &mut MasteringUtilityMetadata, title: &CxxString);
        fn metadata_get_artist(self: &MasteringUtilityMetadata) -> &CxxString;
        fn metadata_set_artist(self: &mut MasteringUtilityMetadata, artist: &CxxString);
        fn metadata_get_album(self: &MasteringUtilityMetadata) -> &CxxString;
        fn metadata_set_album(self: &mut MasteringUtilityMetadata, album: &CxxString);
        fn metadata_get_genre(self: &MasteringUtilityMetadata) -> &CxxString;
        fn metadata_set_genre(self: &mut MasteringUtilityMetadata, genre: &CxxString);
        fn metadata_get_year(self: &MasteringUtilityMetadata) -> &CxxString;
        fn metadata_set_year(self: &mut MasteringUtilityMetadata, year: &CxxString);
        fn metadata_get_comment(self: &MasteringUtilityMetadata) -> &CxxString;
        fn metadata_set_comment(self: &mut MasteringUtilityMetadata, comment: &CxxString);
        fn metadata_get_arguments(self: &MasteringUtilityMetadata) -> &CxxString;
        fn metadata_set_arguments(self: &mut MasteringUtilityMetadata, arguments: &CxxString);
        fn metadata_get_copyright(self: &MasteringUtilityMetadata) -> &CxxString;
        fn metadata_set_copyright(self: &mut MasteringUtilityMetadata, copyright: &CxxString);
        fn metadata_get_path(self: &MasteringUtilityMetadata) -> &CxxString;
        fn metadata_set_path(self: &mut MasteringUtilityMetadata, path: &CxxString);
        fn metadata_get_new_path(self: &MasteringUtilityMetadata) -> &CxxString;
        fn metadata_set_new_path(self: &mut MasteringUtilityMetadata, path: &CxxString);

        // Song (inherits Metadata)
        fn new_song() -> UniquePtr<MasteringUtilitySong>;
        fn song_get_track_number(self: &MasteringUtilitySong) -> i32;
        fn song_set_track_number(self: &mut MasteringUtilitySong, track: i32);
        fn song_get_codec(self: &MasteringUtilitySong) -> &CxxString;
        fn song_set_codec(self: &mut MasteringUtilitySong, codec: &CxxString);

        // Album (inherits Metadata)
        fn new_album() -> UniquePtr<MasteringUtilityAlbum>;
        fn album_get_album_art(self: &MasteringUtilityAlbum) -> &CxxString;
        fn album_set_album_art(self: &mut MasteringUtilityAlbum, path: &CxxString);
        fn album_get_songs(self: &MasteringUtilityAlbum) -> Vec<UniquePtr<MasteringUtilitySong>>;
        fn album_add_song(self: &mut MasteringUtilityAlbum, song: UniquePtr<MasteringUtilitySong>);
    }
}
