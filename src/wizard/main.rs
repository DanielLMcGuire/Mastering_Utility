use std::io::{self, Write};
use std::path::PathBuf;
use std::process::exit;

mod MasteringUtil; // cxx::bridge module
use MasteringUtil::ffi::*;

static mut AUTO_ADD_SONGS: bool = false;

fn cleanup(code: i32) -> ! {
    if code != 0 {
        println!("Exited with code {}", code);
    }
    std::process::exit(code);
}

/// Preview helpers
mod preview {
    use super::*;
    
    pub fn song(song: &MasteringUtilitySong, display_info: bool) {
        if display_info {
            println!("\n=== Song Summary ===");
        }
        println!(
            "Title: {} | Artist: {} | Album: {} | Track: {}",
            song_get_title(song),
            song_get_artist(song),
            song_get_album(song),
            song_get_track_number(song)
        );
        if display_info {
            println!("-------------------------");
        }
    }

    pub fn album(album: &MasteringUtilityAlbum) {
        println!("\n=== Album Summary ===");
        println!(
            "Album: {} | Artist: {} | Genre: {} | Year: {} | Songs: {}",
            album_get_title(album),
            album_get_artist(album),
            album_get_genre(album),
            album_get_year(album),
            album_get_songs(album).len()
        );
        for song in album_get_songs(album) {
            song(&song, false);
        }
        println!("-------------------------");
    }
}

/// Prompt helper
fn prompt_string(prompt: &str, default: Option<&str>) -> String {
    print!("{} [{}]: ", prompt, default.unwrap_or(""));
    io::stdout().flush().unwrap();
    let mut input = String::new();
    io::stdin().read_line(&mut input).unwrap();
    let input = input.trim();
    if input.is_empty() {
        default.unwrap_or("").to_string()
    } else {
        input.to_string()
    }
}

fn prompt_int(prompt: &str, default: Option<i32>) -> i32 {
    loop {
        let input = prompt_string(prompt, default.map(|v| &v.to_string()));
        match input.parse::<i32>() {
            Ok(n) => return n,
            Err(_) => println!("Please enter a valid integer."),
        }
    }
}

/// Handle command line arguments
fn handle_command_line() -> Option<PathBuf> {
    let args: Vec<String> = std::env::args().collect();
    if args.iter().any(|a| a == "--help" || a == "-h") {
        println!("Mastering Utility Wizard\nUsage: mastering_wizard [--markupfile=<path>] [--auto] [--help]");
        return None;
    }

    unsafe {
        if args.iter().any(|a| a == "--auto" || a == "-a") {
            AUTO_ADD_SONGS = true;
        }
    }

    for arg in &args[1..] {
        if arg.starts_with("--markupfile=") {
            return Some(PathBuf::from(&arg[13..]));
        }
    }

    if args.len() > 1 && std::path::Path::new(&args[1]).exists() {
        Some(PathBuf::from(&args[1]))
    } else {
        println!("Using default: new.mas");
        Some(PathBuf::from("new.mas"))
    }
}

