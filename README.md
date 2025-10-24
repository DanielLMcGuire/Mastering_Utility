# Mastering Utility

![GitHub License](https://img.shields.io/github/license/DanielLMcGuire/Mastering_Utility)
![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/DanielLMcGuire/Mastering_Utility/cmake-multi-platform.yml)

Simplifies audio tagging using a metadata markup format and a Wizard to aid in creating the format.

## What does it do?

- Organize multiple albums and tracks in a single configuration file
- Batch re-encode audio files (WAV, FLAC, MP3, etc.) with consistent quality settings
- Automatically apply metadata tags (title, artist, album, track number, year, genre, copyright)
- Maintain a structured workflow for audio mastering projects

## Download

- **Windows**: Pre-built 64-bit installers (MSI and EXE) available on the [Releases page](https://github.com/DanielLMcGuire/Mastering_Utility/releases/latest)
- **Linux/macOS**: Download artifacts from the latest [GitHub Actions build](https://github.com/DanielLMcGuire/Mastering_Utility/actions)
- **Build from source**: See instructions below

## Prerequisites

### Required
- **ffmpeg** - Must be installed and available in your system PATH
  - Test with: `ffmpeg -version`
  - Download from: https://ffmpeg.org/download.html

### For building from source
- CMake 3.10 or higher
- C++17 compatible compiler (GCC, Clang, MSVC, etc.)
- 64-bit system (Recommended)

## Installation

### Option 1: Use pre-built binaries
Download the appropriate installer for your platform from the [Releases page](https://github.com/DanielLMcGuire/Mastering_Utility/releases).

### Option 2: Build from source

```bash
git clone --depth=1 https://github.com/DanielLMcGuire/Mastering_Utility.git
cd Mastering_Utility

cmake -S . -B build
# Optional: use Ninja for faster compilation
# cmake -S . -B build -G Ninja

cmake --build build
```

## Usage

The utility consists of two tools:

### 1. Mastering Wizard (`masteringwizard`)
Interactive tool to create INI configuration files.

```bash
# Create a new configuration file
./masteringwizard output.ini

# Show help
./masteringwizard --help
```

The wizard will prompt you for:
- Number of albums
- Album metadata (title, artist, genre, year, copyright, album art path)
- Output directory for processed files
- Song information (source file, title, artist, track number, output filename)
- Audio codec settings (libmp3lame, flac, copy, etc.)

**Tip**: You can type `exit` or `quit` at any prompt to exit the wizard.

### 2. Mastering Utility (`masteringutility`)
Processes the INI file and encodes all tracks.

```bash
# Process an INI configuration file
./masteringutility myalbum.ini
```

This will:
- Read album and song metadata from the INI file
- Create output directories as specified
- Re-encode each track with ffmpeg using your chosen codec
- Apply all metadata tags to the output files
- Retry failed conversions up to 3 times

## INI File Format

The INI file uses a simple custom format:

```ini
album 1 ("Album Title", "Artist Name", "© 2025 Copyright", "cover.jpg", "./source", "./output", "Genre", "2025", "Optional comment")
{
    song 1 ("Track Title", "Artist Name", 1, "input.wav", "01-track.mp3", "libmp3lame", "Genre", "2025", "Optional comment")
    song 2 ("Another Track", "Artist Name", 2, "input2.flac", "02-track.flac", "flac", "Genre", "2025")
}

album 2 ("Second Album", "Different Artist", "N/A", "art.jpg", "./", "./album2", "Rock", "2024")
{
    song 1 ("Song One", "Different Artist", 1, "song1.wav", "song1.mp3", "libmp3lame", "Rock", "2024")
}
```

### Album syntax
```
album <ID> ("Title", "Artist", "Copyright", "AlbumArtPath", "SourcePath", "OutputPath", "Genre", "Year", "Comment")
```

### Song syntax
```
song <ID> ("Title", "Artist", TrackNumber, "SourceFile", "OutputFile", "Codec", "Genre", "Year", "Comment")
```

### Supported codecs
Any ffmpeg audio codec, including:
- `libmp3lame` - MP3 encoding (VBR quality 3 by default)
- `flac` - FLAC lossless encoding (compression level 12 by default)
- `aac` - AAC encoding
- `libopus` - Opus encoding
- `copy` - Copy audio stream without re-encoding (The wizard will default to this)

## Example Workflow

```bash
./masteringwizard myproject.ini
```
<details>

<summary>Output:</summary>
<pre>
Daniel's Mastering Utility -- Wizard
Enter path to save INI file [myproject.ini]: # [] is the default, being the 1st argument for this prompt
Enter number of albums to create [1]: 1
Enter Album Title: My Greatest Hits
Enter Album Artist: John Doe
Enter Album Genre: Rock
Enter Album Year: 2025
Enter Album Copyright Info [N/A]: (C) 2025 John Doe
Enter Relative Path to save songs: ./output
Enter Relative Path to Album Art: cover.jpg
How many songs in "My Greatest Hits"? [0]: 1

Enter Song Source Filename: track1.wav
Enter Song Title: First Song
Enter Song Artist [John Doe]: 
Enter Song Genre [Rock]: 
Enter Song Year [2025]: 
Enter Song Copyright Info [(C) 2025 John Doe]:  2025 John Doe
Enter New Filename: 01 firstsong.mp3
Enter Song Codec (libmp3lame, flac, etc.) [copy]: libmp3lame

=== Album Summary ===
Album: My Greatest Hits | Artist: John Doe | Genre: Rock | Year: 2025 | Songs: 1
  [1] First Song (John Doe)
-------------------------
Would you like to add this album to the list? (y/n) [Y]: y
Would you like to save "myalbum.ini"? (y/n) [Y]: y
Wrote myproject.ini
Would you like to master this album(s)? (y/n) [n]: n
</pre>
</details>

```bash
./masteringutility myproject.ini
```

## AI Generated Content Disclosure

- Over 80% of the code is written manually
- GitHub Actions workflows and some scripts are primarily AI-generated (verified for correctness)
- All issue responses and documentation refinements are human-written
- Issues made by me are AI generated if they start with `(autogenerated)`

## License

See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Support

For issues, questions, or feature requests, please use the [GitHub Issues page](https://github.com/DanielLMcGuire/Mastering_Utility/issues).
