#!/bin/bash

# Defaults
FILE="mod.json"
BRANCH="main"

# Parse options
while getopts "f:b:" opt; do
    case $opt in
        f) FILE="$OPTARG" ;;
        b) BRANCH="$OPTARG" ;;
        *) echo "Usage: $0 [-f file] [-b branch]"; exit 1 ;;
    esac
done

# Ensure script is run from repo root
if [ ! -d ".git" ]; then
    echo "Error: This script must be run from the root of a git repository."
    exit 1
fi

# Temporary branch to work on edits
TEMP_BRANCH="edit-file-temp-branch"
git checkout -b "$TEMP_BRANCH" "$BRANCH" || exit 1

# List all commits with short hash and message
echo "All commits on $BRANCH:"
git log --oneline --reverse

# Ask user which commits to edit (comma-separated short hashes)
read -p "Enter commit hashes to edit (comma-separated, in order): " HASHES_INPUT
IFS=',' read -r -a COMMITS <<< "$HASHES_INPUT"

# Iterate over selected commits
for COMMIT in "${COMMITS[@]}"; do
    COMMIT=$(echo "$COMMIT" | xargs)  # trim whitespace
    echo "--------------------------------------------"
    echo "Processing commit $COMMIT..."
    
    # Check out the commit in detached HEAD
    git checkout "$COMMIT" || { echo "Failed to checkout $COMMIT"; exit 1; }

    # Check if file exists in this commit
    if git show "$COMMIT:$FILE" >/dev/null 2>&1; then
        # Extract file to working directory
        git show "$COMMIT:$FILE" > "$FILE"
        echo "Editing $FILE for commit $COMMIT..."
        
        # Open editor
        ${EDITOR:-nano} "$FILE"

        # Stage and amend
        git add "$FILE"
        git commit --amend --no-edit
    else
        echo "File $FILE not found in commit $COMMIT. Skipping..."
    fi
done

# Rebase temp branch onto main to apply edits
git checkout "$BRANCH"
git reset --hard "$TEMP_BRANCH"

# Delete temp branch
git branch -D "$TEMP_BRANCH"

echo "Done! Changes applied directly to $BRANCH."
