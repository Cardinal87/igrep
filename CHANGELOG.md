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
