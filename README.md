# Mastering Utility

Mastering Utility provides a simple markup format for tagging music files and re-encoding them as well.

- Windows installers (MSI and EXE) are available from the Releases page: https://github.com/DanielLMcGuire/Mastering_Utility/releases
- For other platforms (Linux, macOS, or windows) check the latest build artifacts on Actions: https://github.com/DanielLMcGuire/Mastering_Utility/actions
- If you prefer building from source, see the "Building from source" section below.

Features
- Define album metadata (artist, title, year, genre) in a single markup file.
- List source audio files for each track and map tags (title, artist, track number).
- Re-encode tracks in batch using ffmpeg (supports all formats supported by ffmpeg).

Prerequisites
- ffmpeg installed and available on PATH.
- A C++ build toolchain (if building from source): CMake, and a supported compiler of your choice.
- Basic familiarity with the command line and INI files.
- A 64-bit machine (unless building from source)

Installing / Building from source

1. Clone the repository
```
git clone --depth=1 https://github.com/DanielLMcGuire/Mastering_Utility.git
cd Mastering_Utility
```

2. Build (example with CMake)
```
# Configure
cmake -S . -B build 
# Specify '-G Ninja' for faster compilation)

# Compile
cmake --build .
```

Usage overview
The utility reads a markup file that describes an album and its tracks, then tags and re-encodes tracks into an output directory.

General usage:

```bash
# Create file.ini
./masteringwizard file.ini 
```

```bash
# Process file.ini
./masteringutility file.ini
```