/// Collect albums interactively
fn collect_albums_interactively(albums: &mut Vec<Box<MasteringUtilityAlbum>>) {
    let initial_album_count = albums.len() as i32;
    let mut album_count = initial_album_count;

    if initial_album_count > 0 {
        println!("Markup file contains {} album(s).", initial_album_count);
        album_count = prompt_int("Enter total number of albums to process", Some(initial_album_count));
    } else {
        album_count = prompt_int("Enter number of albums to create", None);
    }

    let default_path = ".";
    let default_comment = " ";
    let default_copyright = " ";
    let default_args = " ";
    let default_codec = "copy";

    // Edit existing albums
    for i in 0..initial_album_count as usize {
        let album = &mut albums[i];
        preview::album(album);

        if album_get_id(album) == 0 {
            album_set_id(album, (i + 1) as i32);
        }

        album_set_title(album, &prompt_string("Enter Album Title", Some(&album_get_title(album))));
        album_set_artist(album, &prompt_string("Enter Album Artist", Some(&album_get_artist(album))));
        album_set_genre(album, &prompt_string("Enter Album Genre", Some(&album_get_genre(album))));
        album_set_year(album, &prompt_string("Enter Album Year", Some(&album_get_year(album))));
        album_set_copyright(album, &prompt_string("Enter Album Copyright Info", Some(&album_get_copyright(album))));
        album_set_new_path(album, &prompt_string("Enter Relative Path to save songs", Some(default_path)));
        album_set_album_art(album, &prompt_string("Enter Relative Path to Album Art", Some(default_path)));
        album_set_arguments(album, &prompt_string("Enter additional arguments for FFMPEG (if any)", Some(default_args)));

        // Songs editing / adding
        let mut songs = album_get_songs(album);
        let initial_song_count = songs.len() as i32;
        let mut song_count = prompt_int(&format!("Enter total number of songs in {}", album_get_title(album)), Some(initial_song_count));

        for j in 0..song_count as usize {
            if j < initial_song_count as usize {
                let song = &mut songs[j];
                preview::song(song, false);
                if song_get_id(song) == 0 { /* set ID */ }
            } else {
                // Add new song
                let mut song = new_song();
                song_set_track_number(&mut song, j as i32 + 1);
                song_set_album(&mut song, &album_get_title(album));
                song_set_title(&mut song, &prompt_string("Enter Song Title", None));
                song_set_artist(&mut song, &prompt_string("Enter Song Artist", Some(&album_get_artist(album))));
                song_set_genre(&mut song, &prompt_string("Enter Song Genre", Some(&album_get_genre(album))));
                song_set_year(&mut song, &prompt_string("Enter Song Year", Some(&album_get_year(album))));
                song_set_new_path(&mut song, &prompt_string("Enter New Filename", Some(default_path)));
                song_set_codec(&mut song, &prompt_string("Enter Song Codec (mp3, flac, etc.)", Some(default_codec)));
                album_add_song(album, song);
            }
        }

        preview::album(album);
    }

    // Add new albums
    for i in initial_album_count..album_count {
        println!("=== Adding New Album {} of {} ===", i + 1, album_count);
        let mut album = new_album();
        album_set_id(&mut album, i + 1);
        album_set_comment(&mut album, default_comment);

        album_set_title(&mut album, &prompt_string("Enter Album Title", None));
        album_set_artist(&mut album, &prompt_string("Enter Album Artist", None));
        album_set_genre(&mut album, &prompt_string("Enter Album Genre", None));
        album_set_year(&mut album, &prompt_string("Enter Album Year", None));
        album_set_copyright(&mut album, &prompt_string("Enter Album Copyright Info", Some(default_copyright)));
        album_set_new_path(&mut album, &prompt_string("Enter Relative Path to save songs", Some(default_path)));
        album_set_album_art(&mut album, &prompt_string("Enter Relative Path to Album Art", Some(default_path)));
        album_set_arguments(&mut album, &prompt_string("Enter additional arguments for FFMPEG (if any)", Some(default_args)));

        let song_count = prompt_int(&format!("How many songs in {}", album_get_title(&album)), Some(0));
        for j in 0..song_count {
            let mut song = new_song();
            song_set_track_number(&mut song, j + 1);
            song_set_album(&mut song, &album_get_title(&album));
            song_set_title(&mut song, &prompt_string("Enter Song Title", None));
            song_set_artist(&mut song, &prompt_string("Enter Song Artist", Some(&album_get_artist(&album))));
            album_add_song(&mut album, song);
        }

        albums.push(album);
        preview::album(albums.last().unwrap());
    }
}

/// Confirm save
fn confirm_save_interactively(markup_path: &PathBuf) -> bool {
    let input = prompt_string(&format!("Would you like to save \"{}\"? (y/n)", markup_path.display()), None);
    input.to_lowercase() == "y"
}

/// Save markup file
fn save_markup_file(masterer: &MasteringUtility, albums: &Vec<Box<MasteringUtilityAlbum>>, markup_path: &PathBuf) {
    masterer.SaveMarkup(albums, markup_path.to_str().unwrap());
    println!("Markup file saved to \"{}\" successfully.", markup_path.display());
}

fn main() {
    println!("Daniel's Mastering Utility -- Wizard\n");

    let masterer = new_mastering_utility();
    let mut albums: Vec<Box<MasteringUtilityAlbum>> = Vec::new();
    let markup_path = match handle_command_line() {
        Some(p) => p,
        None => cleanup(0),
    };

    if markup_path.exists() {
        albums = masterer.ParseMarkup(markup_path.to_str().unwrap());
    }

    collect_albums_interactively(&mut albums);

    if albums.is_empty() {
        println!("No albums to save. Exiting.");
        cleanup(0);
    }

    if confirm_save_interactively(&markup_path) {
        save_markup_file(&masterer, &albums, &markup_path);
    } else {
        println!("Save operation cancelled by user. Exiting without saving.");
    }

    cleanup(0);
}
