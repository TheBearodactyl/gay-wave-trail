# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [5.3.1] - 2026-03-2

### Fixed

- The custom wave trail implementation is now a LOT less buggy

## [5.3.0] - 2026-03-2

### Added

- The `Trail Rewrite` implementation is now multithreaded (the math is, nothing else though lol)

## [5.2.0] - 2026-03-1

### Added

- Added new Trail Rewrite setting that enables the usage of a new fully custom implementation of the Wave Trail (WIP)

### Changed

- Upgrade to Geode v5.3.0

## [5.1.4] - 2026-02-25

### Added

- Now supports iOS

## [5.1.3] - 2026-02-25

### Fixed

- Redid Eclipse integration

## [5.1.2] - 2026-02-24

### Added

- Started on adding debug logging for the `verbose-logging` setting

## [5.1.1] - 2026-02-24

### Changed

- Scroll position now gets saved when closing the custom settings popup.

### Fixed

- Trail segmentation now somewhat works

## [5.1.0] - 2026-02-24

### Changed

- `src/ui.cpp` is now separated into multiple files instead of being a single 1k line file
- The custom settings popup has more polish to it now and has a lot of new QOL changes

### Fixed

- "Solid Wave Trail" now actually works via a custom hook for `drawPolygon`
- "Trail Fade" and "Solid Wave Trail" are now fully compatible
- "Trail Outline" should be much more consistent now
- "Persist Trail" now works for real lmao

### Added

- New "Disable Regular Streak" setting
- New setting for verbose logging

## [5.0.0] - 2026-02-22

### Fixed

- Update to Geode V5

### Added

- Use a custom settings menu for the pause button
- Add separate modes for displaying settings in the custom popup
- Add "Gay Mode" for the settings menu
- Add `support.md`

### Changed

- Remove CMake code for enabling `-Wall -Wextra -Wpedantic`
- Refactor so that there's less files

## [4.3.3] - 2025-10-21

### Changed

- Trail Outline now uses the global delta time instead of `0.5` for changing colors.

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
