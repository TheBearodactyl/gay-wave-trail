#!/usr/bin/env bash
set -euo pipefail

CHANGELOG_FILE="CHANGELOG.md"

[ -f "$CHANGELOG_FILE" ] || touch "$CHANGELOG_FILE"

nvim "$CHANGELOG_FILE"

exit 0
