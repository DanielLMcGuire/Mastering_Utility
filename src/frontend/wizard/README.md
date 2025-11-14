# Mastering Utility Wizard

This directory contains the implementations of the Mastering Utility Wizard, a command-line tool designed to help users create and edit album and song metadata, and then save this information to a Markup file.

## Implementations

### C++ Implementation (`cpp/wizard.cpp`)

The C++ variant of the wizard provides an command line interface for managing audio project metadata. It utilizes the `DConsole` library for parsing command-line arguments and handling user prompts. Users can define album details, add or modify songs within each album, and specify various metadata fields such as title, artist, genre, year, copyright, file paths, and FFmpeg arguments. The wizard supports loading existing Markup files for editing and saving new or modified data.

### Rust Implementation (`rs/wizard.rs`)

The Rust implementation offers similar functionality to its C++ counterpart, enabling interactive creation and editing of album and song metadata via the command line. This version integrates with the `masteringutil` library through the `cxx` crate for Foreign Function Interface (FFI), allowing it to leverage core utility functions. It features custom prompting functions for user input and provides a streamlined workflow for managing audio project details, including command-line argument parsing for automation and help display.
