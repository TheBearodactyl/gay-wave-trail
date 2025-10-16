#!/bin/bash
set -eu

CHANGED_FILES=$(git diff --cached --name-only)

if echo "$CHANGED_FILES" | grep -q '^src/'; then
	echo "Detected changes to the source code. You may need to update the version in \`mod.json\`"

	MOD_FILE="mod.json"

	if [ ! -f "$MOD_FILE" ]; then
		echo "Error: $MOD_FILE not found."
		exit 1
	fi

	if command -v jq >/dev/null 2>&1; then
		CURRENT_VERSION=$(jq -r '.version' "$MOD_FILE")
	else
		CURRENT_VERSION=$(grep -oE '"version"\s*:\s*"[^"]+"' "$MOD_FILE" | cut -d'"' -f4)
	fi

	echo "Current version: $CURRENT_VERSION"
	printf "Enter new version: "
	read NEW_VERSION

	if [ -z "$NEW_VERSION" ]; then
		echo "No version entered. Aborting commit."
		exit 1
	fi

	if command -v jq >/dev/null 2>&1; then
		jq --arg v "$NEW_VERSION" '.version = $v' "$MOD_FILE" > "$MOD_FILE.tmp" && mv "$MOD_FILE.tmp" "$MOD_FILE"
	elif command -v pwsh >/dev/null 2>&1; then
		pwsh -NoProfile -Command "(Get-Content $MOD_FILE) -replace '\"version\"\s*:\s*\"[^\"]+\"', '\"version\": \"$NEW_VERSION\"' | Set-Content $MOD_FILE"
	elif command -v powershell >/dev/null 2>&1; then
		powershell -NoProfile -Command "(Get-Content $MOD_FILE) -replace '\"version\"\s*:\s*\"[^\"]+\"', '\"version\": \"$NEW_VERSION\"' | Set-Content $MOD_FILE"
	else
		sed "s/\"version\"[[:space:]]*:[[:space:]]*\"[^\"]*\"/\"version\": \"$NEW_VERSION\"/" "$MOD_FILE" > "$MOD_FILE.tmp" && mv "$MOD_FILE.tmp" "$MOD_FILE"
	fi

	git add "$MOD_FILE"

	echo "Updated mod version to $NEW_VERSION and staged it"
fi

exit 0
