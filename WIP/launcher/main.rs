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
