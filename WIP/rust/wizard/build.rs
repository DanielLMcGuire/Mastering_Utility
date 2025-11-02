fn main() {
    cxx_build::bridge("main.rs")
        .file("../rust.cpp")
        .flag_if_supported("-std=c++20")
        .compile("MasteringWizard");

    println!("cargo:rerun-if-changed=main.rs");
    println!("cargo:rerun-if-changed=../rust.cpp");
    println!("cargo:rerun-if-changed=../MasteringUtility.h");
}
