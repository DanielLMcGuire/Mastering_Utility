fn main() {
    cxx_build::bridge("src/MasteringUtil.rs")
        .file("cpp/MasteringUtil.cpp")
        .file("cpp/MasteringUtil_cxx.cpp")
        .flag_if_supported("-std=c++20")
        .compile("masteringutil");

    println!("cargo:rerun-if-changed=cpp/MasteringUtil.cpp");
    println!("cargo:rerun-if-changed=cpp/MasteringUtil_cxx.cpp");
    println!("cargo:rerun-if-changed=cpp/MasteringUtil_cxx.h");
    println!("cargo:rerun-if-changed=cpp/MasteringUtil.h");
    println!("cargo:rerun-if-changed=src/MasteringUtil.rs");
}
