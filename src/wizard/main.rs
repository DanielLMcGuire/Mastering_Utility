use masteringutil::ffi::{self, MasteringUtilWrapper};
use std::env;
use std::io::{self, Write};
use std::path::{Path, PathBuf};
use cxx::UniquePtr;

static mut AUTO_ADD_SONGS: bool = false;

fn prompt_string(message: &str, default_value: Option<&str>) -> String {
    print!("{}", message);
    if let Some(default) = default_value {
        print!(" (Default: {})", default);
    }
    print!(": ");
    io::stdout().flush().unwrap();

    let mut input = String::new();
    io::stdin().read_line(&mut input).expect("Failed to read line");
    let input = input.trim();

    if input.is_empty() {
        if let Some(default) = default_value {
            default.to_string()
        } else {
            // @todo Loop
            input.to_string()
        }
    } else {
        input.to_string()
    }
}

fn prompt_usize(message: &str, default_value: Option<usize>) -> usize {
    loop {
        let input_str = prompt_string(message, default_value.map(|d| d.to_string()).as_deref());
        if input_str.is_empty() {
            if let Some(default) = default_value {
                return default;
            }
        }
        match input_str.parse::<usize>() {
            Ok(n) => return n,
            Err(_) => {
                eprintln!("Invalid input. Must be a positive integer.");
            }
        }
    }
}

fn prompt_char(message: &str, default_value: Option<char>) -> char {
    loop {
        print!("{}", message);
        if let Some(default) = default_value {
            print!(" (Default: {})", default);
        }
        print!(": ");
        io::stdout().flush().unwrap();

        let mut input = String::new();
        io::stdin().read_line(&mut input).expect("Failed to read line");
        
        let trimmed_input = input.trim();

        if trimmed_input.is_empty() {
            if let Some(default) = default_value {
                return default;
            }
        }

        if let Some(c) = trimmed_input.chars().next() {
            return c.to_ascii_lowercase();
        } else if let Some(default) = default_value {
            return default.to_ascii_lowercase();
        }
    }
}


/// Display a summary of a Song
fn preview_song(wrapper: &UniquePtr<MasteringUtilWrapper>, album_index: usize, song_index: usize, display_info: bool) {
    if display_info {
        println!("\n=== Song Summary ===");
    }
    let title = wrapper.GetSongTitle(album_index, song_index);
    let artist = wrapper.GetSongArtist(album_index, song_index);
    let album = wrapper.GetSongAlbum(album_index, song_index);
    let track = wrapper.GetSongTrackNumber(album_index, song_index);
    println!(
        "Title: {} | Artist: {} | Album: {} | Track: {}",
        title, artist, album, track
    );
    if display_info {
        println!("-------------------------");
    }
}

/// Display a summary of an Album
fn preview_album(wrapper: &mut UniquePtr<MasteringUtilWrapper>, album_index: usize) {
    println!("\n=== Album Summary ===");
    let title = wrapper.GetAlbumTitle(album_index);
    let artist = wrapper.GetAlbumArtist(album_index);
    let genre = wrapper.GetAlbumGenre(album_index);
    let year = wrapper.GetAlbumYear(album_index);
    
    let song_count = wrapper.pin_mut().SongCount(album_index); 
    
    println!(
        "Album: {} | Artist: {} | Genre: {} | Year: {} | Songs: {}",
        title, artist, genre, year, song_count
    );
    for j in 0..song_count {
        preview_song(&wrapper, album_index, j, false);
    }
    println!("-------------------------\n");
}

/// Parses command-line arguments and handles the help flag.
fn handle_command_line(args: &[String], _wrapper: &mut UniquePtr<MasteringUtilWrapper>) -> Option<PathBuf> {
    let mut markup_path = PathBuf::from("new.mas");
    let mut auto_flag = false;
    let mut help_flag = false;

    let mut markup_file_arg: Option<String> = None;

    for arg in args.iter().skip(1) {
        if arg == "--help" || arg == "-h" || arg == "/?" {
            help_flag = true;
        } else if arg == "--auto" || arg == "-a" {
            auto_flag = true;
        } else if arg.starts_with("--markupfile=") || arg.starts_with("-f") {
            if let Some(path) = arg.strip_prefix("--markupfile=") {
                markup_file_arg = Some(path.to_string());
            } else if let Some(path) = arg.strip_prefix("-f") {
                markup_file_arg = Some(path.to_string());
            }
        } else if Path::new(arg).exists() {
             markup_path = PathBuf::from(arg);
        }
    }

    if help_flag {
        println!("Mastering Utility Wizard");
        println!("Usage: {} [--markupfile=<path>] [--auto] [--help]\n", args[0]);
        println!("Options:");
        println!("  --markupfile, -f   Path to output Markup file.");
        println!("  --auto, -a      Run in automatic mode with default settings.");
        println!("  --help, -h      Display this help message.");
        return None;
    }

    unsafe {
        AUTO_ADD_SONGS = auto_flag;
    }

    if let Some(file) = markup_file_arg {
        markup_path = PathBuf::from(file);
    } else if args.len() > 1 && !Path::new(&args[1]).starts_with("--") && !Path::new(&args[1]).starts_with("-") {
        // Positional argument for file path
        markup_path = PathBuf::from(&args[1]);
    } else {
        println!("Using default: {}", markup_path.display());
    }

    Some(markup_path)
}

