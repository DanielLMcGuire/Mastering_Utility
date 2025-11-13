fn main() {
    cxx_build::bridge("src/rust/MasteringUtil.rs")
        .file("src/backend/MasteringUtil.cpp")
        .std("c++20")
        .flag_if_supported("/EHsc")
        .include("src/backend")
        .include("src/rust")
        .compile("masteringutil");
}
