# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed

- Have the pre-commit hook open the changelog instead of updating mod version every time

## [4.3.2] - 2025-10-16

### Added

- Trail segmentation settings
- Trail brightness settings
- Trail thickness settings
- Trail music sync
- A pre-commit hook for version bumping and changelog editing
- Player independent gaydient settings

### Changed

- Used an include directory instead of keeping headers in `src`
- Move `src/trail_customization` to just `src/customization`
- Rewrite `.clang-format` to be more Rust-like

### Fixed

- Trail outline being janky
