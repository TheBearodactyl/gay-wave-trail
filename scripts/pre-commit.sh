#!/usr/bin/env bash

set -e
CHANGELOG_FILE="CHANGELOG.md"
CHANGED_FILES=$(git diff --cached --name-only)

if echo "$CHANGED_FILES" | grep -q '^src/'; then
  echo "Detected changes in src/. A version bump of mod.json is required."

  MOD_FILE="mod.json"
  if [ ! -f "$MOD_FILE" ]; then
    echo "Error: $MOD_FILE not found."
    exit 1
  fi

  if command -v jq >/dev/null 2>&1; then
    CURRENT_VERSION=$(jq -r '.version // empty' "$MOD_FILE")
  else
    CURRENT_VERSION=$(awk '/"version"/{print; exit}' "$MOD_FILE" | grep -oP '"version"\s*:\s*"\K[^"]+')
  fi

  if [ -z "$CURRENT_VERSION" ]; then
    echo "No top-level version found in mod.json. Aborting commit."
    exit 1
  fi

  CLEAN_VERSION=$(echo "$CURRENT_VERSION" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//;s/^"//;s/"$//')

  MAJOR=$(echo "$CLEAN_VERSION" | cut -d. -f1)
  MINOR=$(echo "$CLEAN_VERSION" | cut -d. -f2)
  PATCH=$(echo "$CLEAN_VERSION" | cut -d. -f3)

  if [ -z "$MAJOR" ] || [ -z "$MINOR" ] || [ -z "$PATCH" ]; then
    echo "Error: Version format is not valid semver (major.minor.patch). Got: $CURRENT_VERSION"
    exit 1
  fi

  if ! echo "$MAJOR" | grep -qE '^[0-9]+$' || ! echo "$MINOR" | grep -qE '^[0-9]+$' || ! echo "$PATCH" | grep -qE '^[0-9]+$'; then
    echo "Error: Version components must be numeric. Got: $CURRENT_VERSION"
    exit 1
  fi

  SUGGESTED_VERSION="$MAJOR.$MINOR.$((PATCH + 1))"

  while true; do
    printf "Current version: %s\nEnter new version [%s]: " "$CLEAN_VERSION" "$SUGGESTED_VERSION" > /dev/tty
    read -r NEW_VERSION < /dev/tty

    if [ -z "$NEW_VERSION" ]; then
      NEW_VERSION="$SUGGESTED_VERSION"
    fi

    NEW_MAJOR=$(echo "$NEW_VERSION" | cut -d. -f1)
    NEW_MINOR=$(echo "$NEW_VERSION" | cut -d. -f2)
    NEW_PATCH=$(echo "$NEW_VERSION" | cut -d. -f3)
    FIELD_COUNT=$(echo "$NEW_VERSION" | awk -F. '{print NF}')

    if [ "$FIELD_COUNT" -eq 3 ] \
      && echo "$NEW_MAJOR" | grep -qE '^[0-9]+$' \
      && echo "$NEW_MINOR" | grep -qE '^[0-9]+$' \
      && echo "$NEW_PATCH" | grep -qE '^[0-9]+$'; then
      break
    fi

    echo "Invalid version '$NEW_VERSION'. Please use semver format: major.minor.patch (e.g. 1.2.3)" > /dev/tty
  done

  echo "Updating version: $CLEAN_VERSION → $NEW_VERSION"

  if command -v jq >/dev/null 2>&1; then
    jq --arg v "$NEW_VERSION" '.version = $v' "$MOD_FILE" > "$MOD_FILE.tmp" && mv "$MOD_FILE.tmp" "$MOD_FILE"
  elif command -v pwsh >/dev/null 2>&1; then
    pwsh -NoProfile -Command "
      \$jsonContent = Get-Content -Raw '$MOD_FILE' | ConvertFrom-Json
      if (-not \$jsonContent.PSObject.Properties.Name -contains 'version') {
        Write-Error 'No top-level version found in mod.json. Aborting.'
        exit 1
      }
      \$jsonContent.version = '$NEW_VERSION'
      \$jsonContent | ConvertTo-Json -Depth 10 | Set-Content -NoNewline '$MOD_FILE'
    "
  elif command -v powershell >/dev/null 2>&1; then
    powershell -NoProfile -Command "(Get-Content $MOD_FILE) -replace '(\"version\"[ \t]*:[ \t]*\")([^\"\r\n]+)(\")', '\${1}$NEW_VERSION\${3}' | Set-Content $MOD_FILE"
  else
    sed -E "s/\"version\"[[:space:]]*:[[:space:]]*\"[^\"]*\"/\"version\": \"$NEW_VERSION\"/" "$MOD_FILE" > "$MOD_FILE.tmp" && mv "$MOD_FILE.tmp" "$MOD_FILE"
  fi

  git add "$MOD_FILE"
  echo "Updated mod.json version to $NEW_VERSION and staged it."
fi

[ -f "$CHANGELOG_FILE" ] || touch "$CHANGELOG_FILE"

nvim "$CHANGELOG_FILE"

git add "$CHANGELOG_FILE"

exit 0
