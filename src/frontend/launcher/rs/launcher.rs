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

use masteringutil_rs::ffi;
use std::env;

fn main() {
    println!("Daniel's Mastering Utility");
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        eprintln!("Usage: {} <album.mas>", args[0]);
        std::process::exit(1);
    }

    let markup_file = &args[1];

    let mut wrapper = ffi::create();

    wrapper.pin_mut().Master(markup_file);

    println!("Mastering complete!");
}
