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

use masteringutil::ffi;
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    
    if args.len() < 2 {
        eprintln!("Usage: {} <markup_file.ini>", args[0]);
        std::process::exit(1);
    }
    
    let markup_file = &args[1];
    
    // Create the wrapper using the factory function
    let mut wrapper = ffi::create();
    
    // Parse the markup file
    wrapper.pin_mut().ParseMarkup(markup_file);
    
    // Get album count
    let album_count = wrapper.pin_mut().AlbumCount();
    println!("Found {} albums", album_count);
    
    // Process each album
    for i in 0..album_count {
        println!("Processing album {}...", i);
        wrapper.pin_mut().ProcessAlbum(i);
    }
    
    println!("Mastering complete!");
}
