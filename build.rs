fn main() {
    cxx_build::bridge("src/backend/rs/MasteringUtil.rs")
        .file("src/backend/cpp/MasteringUtil.cpp")
        .std("c++20")
        .flag_if_supported("/EHsc")
        .include("src/backend/cpp")
        .include("src/backend/rs")
        .compile("masteringutil");
}
