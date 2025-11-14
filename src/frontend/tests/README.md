# Mastering Utility Tests

This directory contains the implementations of the Mastering Utility Tests, the main tests for this project.

## Key Features

- **Test Data Generation**: Creates `MasteringUtility::Album` and `MasteringUtility::Song` objects with example data.

- **Markup Persistence Test**:

  - Uses `MasteringUtility::SaveMarkup` to write the generated album and song metadata to a temporary `.mas` file.
  - Uses `MasteringUtility::ParseMarkup` to read the metadata back from the same file.

- **Data Validation**: Compares the original data structures with the parsed data structures field by field to ensure data integrity during serialization and deserialization.

- **Performance Measurement**: Times the execution duration of the tests.
