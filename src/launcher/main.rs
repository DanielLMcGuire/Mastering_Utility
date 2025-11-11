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

#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("MasteringUtility.h");

        type MasteringUtility;

        fn new_mastering_utility() -> UniquePtr<MasteringUtility>;

        fn Master(self: &MasteringUtility, markup_file: &CxxString);
        fn ParseMarkup(self: &MasteringUtility, markup_file: &CxxString);
    }
}

fn main() {
    let masterer = ffi::new_mastering_utility();
    let markup_path = "example_markup.txt";

    let masterer_ref = masterer.as_ref().expect("Failed to create MasteringUtility");

    masterer_ref.ParseMarkup(markup_path);
    masterer_ref.Master(markup_path);

    println!("Mastering finished!");
}
