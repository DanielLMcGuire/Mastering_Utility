fn main() {
    cxx_build::bridge("src/main.rs")
        .file("src/main.cpp")
        .flag_if_supported("-std=c++17")
        .compile("masteringutility");

    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=src/main.cpp");
    println!("cargo:rerun-if-changed=MasteringUtility.h");
}
