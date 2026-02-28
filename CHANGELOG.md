# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2025-09-03
### Added
- `create` option to inizialize new index-files (uses optional `--destination` flag to set path)
- Optional `--source-index` flag to `index`, `remove` and `find` options to specify which index-file to use

### Fixed
- Improved output formatting by ensuring each line ends with a newline character.

### Changed
- Update `--help` flag output according to new options and flags

## [1.2.0] - 2025-09-11
### Changed
- **Index format**: Optimized memory and disk usage by approximately 50%.
  - File paths are now stored in a dictionary structure, mapped to unique numerical IDs.
  - All numerical values are encoding using VarInt encoding


## [2.0.0] - 2026-02-28
### Added
- LSM-tree architecture with SSTable storage, enabling indexing of large datasets without loading the entire index into RAM.
- Compact command to optimize the index by removing stale entries and merging SSTable levels.

### Changed 
- Complete overhaul of the indexing engine: replaced in‑memory dictionary with disk‑based LSM tree. Index format is now incompatible with versions 1.x.