/// Interactively collects or edits album and song metadata from the user.
fn collect_albums_interactively(wrapper: &mut UniquePtr<MasteringUtilWrapper>) -> Result<(), String> {
    let initial_album_count = wrapper.pin_mut().AlbumCount();
    
    let album_count;

    if initial_album_count > 0 {
        println!("\nMarkup file contains {} album(s).", initial_album_count);
        album_count = prompt_usize(
            "Enter the total number of albums to process",
            Some(initial_album_count),
        );
    } else {
        album_count = prompt_usize("Enter number of albums to create", None);
    }

    if album_count == 0 && initial_album_count == 0 {
        println!("Skipping album collection/editing.");
        return Ok(());
    }
    
    const DEFAULT_PATH_STR: &str = ".";
    const DEFAULT_COMMENT: &str = " ";
    const DEFAULT_COPYRIGHT: &str = " ";
    const DEFAULT_ARGS: &str = " ";
    const DEFAULT_CODEC: &str = "copy";

    for i in 0..initial_album_count {
        let title = wrapper.GetAlbumTitle(i);
        println!("\n\n=== Editing Existing Album {}/{}: {} ===", i + 1, initial_album_count, title);
        
        preview_album(wrapper, i); 

        if wrapper.GetAlbumID(i) == 0 {
            wrapper.pin_mut().SetAlbumID(i, (i + 1) as i32);
        }

        let title = prompt_string("Enter Album Title", Some(&title));
        wrapper.pin_mut().SetAlbumTitle(i, &title);
        
        let artist = prompt_string("Enter Album Artist", Some(&wrapper.GetAlbumArtist(i)));
        wrapper.pin_mut().SetAlbumArtist(i, &artist);

        let genre = prompt_string("Enter Album Genre", Some(&wrapper.GetAlbumGenre(i)));
        wrapper.pin_mut().SetAlbumGenre(i, &genre);

        let year = prompt_string("Enter Album Year", Some(&wrapper.GetAlbumYear(i)));
        wrapper.pin_mut().SetAlbumYear(i, &year);

        let copyright = wrapper.GetAlbumCopyright(i);
        let copyright = prompt_string("Enter Album Copyright Info", Some(if copyright.is_empty() { DEFAULT_COPYRIGHT } else { &copyright }));
        wrapper.pin_mut().SetAlbumCopyright(i, &copyright);

        let new_path = wrapper.GetAlbumNewPath(i);
        let new_path = prompt_string("Enter Relative Path to save songs", Some(if new_path.is_empty() { DEFAULT_PATH_STR } else { &new_path }));
        wrapper.pin_mut().SetAlbumNewPath(i, &new_path);

        let album_art = wrapper.GetAlbumAlbumArt(i);
        let album_art = prompt_string("Enter Relative Path to Album Art", Some(if album_art.is_empty() { DEFAULT_PATH_STR } else { &album_art }));
        wrapper.pin_mut().SetAlbumAlbumArt(i, &album_art);

        let arguments = wrapper.GetAlbumArguments(i);
        let arguments = prompt_string("Enter additional arguments for FFMPEG (if any)", Some(if arguments.is_empty() { DEFAULT_ARGS } else { &arguments }));
        wrapper.pin_mut().SetAlbumArguments(i, &arguments);

        let initial_song_count = wrapper.pin_mut().SongCount(i);

        let song_count;
        println!("\nAlbum \"{}\" currently has {} song(s).", title, initial_song_count);
        
        song_count = prompt_usize(
            &format!("Enter the total number of songs to process in \"{}\"", title),
            Some(initial_song_count),
        );

        for j in 0..initial_song_count {
            println!("\n--- Editing Existing Song {}/{}: {} ---", j + 1, initial_song_count, wrapper.GetSongTitle(i, j));
            preview_song(&wrapper, i, j, false);

            if wrapper.GetSongID(i, j) == 0 {
                wrapper.pin_mut().SetSongID(i, j, (j + 1) as i32);
            }
            if wrapper.GetSongAlbum(i, j).is_empty() {
                let album_title = wrapper.GetAlbumTitle(i);
                wrapper.pin_mut().SetSongAlbum(i, j, &album_title);
            }

            let path = wrapper.GetSongPath(i, j);
            let path = prompt_string("Enter Song Source Filename", Some(if path.is_empty() { DEFAULT_PATH_STR } else { &path }));
            wrapper.pin_mut().SetSongPath(i, j, &path);

            let song_title = wrapper.GetSongTitle(i, j);
            let song_title = prompt_string("Enter Song Title", Some(&song_title));
            wrapper.pin_mut().SetSongTitle(i, j, &song_title);
            
            let song_artist = wrapper.GetSongArtist(i, j);
            let song_artist_default = if song_artist.is_empty() { wrapper.GetAlbumArtist(i) } else { song_artist };
            let song_artist = prompt_string("Enter Song Artist", Some(&song_artist_default));
            wrapper.pin_mut().SetSongArtist(i, j, &song_artist);
            
            let song_genre = wrapper.GetSongGenre(i, j);
            let song_genre_default = if song_genre.is_empty() { wrapper.GetAlbumGenre(i) } else { song_genre };
            let song_genre = prompt_string("Enter Song Genre", Some(&song_genre_default));
            wrapper.pin_mut().SetSongGenre(i, j, &song_genre);
            
            let song_year = wrapper.GetSongYear(i, j);
            let song_year_default = if song_year.is_empty() { wrapper.GetAlbumYear(i) } else { song_year };
            let song_year = prompt_string("Enter Song Year", Some(&song_year_default));
            wrapper.pin_mut().SetSongYear(i, j, &song_year);
            
            let song_copyright = wrapper.GetSongCopyright(i, j);
            let song_copyright_default = if song_copyright.is_empty() { wrapper.GetAlbumCopyright(i) } else { song_copyright };
            let song_copyright = prompt_string("Enter Song Copyright Info", Some(&song_copyright_default));
            wrapper.pin_mut().SetSongCopyright(i, j, &song_copyright);
            
            let new_path = wrapper.GetSongNewPath(i, j);
            let new_path = prompt_string("Enter New Filename", Some(if new_path.is_empty() { DEFAULT_PATH_STR } else { &new_path }));
            wrapper.pin_mut().SetSongNewPath(i, j, &new_path);
            
            let codec = wrapper.GetSongCodec(i, j);
            let codec = prompt_string("Enter Song Codec (mp3, flac, etc.)", Some(if codec.is_empty() { DEFAULT_CODEC } else { &codec }));
            wrapper.pin_mut().SetSongCodec(i, j, &codec);
            
            let arguments = wrapper.GetSongArguments(i, j);
            let arguments = prompt_string("Enter additional arguments for FFMPEG (if any)", Some(if arguments.is_empty() { DEFAULT_ARGS } else { &arguments }));
            wrapper.pin_mut().SetSongArguments(i, j, &arguments);
        }

        for j in initial_song_count..song_count {
            wrapper.pin_mut().AddSong(i);
            let new_song_index = wrapper.pin_mut().SongCount(i) - 1;

            println!("\n--- Adding New Song {} to album {} ---", j + 1, wrapper.GetAlbumTitle(i));

            wrapper.pin_mut().SetSongTrackNumber(i, new_song_index, (j + 1) as i32);
            wrapper.pin_mut().SetSongID(i, new_song_index, (j + 1) as i32);
            
            let album_title = wrapper.GetAlbumTitle(i);
            wrapper.pin_mut().SetSongAlbum(i, new_song_index, &album_title);
            
            let path = prompt_string("Enter Song Source Filename", Some(DEFAULT_PATH_STR));
            wrapper.pin_mut().SetSongPath(i, new_song_index, &path);

            let song_title = prompt_string("Enter Song Title", None);
            wrapper.pin_mut().SetSongTitle(i, new_song_index, &song_title);

            let song_artist = prompt_string("Enter Song Artist", Some(&wrapper.GetAlbumArtist(i)));
            wrapper.pin_mut().SetSongArtist(i, new_song_index, &song_artist);

            let song_genre = prompt_string("Enter Song Genre", Some(&wrapper.GetAlbumGenre(i)));
            wrapper.pin_mut().SetSongGenre(i, new_song_index, &song_genre);

            let song_year = prompt_string("Enter Song Year", Some(&wrapper.GetAlbumYear(i)));
            wrapper.pin_mut().SetSongYear(i, new_song_index, &song_year);

            let song_copyright = prompt_string("Enter Song Copyright Info", Some(&wrapper.GetAlbumCopyright(i)));
            wrapper.pin_mut().SetSongCopyright(i, new_song_index, &song_copyright);

            let new_path = prompt_string("Enter New Filename", Some(DEFAULT_PATH_STR));
            wrapper.pin_mut().SetSongNewPath(i, new_song_index, &new_path);

            let codec = prompt_string("Enter Song Codec (mp3, flac, etc.)", Some(DEFAULT_CODEC));
            wrapper.pin_mut().SetSongCodec(i, new_song_index, &codec);

            let arguments = prompt_string("Enter additional arguments for FFMPEG (if any)", Some(DEFAULT_ARGS));
            wrapper.pin_mut().SetSongArguments(i, new_song_index, &arguments);
            
            preview_song(&wrapper, i, new_song_index, true);
            
            let mut add_song = unsafe { AUTO_ADD_SONGS };
            
            if !add_song {
                let response = prompt_char("Would you like to add this song to the list? (y/n/a)", None);
                match response {
                    'a' => {
                        unsafe { AUTO_ADD_SONGS = true; }
                        add_song = true;
                    }
                    'y' => add_song = true,
                    'n' => add_song = false,
                    _ => return Err("Unexpected response.".to_string()),
                }
            }

            if !add_song {
                wrapper.pin_mut().RemoveSong(i, new_song_index);
            }
        }

    }

    for i in initial_album_count..album_count {
        wrapper.pin_mut().AddAlbum();
        let new_album_index = wrapper.pin_mut().AlbumCount() - 1;

        println!("\n\n=== Adding New Album {} of {} ===", i + 1, album_count);
        
        wrapper.pin_mut().SetAlbumID(new_album_index, (i + 1) as i32);
        wrapper.pin_mut().SetAlbumComment(new_album_index, DEFAULT_COMMENT);

        let title = prompt_string("Enter Album Title", None);
        wrapper.pin_mut().SetAlbumTitle(new_album_index, &title);
        
        let artist = prompt_string("Enter Album Artist", None);
        wrapper.pin_mut().SetAlbumArtist(new_album_index, &artist);

        let genre = prompt_string("Enter Album Genre", None);
        wrapper.pin_mut().SetAlbumGenre(new_album_index, &genre);

        let year = prompt_string("Enter Album Year", None);
        wrapper.pin_mut().SetAlbumYear(new_album_index, &year);

        let copyright = prompt_string("Enter Album Copyright Info", Some(DEFAULT_COPYRIGHT));
        wrapper.pin_mut().SetAlbumCopyright(new_album_index, &copyright);

        let new_path = prompt_string("Enter Relative Path to save songs", Some(DEFAULT_PATH_STR));
        wrapper.pin_mut().SetAlbumNewPath(new_album_index, &new_path);

        let album_art = prompt_string("Enter Relative Path to Album Art", Some(DEFAULT_PATH_STR));
        wrapper.pin_mut().SetAlbumAlbumArt(new_album_index, &album_art);

        let arguments = prompt_string("Enter additional arguments for FFMPEG (if any)", Some(DEFAULT_ARGS));
        wrapper.pin_mut().SetAlbumArguments(new_album_index, &arguments);

        let song_count_new_album = prompt_usize(
            &format!("How many songs in \"{}\"?", title), 
            Some(0)
        );

        for j in 0..song_count_new_album {
            wrapper.pin_mut().AddSong(new_album_index);
            let new_song_index = wrapper.pin_mut().SongCount(new_album_index) - 1;
            
            println!("\n--- Adding New Song {} ---", j + 1);

            wrapper.pin_mut().SetSongTrackNumber(new_album_index, new_song_index, (j + 1) as i32);
            wrapper.pin_mut().SetSongID(new_album_index, new_song_index, (j + 1) as i32);
            
            let album_title = wrapper.GetAlbumTitle(new_album_index);
            wrapper.pin_mut().SetSongAlbum(new_album_index, new_song_index, &album_title);

            let path = prompt_string("Enter Song Source Filename", Some(DEFAULT_PATH_STR));
            wrapper.pin_mut().SetSongPath(new_album_index, new_song_index, &path);

            let song_title = prompt_string("Enter Song Title", None);
            wrapper.pin_mut().SetSongTitle(new_album_index, new_song_index, &song_title);

            let song_artist = prompt_string("Enter Song Artist", Some(&wrapper.GetAlbumArtist(new_album_index)));
            wrapper.pin_mut().SetSongArtist(new_album_index, new_song_index, &song_artist);

            let song_genre = prompt_string("Enter Song Genre", Some(&wrapper.GetAlbumGenre(new_album_index)));
            wrapper.pin_mut().SetSongGenre(new_album_index, new_song_index, &song_genre);

            let song_year = prompt_string("Enter Song Year", Some(&wrapper.GetAlbumYear(new_album_index)));
            wrapper.pin_mut().SetSongYear(new_album_index, new_song_index, &song_year);

            let song_copyright = prompt_string("Enter Song Copyright Info", Some(&wrapper.GetAlbumCopyright(new_album_index)));
            wrapper.pin_mut().SetSongCopyright(new_album_index, new_song_index, &song_copyright);

            let new_path = prompt_string("Enter New Filename", Some(DEFAULT_PATH_STR));
            wrapper.pin_mut().SetSongNewPath(new_album_index, new_song_index, &new_path);

            let codec = prompt_string("Enter Song Codec (mp3, flac, etc.)", Some(DEFAULT_CODEC));
            wrapper.pin_mut().SetSongCodec(new_album_index, new_song_index, &codec);

            let arguments = prompt_string("Enter additional arguments for FFMPEG (if any)", Some(DEFAULT_ARGS));
            wrapper.pin_mut().SetSongArguments(new_album_index, new_song_index, &arguments);

            preview_song(&wrapper, new_album_index, new_song_index, true);
            
            let mut add_song = unsafe { AUTO_ADD_SONGS };
            
            if !add_song {
                let response = prompt_char("Would you like to add this song to the list? (y/n/a)", None);
                match response {
                    'a' => {
                        unsafe { AUTO_ADD_SONGS = true; }
                        add_song = true;
                    }
                    'y' => add_song = true,
                    'n' => add_song = false,
                    _ => return Err("Unexpected response.".to_string()),
                }
            }

            if !add_song {
                wrapper.pin_mut().RemoveSong(new_album_index, new_song_index);
            }
        }

        preview_album(wrapper, new_album_index);
        let add_album_response = prompt_char(
            &format!("Would you like to add this new album to the list? (y/n)"), 
            Some('y')
        );

        if add_album_response == 'n' {
            wrapper.pin_mut().RemoveAlbum(new_album_index);
        }
    }

    Ok(())
}

