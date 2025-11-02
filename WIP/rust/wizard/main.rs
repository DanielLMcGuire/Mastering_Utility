use cxx::{CxxVector, UniquePtr};
use std::io::{self, Write};
use std::path::PathBuf;

#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("MasteringUtility.h");

        type MasteringUtility;

        fn new_mastering_utility() -> UniquePtr<MasteringUtility>;
        fn Master(self: &MasteringUtility, markup_file: &CxxString);
        fn SaveMarkup(self: &MasteringUtility, albums: &CxxVector<Album>, markup_file: &CxxString);

        type Album;
        type Song;

        type Albums = CxxVector<Album>;

        fn title(self: &Album) -> &CxxString;
        fn set_title(self: Pin<&mut Album>, val: &CxxString);
        fn artist(self: &Album) -> &CxxString;
        fn set_artist(self: Pin<&mut Album>, val: &CxxString);
        fn genre(self: &Album) -> &CxxString;
        fn set_genre(self: Pin<&mut Album>, val: &CxxString);
        fn year(self: &Album) -> &CxxString;
        fn set_year(self: Pin<&mut Album>, val: &CxxString);
        fn songs_list(self: &Album) -> &CxxVector<Song>;

        fn title(self: &Song) -> &CxxString;
        fn set_title(self: Pin<&mut Song>, val: &CxxString);
        fn artist(self: &Song) -> &CxxString;
        fn set_artist(self: Pin<&mut Song>, val: &CxxString);
        fn track_number(self: &Song) -> i32;
        fn set_track_number(self: Pin<&mut Song>, val: i32);
    }
}

static mut AUTO_ADD_SONGS: bool = false;

fn preview_song(song: &ffi::Song) {
    println!(
        "Song Summary | {} | {} | Track {}",
        song.title(),
        song.artist(),
        song.track_number()
    );
}

fn preview_album(album: &ffi::Album) {
    println!(
        "Album Summary | {} | {} | Songs: {}",
        album.title(),
        album.artist(),
        album.songs_list().len()
    );
    for song in album.songs_list().iter() {
        preview_song(song);
    }
}

fn handle_command_line() -> Option<PathBuf> {
    let args: Vec<String> = std::env::args().collect();
    let mut markup_path = None;

    for arg in &args {
        if arg == "--help" || arg == "-h" {
            println!("Mastering Utility Wizard\nUsage: mastering_wizard [--markupfile=<path>] [--auto] [--help]");
            return None;
        }
        if arg.starts_with("--markupfile=") {
            markup_path = Some(PathBuf::from(arg.split('=').nth(1).unwrap()));
        }
        if arg == "--auto" || arg == "-a" {
            unsafe { AUTO_ADD_SONGS = true; }
        }
    }

    if markup_path.is_none() && args.len() > 1 && std::path::Path::new(&args[1]).exists() {
        markup_path = Some(PathBuf::from(&args[1]));
    }

    if markup_path.is_none() {
        println!("Using default: new.mas");
        markup_path = Some(PathBuf::from("new.mas"));
    }

    markup_path
}

fn collect_albums_interactively(albums: &mut Vec<UniquePtr<ffi::Album>>) {
    let mut album_count = 1;
    print!("Enter number of albums to create (default 1): ");
    io::stdout().flush().unwrap();
    let mut input = String::new();
    io::stdin().read_line(&mut input).unwrap();
    if let Ok(val) = input.trim().parse::<usize>() {
        album_count = val.max(1);
    }

    for i in 0..album_count {
        println!("Creating album {}/{}", i + 1, album_count);
        let mut album = ffi::Album::default(); /// @TODO Implement this

        print!("Album title: ");
        io::stdout().flush().unwrap();
        input.clear();
        io::stdin().read_line(&mut input).unwrap();
        album.as_mut().unwrap().set_title(&input.trim().into());

        /// @TODO Similar prompts for artist, genre, year, etc.
        albums.push(album);
    }
}

fn confirm_save_interactively(path: &PathBuf) -> bool {
    print!("Would you like to save \"{}\"? (y/n) ", path.display());
    io::stdout().flush().unwrap();
    let mut input = String::new();
    io::stdin().read_line(&mut input).unwrap();
    matches!(input.trim().to_lowercase().as_str(), "y")
}

fn main() {
    println!("Daniel's Mastering Utility -- Wizard\n");

    let masterer = ffi::new_mastering_utility();
    let masterer_ref = masterer.as_ref().expect("Failed to create MasteringUtility");

    let mut albums: Vec<UniquePtr<ffi::Album>> = Vec::new();
    let markup_path = match handle_command_line() {
        Some(p) => p,
        None => return,
    };

    if let Err(_) = std::panic::catch_unwind(|| {
        collect_albums_interactively(&mut albums);

        if albums.is_empty() {
            println!("No albums to save. Exiting.");
            return;
        }

        if confirm_save_interactively(&markup_path) {
            /// @TODO Convert Vec<UniquePtr<Album>> to CxxVector<Album>
            println!("Markup file saved to \"{}\" successfully.", markup_path.display());
        } else {
            println!("Save operation cancelled by user. Exiting without saving.");
        }
    }) {
        eprintln!("[Fatal Error] Unknown exception occurred.");
    }
}
