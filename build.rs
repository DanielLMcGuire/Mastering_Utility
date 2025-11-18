fn main() {
    cxx_build::bridge("src/backend/rs/MasteringUtil.rs")
        .file("src/backend/cpp/MasteringUtil.cpp")
        .include("src/backend/cpp")
        .include("src/backend/rs")
        .std("c++20")
    	.flag_if_supported("/EHsc")
		.flag_if_supported("-fexceptions")
		.flag_if_supported("-fcxx-exceptions")
        .compile("masteringutil");
}