/// Prompts the user to confirm whether the Markup file should be written.
fn confirm_save_interactively(markup_path: &Path) -> bool {
    let prompt_msg = format!("Would you like to save \"{}\"? (y/n)", markup_path.display());
    let save_response = prompt_char(&prompt_msg, Some('y'));
    save_response == 'y'
}

/// Writes the album metadata to disk as an Markup file.
fn save_markup_file(wrapper: &mut UniquePtr<MasteringUtilWrapper>, markup_path: &Path) {
    let path_str = markup_path.to_str().expect("Invalid path string");
    wrapper.pin_mut().SaveMarkup(path_str);
    println!("Markup file saved to \"{}\" successfully.", markup_path.display());
}

fn main() {
    println!("Daniel's Mastering Utility -- Wizard\n");
    
    let args: Vec<String> = env::args().collect();
    let mut wrapper = ffi::create();

    let markup_path = match handle_command_line(&args, &mut wrapper) {
        Some(path) => path,
        None => {
            std::process::exit(0);
        }
    };
    
    let result = (|| -> Result<(), String> {
        if markup_path.exists() {
            let path_str = markup_path.to_str().ok_or("Invalid markup path for parsing")?;
            wrapper.pin_mut().ParseMarkup(path_str);
        }

        collect_albums_interactively(&mut wrapper)?;

        if wrapper.pin_mut().AlbumCount() == 0 {
            println!("No albums to save. Exiting.");
            return Ok(());
        }

        if confirm_save_interactively(&markup_path) {
            save_markup_file(&mut wrapper, &markup_path);
        } else {
            println!("Save operation cancelled by user. Exiting without saving.");
        }

        Ok(())
    })();

    match result {
        Ok(_) => cleanup(0),
        Err(e) => {
            eprintln!("[Fatal Error] {}", e);
            cleanup(1);
        }
    }
}

fn cleanup(code: i32) {
    if code != 0 {
        eprintln!("Exited with code {}", code);
    }
    std::process::exit(code);
}
