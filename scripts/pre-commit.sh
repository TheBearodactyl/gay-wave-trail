#!/usr/bin/env bash

set -e

CHANGED_FILES=$(git diff --cached --name-only)

if echo "$CHANGED_FILES" | grep -q '^src/'; then
  echo "Detected changes in src/. Auto-updating mod.json version."

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

  IFS='.' read -r MAJOR MINOR PATCH <<< "$CURRENT_VERSION"
  if [ -z "$PATCH" ]; then
    PATCH=0
  fi

  PATCH=$((PATCH + 1))
  NEW_VERSION="$MAJOR.$MINOR.$PATCH"

  echo "Current version: $CURRENT_VERSION → New version: $NEW_VERSION"

  if command -v jq >/dev/null 2>&1; then
    jq --arg v "$NEW_VERSION" '.version = $v' "$MOD_FILE" > "$MOD_FILE.tmp" && mv "$MOD_FILE.tmp" "$MOD_FILE"
  elif command -v pwsh >/dev/null 2>&1; then
	  pwsh -NoProfile -Command "
	  \$jsonContent = Get-Content -Raw '$MOD_FILE' | ConvertFrom-Json
	  if (-not \$jsonContent.PSObject.Properties.Name -contains 'version') {
		  Write-Error 'No top-level version found in mod.json. Aborting.'
		  exit 1
	  }
      \$versionParts = \$jsonContent.version -split '\.'
	  if (\$versionParts.Count -ne 3) {
		  Write-Error 'Version format is not valid semver (major.minor.patch)'
		  exit 1
	  }
      [int]\$major = \$versionParts[0]
      [int]\$minor = \$versionParts[1]
      [int]\$patch = \$versionParts[2]
	  \$patch++
	  \$jsonContent.version = \"\$major.\$minor.\$patch\"
	  \$jsonContent | ConvertTo-Json -Depth 10 | Set-Content -NoNewline '$MOD_FILE'
	  "
  elif command -v powershell >/dev/null 2>&1; then
    powershell -NoProfile -Command "(Get-Content $MOD_FILE) -replace '(\"version\"[ \t]*:[ \t]*\")([^\"\r\n]+)(\")', '\$1$NEW_VERSION\$3',1 | Set-Content $MOD_FILE"
  else
    sed "0,/\"version\"[[:space:]]*:[[:space:]]*\"[^\"]*\"/{s/\"version\"[[:space:]]*:[[:space:]]*\"[^\"]*\"/\"version\": \"$NEW_VERSION\"/}" "$MOD_FILE" > "$MOD_FILE.tmp" && mv "$MOD_FILE.tmp" "$MOD_FILE"
  fi

  git add "$MOD_FILE"
  echo "Auto-bumped top-level mod.json version to $NEW_VERSION and staged it."
fi

exit 0

