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

fn main() {
    cxx_build::bridge("main.rs")
        .file("../rust.cpp")
        .flag_if_supported("-std=c++20")
        .compile("MasteringUtility");

    println!("cargo:rerun-if-changed=main.rs");
    println!("cargo:rerun-if-changed=../rust.cpp");
    println!("cargo:rerun-if-changed=../MasteringUtility.h");
}
