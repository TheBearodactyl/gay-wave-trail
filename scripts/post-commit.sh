#!/bin/sh
# heavily modified version of https://github.com/MartinSeeler/auto-changelog-hook/blob/master/post-commit

OUTPUT_FILE=CHANGELOG.md
TEMP_FILE=$(mktemp)

cat > "$TEMP_FILE" << 'EOF'
# Changelog

## [Unreleased]

EOF

COMMIT_HASHES=$(git rev-list --no-merges --reverse HEAD)

LAST_VERSION=""

if git log -1 --pretty=%B | grep -q "Populated Changelog"; then
    echo "Changelog already populated in the last commit. Skipping."
    rm "$TEMP_FILE"
    exit 0
elif git log -1 --pretty=%B | grep -q "Updated Changelog"; then
    echo "Changelog already populated in the last commit. Skipping."
    rm "$TEMP_FILE"
    exit 0
fi

for COMMIT_HASH in $COMMIT_HASHES
do
    VERSION=$(git show "$COMMIT_HASH:mod.json" 2>/dev/null | grep -o '\"version\": \"[0-9.]*\"' | awk -F'\"' '{print $4}')

    if [ -n "$VERSION" ] && [ "$VERSION" != "$LAST_VERSION" ]; then
        echo "## [$VERSION] - $(git show -s --format=%ci "$COMMIT_HASH" | cut -d ' ' -f 1)" >> "$TEMP_FILE"
        LAST_VERSION=$VERSION
    fi

    git --no-pager log -1 --format="### %s%n**Date:** %aD  \n**Author:** %aN (%aE)  \n**Commit Hash:** `%H`  \n%n%b%n\n---\n" "$COMMIT_HASH" >> "$TEMP_FILE"
done

if [ -f "$OUTPUT_FILE" ]; then
    tail -n +2 "$OUTPUT_FILE" >> "$TEMP_FILE" 2>/dev/null || true
fi

if [ "$(wc -l < "$TEMP_FILE")" -gt 2 ]; then
    mv "$TEMP_FILE" "$OUTPUT_FILE"

    if ! git diff --quiet "$OUTPUT_FILE"; then
        git add "$OUTPUT_FILE"
        git commit --amend --no-edit -m "Updated Changelog"

        echo "Populated Changelog in $OUTPUT_FILE"
    else
        echo "No changes to $OUTPUT_FILE"
        rm "$TEMP_FILE"
    fi
else
    echo "No new entries to add to the changelog."
    rm "$TEMP_FILE"
fi
