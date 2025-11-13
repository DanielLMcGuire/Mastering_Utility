fn main() {
    cxx_build::bridge("src/MasteringUtil.rs")
        .file("src/MasteringUtil.cpp")
        .std("c++20")
        .flag_if_supported("/EHsc")
        .include("src")
        .compile("masteringutil");
}
