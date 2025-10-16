#!/bin/sh

OUTPUT_FILE=CHANGELOG.md
TEMP_FILE=$(mktemp)

cat > "$TEMP_FILE" << 'EOF'
# Changelog

## [Unreleased]

EOF

COMMIT_HASHES=$(git rev-list --no-merges --reverse HEAD)
LAST_VERSION=""

for COMMIT_HASH in $COMMIT_HASHES
do
    VERSION=$(git show "$COMMIT_HASH:mod.json" 2>/dev/null | grep -o '\"version\": \"[0-9.]*\"' | awk -F'"' '{print $4}')

    if [ -n "$VERSION" ] && [ "$VERSION" != "$LAST_VERSION" ]; then
        echo -e "\n## [$VERSION] - $(git show -s --format=%ci "$COMMIT_HASH" | cut -d ' ' -f 1)\n" >> "$TEMP_FILE"
        LAST_VERSION=$VERSION
    fi

    COMMIT_SUBJECT=$(git log -1 --format="%s" "$COMMIT_HASH")
    COMMIT_AUTHOR=$(git log -1 --format="%aN <%aE>" "$COMMIT_HASH")
    COMMIT_DATE=$(git log -1 --format="%aD" "$COMMIT_HASH")
    COMMIT_BODY=$(git log -1 --format="%b" "$COMMIT_HASH")

    echo -e "- **$COMMIT_SUBJECT**  \n  **Date:** $COMMIT_DATE  \n  **Author:** $COMMIT_AUTHOR  \n  **Commit:** \`$COMMIT_HASH\`" >> "$TEMP_FILE"

    if [ -n "$COMMIT_BODY" ]; then
        echo -e "  \n  $COMMIT_BODY" >> "$TEMP_FILE"
    fi

    echo >> "$TEMP_FILE"
done

if [ -f "$OUTPUT_FILE" ]; then
    tail -n +2 "$OUTPUT_FILE" >> "$TEMP_FILE" 2>/dev/null || true
fi

if [ -f "$OUTPUT_FILE" ] && cmp -s "$TEMP_FILE" "$OUTPUT_FILE"; then
    rm "$TEMP_FILE"
    echo "No changes to $OUTPUT_FILE"
    exit 0
fi

mv "$TEMP_FILE" "$OUTPUT_FILE"

res=$(git status --porcelain | grep -c ".\$OUTPUT_FILE$")
if [ "$res" -gt 0 ]; then
    git add "$OUTPUT_FILE"
    git commit --amend --no-edit
    echo "Populated Changelog in $OUTPUT_FILE"
else
    echo "No changes to $OUTPUT_FILE"
fi

