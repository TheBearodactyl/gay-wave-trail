# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Introduce Docker based GitHub actions workflow
- Create `Dockerfile` and `docker-compose.yml`

## [5.4.5] - 2026-03-11

### Fixed

- Fixed P1 being a cracker when the gaydient is enabled

## [5.4.4] - 2026-03-10

### Added

- Weighted insults

### Removed

- bad word

## [5.4.3] - 2026-03-10

### Changed

- Insult generator is now only pseudorandom

### Removed

- Removed the truerand system :(

## [5.4.2] - 2026-03-10

### Added

- Add new granular `phase-calc-speed-modifier` setting
- Implemented crossplatform true RNG in `include/gay/utils/trand.hpp`
- Added a warning popup for when enabling granular mode
- Added a bunch of insults in `include/gay/setting_defs.hpp`

### Changed

- Added to `support.md`
- Bump version to 5.4.2

## [5.4.1] - 2026-03-9

### Added

- New API for registering custom settings menu modes

### Changed

- Refactor the settings menu modes system to make it easier to add more modes

## [5.4.0] - 2026-03-5

### Added

- New granular mode for the custom settings menu with lots of extremely specific settings for tuning
- Different modes for the geometry of the custom wave trail rewrite
- Add pulsing to the custom wave trail rewrite

### Changed

- Optimize the geometry builders

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

### Added

- New "Disable Regular Streak" setting
- New setting for verbose logging

### Changed

- `src/ui.cpp` is now separated into multiple files instead of being a single 1k line file
- The custom settings popup has more polish to it now and has a lot of new QOL changes

### Fixed

- "Solid Wave Trail" now actually works via a custom hook for `drawPolygon`
- "Trail Fade" and "Solid Wave Trail" are now fully compatible
- "Trail Outline" should be much more consistent now
- "Persist Trail" now works for real lmao

## [5.0.0] - 2026-02-22

### Added

- Use a custom settings menu for the pause button
- Add separate modes for displaying settings in the custom popup
- Add "Gay Mode" for the settings menu
- Add `support.md`

### Changed

- Remove CMake code for enabling `-Wall -Wextra -Wpedantic`
- Refactor so that there's less files

### Fixed

- Update to Geode V5

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

