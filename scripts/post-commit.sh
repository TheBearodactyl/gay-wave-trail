#!/bin/sh

OUTPUT_FILE=CHANGELOG.md
TEMP_FILE=$(mktemp)

cat > "$TEMP_FILE" << 'EOF'
# Changelog

## [Unreleased]

EOF

COMMIT_HASHES=$(git rev-list --no-merges --reverse HEAD)
declare -A VERSION_COMMITS

for COMMIT_HASH in $COMMIT_HASHES
do
    VERSION=$(git show "$COMMIT_HASH:mod.json" 2>/dev/null | grep -o '\"version\": \"[0-9.]*\"' | awk -F'"' '{print $4}')
    [ -z "$VERSION" ] && VERSION="Unreleased"

    COMMIT_SUBJECT=$(git log -1 --format="%s" "$COMMIT_HASH")
    COMMIT_AUTHOR=$(git log -1 --format="%aN <%aE>" "$COMMIT_HASH")
    COMMIT_DATE=$(git log -1 --format="%aD" "$COMMIT_HASH")
    COMMIT_BODY=$(git log -1 --format="%b" "$COMMIT_HASH")

    ENTRY="- **$COMMIT_SUBJECT**  \n  **Date:** $COMMIT_DATE  \n  **Author:** $COMMIT_AUTHOR  \n  **Commit:** \`$COMMIT_HASH\`"
    if [ -n "$COMMIT_BODY" ]; then
        ENTRY="$ENTRY  \n  $COMMIT_BODY"
    fi
    ENTRY="$ENTRY\n"

    if [ -n "${VERSION_COMMITS[$VERSION]}" ]; then
        VERSION_COMMITS[$VERSION]="${VERSION_COMMITS[$VERSION]}$ENTRY"
    else
        VERSION_COMMITS[$VERSION]="$ENTRY"
    fi
done

VERSIONS=$(printf "%s\n" "${!VERSION_COMMITS[@]}" | grep -v "Unreleased" | sort -V)

for VER in $VERSIONS; do
    FIRST_COMMIT=$(git rev-list --no-merges --reverse HEAD | while read H; do
        V=$(git show "$H:mod.json" 2>/dev/null | grep -o '\"version\": \"[0-9.]*\"' | awk -F'"' '{print $4}')
        [ "$V" = "$VER" ] && echo $H && break
    done)
    VER_DATE=$(git show -s --format=%ci "$FIRST_COMMIT" | cut -d ' ' -f 1)

    echo -e "\n## [$VER] - $VER_DATE\n" >> "$TEMP_FILE"
    echo -e "${VERSION_COMMITS[$VER]}" >> "$TEMP_FILE"
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
